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

#ifndef PF_CORE_WITH_NOLUA

#include "pf/script/lua/config.h"
#include <lua.hpp>
#include "pf/base/hashmap/template.h"
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
   //注册全局变量
   bool register_globalvalue(const char *name, const char *value);
   bool register_globalvalue(const char *name, lua_Number value);
   bool register_ref(const char *table, const char *field);
   bool get_ref(const char *table, const char *field);
   bool unregister_ref(const char *table, const char *field);
   void unregister_refs();
   void checkgc(int32_t freetime);

 public:
  void set_rootpath(const char *path);
  void set_workpath(const char *path);
  const char *get_rootpath();

 protected:
   bool load(const char *filename);
   bool loadbuffer(unsigned char *buffer, uint64_t length);
   void callfunction_enter(int32_t *index);
   bool callstring(const char *);
   bool callfunction(const char *name,
     pf_base::variable_array_t &params,
     pf_base::variable_array_t &results);
   void callfunction_leave(int32_t index);

 private:
   FileBridge filebridge_;
   std::map<std::string, int32_t> refs_;
   friend class Interface;

 private:
   bool executecode();
   void on_init();
   void on_scripterror(int32_t error);
   void on_scripterror(int32_t, int32_t);
   const char *get_lastresult();

};

}; //namespace lua

}; //namespace pf_script

#endif

#endif //PF_SCRIPT_LUA_VM_H_
