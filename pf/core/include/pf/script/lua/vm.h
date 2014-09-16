/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id vm.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/25 22:28
 * @uses script lua module Virtual Manufacturing
 */
#ifndef PF_SCRIPT_LUA_VM_H_
#define PF_SCRIPT_LUA_VM_H_

#include "pf/script/lua/config.h"
#include <lua.hpp>
#include "pf/script/lua/filebridge.h"

namespace pf_script {

namespace lua {

class PF_API VM {

 public:
   VM(const char *rootpath = NULL, const char *workpath = NULL);
   ~VM();

 public:
   typedef enum {
     kErrorCodeCreate = 1,
     kErrorCodeLength = 2,
     kErrorCodeLoadBuffer = 3,
     kErrorCodeExecute = 4,
     kErrorCodeNotNumber = 5,
     kErrorCodeNotString = 6,
     kErrorCodeNotTable = 7,
     kErrorCodeStateIsNil = 8,
     kErrorCodeResize = 9,
   } errorcode_t;

 public:
   lua_State *lua_state_;

 public:
   bool init(int32_t size = 0);
   void release();
   //注册全局方法
   bool register_function(const char *name, void *pointer);
   //注册方法到一个全局表中
   bool register_functiontable(
       const char *name, const struct luaL_Reg regtable[]);
   bool load(const char *filename);
   bool loadbuffer(unsigned char *buffer, uint64_t length);

 public:
   void callfunction_enter(int32_t *index);
   bool callfunction(const char *, int32_t, const char *, ...);
   bool callfunction(const char *, int32_t); //0
   bool callfunction(const char *, int32_t, int64_t); //1
   bool callfunction(const char *, int32_t, int64_t, int64_t); //2
   bool callfunction(const char *, int32_t, int64_t, int64_t, int64_t); //3
   //4
   bool callfunction(const char *, int32_t, int64_t, int64_t, int64_t, int64_t);
   bool callfunction(const char *,
                     int32_t,
                     int64_t,
                     int64_t,
                     int64_t,
                     int64_t,
                     int64_t); //5
   bool callfunction(const char *,
                     int32_t,
                     int64_t,
                     int64_t,
                     int64_t,
                     int64_t,
                     int64_t,
                     int64_t); //6
  bool callfunction(const char *,
                    int32_t,
                    int64_t,
                    int64_t,
                    int64_t,
                    int64_t,
                    int64_t,
                    int64_t,
                    int64_t); //7
  bool callfunction(const char *,
                    int32_t,
                    int64_t,
                    int64_t,
                    int64_t,
                    int64_t,
                    int64_t,
                    int64_t,
                    int64_t,
                    int64_t); //8
  bool callfunction(const char *,
                    int32_t,
                    int64_t,
                    int64_t,
                    float,
                    float); //9
  bool callfunction(const char *,
                    int32_t,
                    int64_t,
                    int64_t,
                    const char *,
                    const char *); //10
  void callfunction_leave(int32_t index);

 public:
   void set_rootpath(const char *path);
   void set_workpath(const char *path);
   const char *get_rootpath();

 private:
   bool executecode();
   void on_init();
   void on_scripterror(int32_t error);
   void on_scripterror(int32_t, int32_t);
   const char *get_lastresult();

 private:
   FileBridge filebridge_;

};

}; //namespace lua

}; //namespace pf_script

#endif //PF_SCRIPT_LUA_VM_H_
