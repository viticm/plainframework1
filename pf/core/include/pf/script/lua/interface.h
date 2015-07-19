/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id system.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/26 16:48
 * @uses script lua module system class(script interface)
 *       lua脚本系统基础接口，如需扩展，继承该类
 *       这部分代码将重构，调用lua方法时使用少许接口
 *       VM虚拟机的方法不支持多线程同时调用，该部分代码需要对多线程进行支持改进
 */
#ifndef PF_SCRIPT_LUA_INTERFACE_H_
#define PF_SCRIPT_LUA_INTERFACE_H_

#ifndef PF_CORE_WITH_NOLUA

#include "pf/script/lua/config.h"
#include "pf/base/hashmap/template.h"
#include "pf/sys/thread.h"
#include "pf/script/cache/base.h" 
#include "pf/script/lua/vm.h"
#include "pf/script/lua/stack.h"

namespace pf_script {

namespace lua {

class PF_API Interface {

 public:
   Interface();
   ~Interface();

 public:
   static bool lua_reloadscript_always_;
   static bool lua_recordscript_step_;
   static const char *kLuaScriptStepFileName;
   static const int32_t kLuaScriptCountMax;

 public:
   bool init();
   void release();
   bool find_function(const char *name);
   bool verify_function(const char **names);
   static bool find_function(lua_State *L, const char *name);
   static bool verify_function(lua_State *L, const char **names);
   void enter_runstep(int32_t scriptid, 
                      const char *functionname);
   void leave_runstep(int32_t scriptid, 
                      const char *functionname);
   void enter_runstep(const char *functionname);
   void leave_runstep(const char *functionname);
   bool reloadscript(int32_t scriptid);
   void *getscript_byid(int32_t scriptid);
   bool addscript(int32_t scriptid, void *data);
   static cache::Base *getscript_filedata(int32_t scriptid);

 public:
   bool loadscript(const char *filename);
   //重载脚本，支持多线程同时调用
   bool reload(const char *filename);
   void set_globalfile(const char *filename);
   void set_rootpath(const char *path);
   void set_workpath(const char *path);
   const char *get_rootpath();
   VM *get_vm();

 public:
   static bool check_paramnumber(lua_State *L, int32_t count);
   static bool is_paramnumber(lua_State *L, 
                              int32_t index, 
                              const char *function_name);
   static bool is_paramstring(lua_State *L,
                              int32_t index,
                              const char *function_name);
   static int32_t getscript_stepid(lua_State *L);
   static void mark_scriptname(lua_State *L, const char *scriptname);
   static void check_scriptvalid(void *file, 
                                 int32_t scriptid, 
                                 const char *function_name);

 public:
   //执行字符串的方法，支持多线程同时调用
   int64_t runstring(const char *string);
   //执行带参数和结果的方法，支持多线程同时调用
   bool runfunction(const char *name,
                    pf_base::variable_array_t &params,
                    pf_base::variable_array_t &results);

 private:
   VM VM_;
   pf_sys::ThreadLock lock_;
   pf_base::hashmap::Template<int32_t, void *> script_loaded_;
   char global_filename_[FILENAME_MAX];

};
  
}; //namespace lua

}; //namespace pf_script

#else

#include "pf/script/lua/none.h"

#endif

#endif //PF_SCRIPT_LUA_INTERFACE_H_
