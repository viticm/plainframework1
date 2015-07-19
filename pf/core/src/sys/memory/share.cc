#include "pf/base/log.h"
#include "pf/base/util.h"
#if __LINUX__
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#elif __WINDOWS__
#include <winbase.h>
#endif
#include "pf/sys/memory/share.h"

namespace pf_sys {

namespace memory { 

namespace share {

header_struct::header_struct() {                                           
  clear();
}                                                                                  

void header_struct::clear() {
  __ENTER_FUNCTION                                                                 
    key = 0;                                                                       
    size = 0;                                                                      
    initword(flag, kFlagFree);
    version = 0;                                                                   
    poolposition = 0;
  __LEAVE_FUNCTION                                                                 
}

void header_struct::lock(int8_t type) {
  share::lock(&flag, type);
}
  
void header_struct::unlock(int8_t type) {
  share::unlock(&flag, type);
}
                                                                                  
header_struct::~header_struct() {                                          
  //do nothing
}

dataheader_struct::dataheader_struct() {
  clear();
}

dataheader_struct::~dataheader_struct() {
  //do nothing
}

void dataheader_struct::clear() {
  __ENTER_FUNCTION
    key = 0;
    version = 0;
    usestatus = kUseFree;
    poolid = ID_INVALID;
    initword(flag, kFlagFree);
    savetime = 0;
  __LEAVE_FUNCTION
}

  
dataheader_t &dataheader_struct::operator = (const dataheader_t &object) {
  __ENTER_FUNCTION
    key = object.key;
    version = object.version;
    int64_t _flag = atomic_read(&object.flag);
    initword(flag, _flag);
    poolid = object.poolid;
    savetime = object.savetime;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
dataheader_t *dataheader_struct::operator = (const dataheader_t *object) {
  __ENTER_FUNCTION
    if (object) {
      key = object->key;
      version = object->version;
      int64_t _flag = atomic_read(&object->flag);
      initword(flag, _flag);
      poolid = object->poolid;
      savetime = object->savetime;
    }
    return this;
  __LEAVE_FUNCTION
    return this;
}
 
//struct end --

namespace api {

#if __LINUX__
int32_t create(uint32_t key, uint32_t size) {
  int32_t handle = 0;
#elif __WINDOWS__
HANDLE create(uint32_t key, uint32_t size) {
    HANDLE handle = NULL;
#endif
  __ENTER_FUNCTION
#if __LINUX__
    handle = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
    if (HANDLE_INVALID == handle) {
      SLOW_ERRORLOG(
          APPLICATION_NAME,
          "[sys.memory.share] (api::create) handle = %d," 
          " key = %d, error: %d",
          handle, 
          key, 
          errno);
    }
#elif __WINDOWS__
    char buffer[65];
    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer) - 1, "%d", key);
    handle = (CreateFileMapping(reinterpret_cast<HANDLE>(0xFFFFFFFFFFFFFFFF), 
                                NULL, 
                                PAGE_READWRITE, 
                                0, 
                                size, 
                                buffer));
#endif
    return handle;
  __LEAVE_FUNCTION
    return handle;
}
#if __LINUX__
int32_t open(uint32_t key, uint32_t size) {
  int32_t handle = 0;
#elif __WINDOWS__
HANDLE open(uint32_t key, uint32_t size) {
  HANDLE handle = NULL;
#endif
  __ENTER_FUNCTION
    USE_PARAM(size);
#if __LINUX__
    handle = shmget(key, size, 0);
    if (HANDLE_INVALID == handle) {
      SLOW_ERRORLOG(
          APPLICATION_NAME, 
          "[sys.memory.share] (api::open) handle = %d,"
          " key = %d, error: %d", 
          handle, 
          key, 
          errno);
    }
#elif __WINDOWS__
    char buffer[65];
    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer) - 1, "%d", key);
    handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, true, buffer);
#endif
    return handle;
  __LEAVE_FUNCTION
    return handle;
}

#if __LINUX__
char *map(int32_t handle) {
#elif __WINDOWS__
char *map(HANDLE handle) {
#endif
  __ENTER_FUNCTION
    char *result;
#if __LINUX__
    result = static_cast<char*>(shmat(handle, 0, 0));
#elif __WINDOWS__
    result = 
      static_cast<char*>(MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, 0));
#endif
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

void unmap(char *pointer) {
  __ENTER_FUNCTION
#if __LINUX__
    shmdt(pointer);
#elif __WINDOWS__
    UnmapViewOfFile(pointer);
#endif
  __LEAVE_FUNCTION
}

#if __LINUX__
void close(int32_t handle) {
#elif __WINDOWS__
void close(HANDLE handle) {
#endif
  __ENTER_FUNCTION
#if __LINUX__
    shmctl(handle, IPC_RMID, 0);
#elif __WINDOWS__
    CloseHandle(reinterpret_cast<HANDLE>(handle));
#endif
  __LEAVE_FUNCTION
}

} //namespace api


//-- class start
Base::Base() {
  __ENTER_FUNCTION
    data_pointer_ = NULL;
    handle_ = HANDLE_INVALID;
    size_ = 0;
    header_ = NULL;
  __LEAVE_FUNCTION
}

Base::~Base() {
  //do nothing
}

bool Base::create(uint32_t key, uint32_t size) {
  __ENTER_FUNCTION
    if (kCmdModelClearAll == cmd_model_) return false;
    handle_ = api::create(key, size);
    if (HANDLE_INVALID == handle_) {
      SLOW_ERRORLOG(
          APPLICATION_NAME, 
          "[sys.memory.share] (Base::create)"
          " failed! handle = %d, key = %d",
          handle_, 
          key);
      return false;
    }
    header_ = api::map(handle_);
    if (header_) {
      data_pointer_ = header_ + sizeof(header_t);
      getheader()->clear();
      getheader()->key = key;
      getheader()->size = size;
      size_ = size;
      SLOW_LOG(
          APPLICATION_NAME, 
          "[sys.memory.share] (Base::create)"
          " success! handle = %d, key = %d",
          handle_, 
          key);
      return true;
    } else {
      SLOW_ERRORLOG(
          APPLICATION_NAME, 
          "[sys.memory.share] (Base::create)"
          "map failed! handle = %d, key = %d", 
          handle_, 
          key);
      return false;
    }
  __LEAVE_FUNCTION
    return false;
}

void Base::destory() {
  __ENTER_FUNCTION
    if (header_) {
      api::unmap(header_);
      header_ = NULL;
    }
    if (handle_) {
      api::close(handle_);
#if __LINUX__
      handle_ = 0;
#elif __WINDOWS__
      handle_ = NULL;
#endif
    }
    size_ = 0;
  __LEAVE_FUNCTION
}

bool Base::attach(uint32_t key, uint32_t size) {
  __ENTER_FUNCTION
    handle_ = api::open(key, size);
    if (kCmdModelClearAll == cmd_model_) {
      destory();
      SLOW_LOG(
          APPLICATION_NAME,
          "[sys.memory.share] (Base::attach) close memory, key = %d", 
          key);
      return false;
    }
    if (HANDLE_INVALID == handle_) {
      SLOW_ERRORLOG(
          APPLICATION_NAME, 
          "[sys.memory.share] (Base::attach) failed, key = %d", 
          key); 
      return false;
    }
    header_ = api::map(handle_);
    if (header_) {
      data_pointer_ = header_ + sizeof(header_t);
      Assert(getheader()->key == key);
      Assert(getheader()->size == size);
        size_ = size;
      SLOW_LOG(
          APPLICATION_NAME, 
          "[sys.memory.share] (Base::attach) success, key = %d", 
          key); 
      return true;
    } else {
      SLOW_ERRORLOG(
          APPLICATION_NAME, 
          "[sys.memory.share] (Base::attach) map failed, key = %d", 
          key); 
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

header_t *Base::getheader() {
  return reinterpret_cast<header_t *>(header_);
}

char *Base::get_data_pointer() {
  return data_pointer_;
}

char *Base::get_data(uint32_t size, uint32_t index) {
  __ENTER_FUNCTION
    Assert(size > 0);
    Assert(size * index < size_);
    char *result;
    result = 
      (size <= 0 || size * index > size_) ? NULL : data_pointer_ + size * index;
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

uint32_t Base::get_size() const {
  __ENTER_FUNCTION
    return size_;
  __LEAVE_FUNCTION
    return 0;
}

bool Base::dump(const char *filename) {
  __ENTER_FUNCTION
    Assert(filename);
    FILE* fp = fopen(filename, "wb");
    if (!fp) return false;
    fwrite(header_, 1, size_, fp);
    fclose(fp);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Base::merge_from_file(const char *filename) {
  __ENTER_FUNCTION
    Assert(filename);
    FILE* fp = fopen(filename, "rb");
    if (!fp) return false;
    fseek(fp, 0L, SEEK_END);
    int32_t filelength = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    fread(header_, filelength, 1, fp);
    fclose(fp);
    return true;
  __LEAVE_FUNCTION
    return false;
}

//class end --

//-- functions start

void lock(atword_t *flag, int8_t type) {
  __ENTER_FUNCTION
    USE_PARAM(type);
    if (GLOBAL_VALUES["app_cmdmodel"] == kCmdModelRecover ||
        GLOBAL_VALUES["app_status"] == kAppStatusStop) return;
    int32_t count = 0;
#if __LINUX__
    while (!atomic_inc_and_test(flag)) {
      if (GLOBAL_VALUES["app_status"] == kAppStatusStop) break;
      int32_t count1 = 0;
      ++count;
      ++count1;
      atomic_dec(flag);
      pf_base::util::sleep(0);
      if (count > 1000) {
        SLOW_ERRORLOG(APPLICATION_NAME, 
                      "[sys.memory] (share::lock) error, flag: %d, count: %d",
                      *flag,
                      count1);
        count = 0;
      }
    }
#elif __WINDOWS__
    while (InterlockedCompareExchange(
            const_cast<LPLONG>(flag), 0, kFlagFree) != kFlagFree) {
      if (GLOBAL_VALUES["app_status"] == kAppStatusStop) break;
      ++count;
      pf_base::util::sleep(0);
      if (count > 100) {
        char time_str[256] = {0};
        pf_base::Log::get_log_timestr(time_str, sizeof(time_str) - 1);
        ERRORPRINTF("%s[sys.memory] (share::lock) failed", time_str);
        count = 0;
      }
    }
#endif
  __LEAVE_FUNCTION
}

void unlock(atword_t *flag, int8_t type) {
  __ENTER_FUNCTION
    USE_PARAM(type);
    if (GLOBAL_VALUES["app_cmdmodel"] == kCmdModelRecover ||
        GLOBAL_VALUES["app_status"] == kAppStatusStop) return;
    //(int32_t)(int64_t)((int32_t*)flag);
#if __LINUX__
    if (atomic_read(flag) > kFlagFree) atomic_dec(flag);
#elif __WINDOWS__
    uint32_t count = 0;
    while (InterlockedCompareExchange(
            const_cast<LPLONG>(flag), kFlagFree, 0) != 0) {
      if (GLOBAL_VALUES["app_status"] == kAppStatusStop) break;
      char time_str[256] = {0};
      pf_base::Log::get_log_timestr(time_str, sizeof(time_str) - 1);
      ERRORPRINTF("%s[sys.memory] (share::unlock) failed", time_str);
      pf_base::util::sleep(0);
      ++count;
      if (count > 100) {
        InterlockedExchange(const_cast<LPLONG>(flag), kFlagFree);
        throw;
      }
    }
#endif
  __LEAVE_FUNCTION
}

//functions end --

} //namespace share

} //namespace share

} //namespace pf_sys
