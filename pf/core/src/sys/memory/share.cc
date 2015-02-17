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

int32_t g_cmd_model = 0;

namespace pf_sys {

namespace memory { 

namespace share {

//-- struct start
dataheader_struct::dataheader_struct() {
  __ENTER_FUNCTION
    key = 0;
    size = 0;
    version = 0;
  __LEAVE_FUNCTION
}

dataheader_struct::~dataheader_struct() {
  //do nothing
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
    SLOW_ERRORLOG(
        APPLICATION_NAME,
        "[sys.memory.share] (api::create) handle = %d," 
        " key = %d, error: %d",
        handle, 
        key, 
        errno);
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
    SLOW_ERRORLOG(
        APPLICATION_NAME, 
        "[sys.memory.share] (api::open) handle = %d,"
        " key = %d, error: %d", 
        handle, 
        key, 
        errno);
#elif __WINDOWS__
    char buffer[65];
    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer) - 1, "%"PRIu64, key);
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
    handle_ = 0;
    size_ = 0;
    header_ = 0;
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
      data_pointer_ = header_ + sizeof(dataheader_t);
      (reinterpret_cast<dataheader_struct *>(header_))->key = key;
      (reinterpret_cast<dataheader_struct *>(header_))->size = size;
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
      data_pointer_ = header_ + sizeof(dataheader_t);
      Assert((reinterpret_cast<dataheader_struct *>(header_))->key == key);
      Assert((reinterpret_cast<dataheader_struct *>(header_))->size == size);
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
  __LEAVE_FUNCTION
    return false;
}

char *Base::get_data_pointer() {
  __ENTER_FUNCTION
    return data_pointer_;
  __LEAVE_FUNCTION
    return NULL;
}

char *Base::get_data(uint32_t size, uint32_t index) {
  __ENTER_FUNCTION
    Assert(size > 0);
    Assert(size * index < size_);
    char *result;
    result = (size <= 0 || index > size_) ? NULL : data_pointer_ + size * index;
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

void Base::set_head_version(uint32_t version) {
  __ENTER_FUNCTION
    (reinterpret_cast<dataheader_struct *>(header_))->version = version;
  __LEAVE_FUNCTION
}

uint32_t Base::get_head_version() const {
  __ENTER_FUNCTION
    uint32_t version = 
      (reinterpret_cast<dataheader_struct *>(header_))->version;
    return version;
  __LEAVE_FUNCTION
    return 0;
}

//class end --

//-- functions start

void lock(atword_t *flag, int8_t type) {
  __ENTER_FUNCTION
    USE_PARAM(type);
    if (kCmdModelRecover == g_cmd_model) return;
    int32_t count = 0;
#if __LINUX__
    while (!atomic_inc_and_test(flag)) {
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
            const_cast<LPLONG>(flag), 0, kUseFree) != kUseFree) {
      ++count;
      pf_base::util::sleep(0);
      if (count > 10) {
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
    if (kCmdModelRecover == g_cmd_model) return;
#if __LINUX__
    if ((int32_t)(int64_t)((int32_t*)flag) != kUseFree) atomic_dec(flag);
#elif __WINDOWS__
    uint32_t count = 0;
    while (InterlockedCompareExchange(
            const_cast<LPLONG>(flag), kUseFree, 0) != 0) {
      char time_str[256] = {0};
      pf_base::Log::get_log_timestr(time_str, sizeof(time_str) - 1);
      ERRORPRINTF("%s[sys.memory] (share::unlock) failed", time_str);
      pf_base::util::sleep(0);
      ++count;
      if (count > 100) {
        InterlockedExchange(const_cast<LPLONG>(flag), kUseFree);
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
