#include "pf/base/util.h"
#include "pf/base/string.h"
#include "pf/base/log.h"
#include "pf/script/lua/filebridge.h"

using namespace pf_base;

namespace pf_script {

namespace lua {

FileBridge::FileBridge(const char *rootpath, const char *workpath) {
  __ENTER_FUNCTION
    fp_ = NULL;
    length_ = 0;
    position_ = 0;
    memset(rootpath_, 0, sizeof(rootpath_));
    memset(workpath_, 0, sizeof(workpath_));
    if (NULL == rootpath) {
      string::safecopy(rootpath_, 
                       SCRIPT_ROOT_PATH_DEFAULT, 
                       sizeof(rootpath_));
    }
    else {
      string::safecopy(rootpath_, rootpath, sizeof(rootpath_));
    }
    if (NULL == workpath) {
      string::safecopy(workpath_,
                       SCRIPT_WORK_PATH_DEFAULT,
                       sizeof(workpath_));
    }
    else {
      string::safecopy(workpath_, workpath, sizeof(workpath_));
    }
  __LEAVE_FUNCTION
}

FileBridge::~FileBridge() {
  __ENTER_FUNCTION
    close();
  __LEAVE_FUNCTION
}

bool FileBridge::open(const char *filename) {
  __ENTER_FUNCTION
    if (fp_) close();
    char filepath[FILENAME_MAX] = {0};
    get_fullpath(filepath, filename, sizeof(filepath) - 1);
#if __WINDOWS__
    util::path_tounix(filepath, sizeof(filepath) - 1);
    char casepath[FILENAME_MAX] = {0};
    string::safecopy(casepath, filepath, sizeof(casepath));
    fp_ = fopen(casepath, "rb");
#endif
    fp_ = fp_ ? fp_ : fopen(filepath, "rb");
    if (NULL == fp_) {
      SLOW_ERRORLOG(SCRIPT_MODULENAME, 
                    "[script][lua] (FileBridge::open) file error: %s", 
                    filepath);  
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void FileBridge::close() {
  __ENTER_FUNCTION
    if (fp_) fclose(fp_);
    fp_ = NULL;
    length_ = 0;
    position_ = 0;
  __LEAVE_FUNCTION
}

uint64_t FileBridge::read(void *buffer, uint64_t read_bytes) {
  __ENTER_FUNCTION
    if (!fp_) return 0;
    uint64_t _read_bytes;
    _read_bytes = fread(buffer, 1, static_cast<size_t>(read_bytes), fp_);  
    position_ += _read_bytes;
    return _read_bytes;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t FileBridge::write(void *buffer, uint64_t write_bytes) {
  __ENTER_FUNCTION
    if (!fp_) return 0;
    uint64_t _write_bytes;
    _write_bytes = fwrite(buffer, 1, static_cast<size_t>(write_bytes), fp_); 
    position_ += _write_bytes;
    return _write_bytes;
  __LEAVE_FUNCTION
    return 0;
}

int64_t FileBridge::seek(int64_t position, file_accessmode accessmode) {
  __ENTER_FUNCTION
    if (!fp_) return -1;
    fseek(fp_, static_cast<long>(position), accessmode);
    position_ = ftell(fp_);
    return position_;
  __LEAVE_FUNCTION
    return -1;
}

int64_t FileBridge::tell() {
  __ENTER_FUNCTION
    if (!fp_) return -1;
    return position_;
  __LEAVE_FUNCTION
    return -1;
}

uint64_t FileBridge::size() {
  __ENTER_FUNCTION
    if (!fp_) return 0;
    if (0 == length_) {
      uint64_t temp = position_;
      length_ = static_cast<uint64_t>(seek(0, kFileAccessModeEnd));
      seek(temp, kFileAccessModeBegin);
    }
    return length_;
  __LEAVE_FUNCTION
    return 0;
}

void FileBridge::set_rootpath(const char *path) {
  __ENTER_FUNCTION
    string::safecopy(rootpath_, path, sizeof(rootpath_));
  __LEAVE_FUNCTION
}

void FileBridge::set_workpath(const char *path) {
  __ENTER_FUNCTION
    string::safecopy(workpath_, path, sizeof(workpath_));
  __LEAVE_FUNCTION
}

void FileBridge::get_fullpath(char *path, 
                              const char *filename, 
                              size_t length) {
  __ENTER_FUNCTION
    if (':' == filename[1]) {
      string::safecopy(path, filename, length);
    }
    if ('\\' == filename[0] || '/' == filename[0]) {
      string::safecopy(path, rootpath_, length);
      strncat(path, filename, length - strlen(path));
      return;
    }
#if __WINDOWS__
    if (':' == workpath_[1]) {
      string::safecopy(path, workpath_, length);
      strncat(path, filename, length - strlen(path));
      return;
    }
#endif 
    string::safecopy(path, rootpath_, length);
    if (workpath_[0] != '\\' && workpath_[0] != '/') {
#if __WINDOWS__ /* { */
      strncat(path, "\\", length - strlen(path)); 
#elif __LINUX__ /* }{ */
      strncat(path, "/", length - strlen(path)); 
#endif /* } */
    }
    strncat(path, workpath_, length - strlen(path));
    if ('.' == filename[0] && ('\\' == filename[1] || '/' == filename[1])) {
      strncat(path, filename + 2, length - strlen(path));      
    }
    else {
      strncat(path, filename, length - strlen(path));
    }
  __LEAVE_FUNCTION
}

const char *FileBridge::get_rootpath() {
  __ENTER_FUNCTION
    return rootpath_;
  __LEAVE_FUNCTION
    return NULL;
}

} //namespace lua

} //namespace pf_script
