/**
 * PAP Engine ( https://github.com/viticm/plainframework1 )
 * $Id none.h
 * @link https://github.com/viticm/plainframework1 for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com/viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com/viticm.ti@gmail.com>
 * @date 2015/05/11 13:20
 * @uses your description
 */
#ifndef PF_SCRIPT_LUA_NONE_H_
#define PF_SCRIPT_LUA_NONE_H_

#include "pf/script/lua/config.h"

namespace pf_script {

namespace lua {

class PF_API Interface {

 public:
   Interface() {};
   ~Interface() {};

 public:
   bool init() { return false; };
   void release() {};
   bool find_function(const char *) { return true; };
   bool verify_function(const char **) {  return true; };
   static bool find_function(void *, const char *) { return true; };
   static bool verify_function(void *, const char **) { return true; };
   void enter_runstep(int32_t, const char *) {};
   void leave_runstep(int32_t, const char *) {};
   void enter_runstep(const char *) {};
   void leave_runstep(const char *) {};
   bool reloadscript(int32_t) { return true; };
   void *getscript_byid(int32_t) { return NULL; };
   bool addscript(int32_t, void *) { return true; };
   static cache::Base *getscript_filedata(int32_t) { return NULL; };

 public:
   bool loadscript(const char *) { return true; };
   //重载脚本，支持多线程同时调用
   bool reload(const char *) { return true; };
   void set_globalfile(const char *) {};
   void set_rootpath(const char *) {};
   void set_workpath(const char *) {};
   const char *get_rootpath() { return NULL; };
   VM *get_vm() { return NULL; };

 public:
   static bool check_paramnumber(void *, int32_t) { return true; };
   static bool is_paramnumber(void *, int32_t, const char *) { return true; };
   static bool is_paramstring(void *, int32_t, const char *) { return true; };
   static int32_t getscript_stepid(void *) { return -1; };
   static void mark_scriptname(void *, const char *) {};
   static void check_scriptvalid(void *, int32_t, const char *) {};

 public:
   //执行字符串的方法，支持多线程同时调用
   int64_t runstring(const char *) { return -1; };
   //执行带参数和结果的方法，支持多线程同时调用
   bool runfunction(const char *,
                    pf_base::variable_array_t &,
                    pf_base::variable_array_t &) { return true; };

};
  
}; //namespace lua

}; //namespace pf_script

#endif
