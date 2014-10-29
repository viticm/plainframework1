/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id filebridge.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/25 21:32
 * @uses script lua module file bridge class
 */
#ifndef PF_SCRIPT_LUA_FILEBRIDGE_H_
#define PF_SCRIPT_LUA_FILEBRIDGE_H_

#include "pf/script/lua/config.h"

namespace pf_script {

namespace lua {

class PF_API FileBridge {

 public:
   FileBridge(const char *rootpath = NULL, const char *workpath = NULL);
   ~FileBridge();

 public:
   enum file_accessmode {
     kFileAccessModeBegin = 0,
     kFileAccessModeCurrent,
     kFileAccessModeEnd
   };

 public:
   void set_rootpath(const char *path);
   void set_workpath(const char *path);
   void get_fullpath(char *path, const char *filename, size_t length);
   const char *get_rootpath();
   
 public:
   bool open(const char *);
   void close();
   uint64_t read(void *, uint64_t);
   uint64_t write(void *, uint64_t);
   int64_t seek(int64_t, file_accessmode);
   int64_t tell();
   uint64_t size();

 private:
   FILE *fp_;
   uint64_t length_;
   int64_t position_;
   char rootpath_[FILENAME_MAX];
   char workpath_[FILENAME_MAX];

};

}; //namespace lua

}; //namespace pf_script

#endif //PF_SCRIPT_LUA_FILEBRIDGE_H_
