#include <sys/stat.h>
#include "pf/file/api.h"
#include "pf/base/string.h"
#include "pf/base/util.h"
#include "pak/util.h"
#include "pak/file.h"

//The old code use open functions, 
//I find it can't work in unix/linux, so changed to fopen/fclose/fseek

namespace pak {

namespace file {

void *createex(const char *_filename, 
               uint64_t mode, 
               uint64_t sharing, 
               void *secondattribute, 
               uint64_t creation, 
               uint64_t flag, 
               void *file) {
  __ENTER_FUNCTION
    void *result = HANDLE_INVALID_VALUE;
    char filename[FILENAME_MAX] = {0};
    pf_base::string::safecopy(filename, _filename, sizeof(filename));
    pf_base::util::path_tounix(filename, static_cast<uint16_t>(strlen(filename)));
#if __LINUX__
    //the fileopen mode: "a" add content to the end, "w" can rewrite data
    switch (creation) {
      case OPEN_EXISTING:
        result = 
          reinterpret_cast<void *>(fopen(filename, "rb+"));
        break;
      case OPEN_ALWAYS:
        result = reinterpret_cast<void *>(fopen(filename, "rb+"));
        break;
      case CREATE_ALWAYS:
        result = reinterpret_cast<void *>(fopen(filename, "wb+"));
        break;
      case CREATE_NEW:
        result = reinterpret_cast<void *>(fopen(filename, "wb+"));
        break;
      default:
        break;
    }
#elif __WINDOWS__
    result = 
      CreateFile(filename, 
                 static_cast<DWORD>(mode), 
                 static_cast<DWORD>(sharing), 
                 (LPSECURITY_ATTRIBUTES)secondattribute, 
                 static_cast<DWORD>(creation), 
                 static_cast<DWORD>(flag), 
                 file);
#endif
    //DEBUGPRINTF("filename: %s fp: 0x%08x", filename, result);
    if (HANDLE_INVALID_VALUE == result)
      util::set_lasterror(PAK_ERROR_FILE_CREATE);
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

bool closeex(handle_t fp) {
  bool result = false;
#if __LINUX__ 
  result = true;
  FILE *_fp = reinterpret_cast<FILE *>(fp);
  fclose(_fp);
#elif __WINDOWS__
  result = (1 == CloseHandle(fp));
#endif
  if (false == result)
    util::set_lasterror(PAK_ERROR_FILE_CREATE);
  return result;
}

uint64_t getszie(void *fp, uint64_t *offsethigh) {
  __ENTER_FUNCTION
    uint64_t result;
    if (HANDLE_INVALID_VALUE == fp) return 0xffffffff;
#if __LINUX__
    struct stat fileinfo;
    int32_t _fp = fileno(reinterpret_cast<FILE *>(fp));
    fstat(_fp, &fileinfo);
    result = fileinfo.st_size;
#elif __WINDOWS__
    result = GetFileSize(fp, reinterpret_cast<LPDWORD>(offsethigh));
#endif
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t setpointer(void *fp, 
                    int64_t offsetlow, 
                    int64_t *offsethigh, 
                    uint64_t method) {
  __ENTER_FUNCTION
    uint64_t result;
#if __LINUX__
    FILE *_fp = reinterpret_cast<FILE *>(fp);
    off64_t offset = static_cast<off64_t>(offsetlow);
    if (offsethigh != NULL)
      offset |= (*(off64_t *)offsethigh) << 32;
    result = fseek(_fp, offset, method);
#elif __WINDOWS__
    result = SetFilePointer(fp, 
                            static_cast<LONG>(offsetlow), 
                            reinterpret_cast<PLONG>(offsethigh), 
                            static_cast<DWORD>(method));
#endif
    return result;
  __LEAVE_FUNCTION
    return 0;
}

bool setend(void *fp) {
  __ENTER_FUNCTION
    bool result = false;
#if __LINUX__
    FILE *_fp = reinterpret_cast<FILE *>(fp);
    result = (0 == fseek(_fp, 0, SEEK_END));
#elif __WINDOWS__
    result = 1 == SetEndOfFile(fp);
#endif
  __LEAVE_FUNCTION
    return false;
}

bool readex(void *fp, 
            void *buffer, 
            uint64_t length, 
            uint64_t *_read, 
            void *overlapped) {
  __ENTER_FUNCTION
    bool result = false;
#if __LINUX__
    FILE *_fp = reinterpret_cast<FILE *>(fp);
    ssize_t count;
    count = fread(buffer, 1, length, _fp);
    if (-1 == count) {
      *_read = 0;
    } else {
      *_read = count;
      result = true;
    }
#elif __WINDOWS__
    result = 1 == ReadFile(fp, 
                           buffer, 
                           static_cast<DWORD>(length), 
                           (LPDWORD)_read, 
                           (LPOVERLAPPED)overlapped);
#endif
    if (false == result)
      util::set_lasterror(PAK_ERROR_FILE_READ);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool writeex(void *fp, 
             const void *buffer, 
             uint64_t length, 
             uint64_t *_write, 
             void *overlapped) {
  __ENTER_FUNCTION
    bool result = false;
#if __LINUX__
    FILE *_fp = reinterpret_cast<FILE *>(fp);
    ssize_t count;
    if (-1 == (count = fwrite(buffer, 1, length, _fp))) {
      *_write = 0;
    } else {
      fflush(_fp);
      *_write = count;
      result = true;
    }
#elif __WINDOWS__
    result = 1 == WriteFile(fp, 
                            buffer, 
                            static_cast<DWORD>(length), 
                            (LPDWORD)_write, 
                            (LPOVERLAPPED)overlapped);
#endif
   if (false == result)
      util::set_lasterror(PAK_ERROR_FILE_WRITE);
    return result;
  __LEAVE_FUNCTION
    return false;
}

uint64_t getattribute(const char *filename) {
  __ENTER_FUNCTION
    uint64_t result = 0;
#if __LINUX__
    //do nothing
#elif __WINDOWS__
    result = GetFileAttributes(filename);
#endif
    return result;
  __LEAVE_FUNCTION
    return 0;
}

void get_temppath(uint64_t length, char *temp) {
#if __LINUX__
  strncpy(temp, P_tmpdir, length);
#elif __WINDOWS__
  GetTempPath(static_cast<DWORD>(length), temp);
#endif
}

void get_temp_filename(const char *temp_directorypath, 
                       const char *filename, 
                       uint64_t something, 
                       char *delimiter) {
  __ENTER_FUNCTION
#if __LINUX__
    char tempname[15] = "/tmp/sl.XXXXXX";
    int32_t create_tmp = mkstemp(tempname);
    if (create_tmp != -1) {
      close(create_tmp);
      strcpy(delimiter, tempname);
    }
#elif __WINDOWS__
    GetTempFileName(temp_directorypath, filename, static_cast<UINT>(something), delimiter);
#endif
  __LEAVE_FUNCTION
}

bool removeex(const char *filename) {
  bool result = false;
#if __LINUX__
  result = 0 == remove(filename);
#elif __WINDOWS__
  result = 1 == DeleteFile(filename);
#endif
  if (false == result) util::set_lasterror(PAK_ERROR_FILE_REMOVE);
  return result;
}

bool move(const char *source, const char *target) {
  __ENTER_FUNCTION
    bool result = false;
#if __LINUX__
    result = 0 == rename(source, target);
#elif __WINDOWS__
    result = 1 == MoveFile(source, target);
#endif
    if (false == result) util::set_lasterror(PAK_ERROR_FILE_MOVE);
    return result;
  __LEAVE_FUNCTION
    return false;
}

} //namespace file

} //namespace pak
