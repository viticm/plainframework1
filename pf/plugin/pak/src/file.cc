#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pak/file.h"

namespace pak {

namespace file {

void *createex(const char *filename, 
               uint64_t mode, 
               uint64_t sharing, 
               void *secondattribute, 
               uint64_t creation, 
               uint64_t flag, 
               void *file) {
  __ENTER_FUNCTION
    void *result = NULL;
#if __LINUX__
    switch (sharing) {
      case OPEN_EXISTING:
        result = 
          reinterpret_cast<void *>(open(filename, O_RDONLY | O_LARGEFILE));
        break;
      case OPEN_ALWAYS:
        result = reinterpret_cast<void *>(open(filename, O_RDWR | O_CREAT));
        break;
      case CREATE_ALWAYS:
        break;
      case CREATE_NEW:
        result = 
          reinterpret_cast<void *>(open(filename, O_RDWR | O_CREAT | O_TRUNC));
        break;
      default:
        break;
    }
#elif __WINDOWS__
    result = 
      CreateFile(filename, mode, sharing, secondattribute, creation, flag, file);
#endif
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

bool closeex(handle_t fp) {
  bool result = false;
#if __LINUX__ 
  int32_t _fp = static_cast<int32_t>(reinterpret_cast<int64_t>(fp));
  result = (0 == close(_fp));
#elif __WINDOWS__
  result = (1 == CloseHandle(fp));
#endif
  return result;
}

uint64_t getszie(void *fp, uint64_t *offsethigh) {
  __ENTER_FUNCTION
    uint64_t result;
#if __LINUX__
    if (NULL == fp) return 0xffffffff;
    struct stat fileinfo;
    int32_t _fp = static_cast<int32_t>(reinterpret_cast<int64_t>(fp));
    fstat(_fp, &fileinfo);
    result = fileinfo.st_size;
#elif __WINDOWS__
    result = GetFileSize(fp, offsethigh);
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
    int32_t _fp = static_cast<int32_t>(reinterpret_cast<int64_t>(fp));
    off64_t offset = static_cast<off64_t>(offsetlow);
    if (offsethigh != NULL)
      offset |= (*(off64_t *)offsethigh) << 32;
    result = lseek64(_fp, offset, method);
#elif __WINDOWS__
    result = SetFilePointer(fp, offsetlow, offsethigh, method);
#endif
  __LEAVE_FUNCTION
    return 0;
}

bool setend(void *fp) {
  __ENTER_FUNCTION
    bool result = false;
#if __LINUX__
    int32_t _fp = static_cast<int32_t>(reinterpret_cast<int64_t>(fp));
    result = (0 == ftruncate(_fp, lseek(_fp, 0, SEEK_CUR)));
#elif __WINDOWS__
    result = SetEndOfFile(fp);
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
    ssize_t count;
    int32_t _fp = static_cast<int32_t>(reinterpret_cast<int64_t>(fp));
    if (-1 == (count = read(_fp, buffer, length))) {
      *_read = 0;
    } else {
      *_read = count;
      result = true;
    }
#elif __WINDOWS__
    result = 1 == ReadFile(fp, buffer, length, length, _read, overlapped);
#endif
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
    int32_t _fp = static_cast<int32_t>(reinterpret_cast<int64_t>(fp));
    ssize_t count;
    if (-1 == (count = write(_fp, buffer, length))) {
      *_write = 0;
    } else {
      *_write = count;
      result = true;
    }
#elif __WINDOWS__
    result = 1 == ReadFile(fp, buffer, length, length, _write, overlapped);
#endif
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
  __LEAVE_FUNCTION
    return 0;
}

void get_temppath(uint64_t length, char *temp) {
#if __LINUX__
  strncpy(temp, P_tmpdir, length);
#elif __WINDOWS__
  GetTempPath(length, temp);
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
    GetTempFileName(temp_directorypath, filename, something, delimiter);
#endif
  __LEAVE_FUNCTION
}

bool removex(const char *filename) {
  bool result = false;
#if __LINUX__
  result = 0 == remove(filename);
#elif __WINDOWS__
  result = 1 == DeleteFile(filename);
#endif
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
    return result;
  __LEAVE_FUNCTION
    return false;
}

} //namespace file

} //namespace pak
