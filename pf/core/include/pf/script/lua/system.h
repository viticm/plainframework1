/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id system.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/29 10:51
 * @uses script lua module system
 */
#ifndef PF_SCRIPT_LUA_SYSTEM_H_
#define PF_SCRIPT_LUA_SYSTEM_H_

#include "pf/script/lua/config.h"
#include "pf/base/singleton.h"
#include "pf/script/lua/interface.h"

namespace pf_script {

namespace lua {

typedef bool (__stdcall *function_registers)();

class PF_API System : public pf_base::Singleton<System>, public Interface {

 public:
   System();
   ~System();

 public:
   static System *getsingleton_pointer();
   static System &getsingleton();
   function_registers function_registers_;
   void registerfunctions();
   void set_function_registers(function_registers function);

 public:
   static int32_t call_noclosure(lua_State *L);

};

}; //namespace lua

}; //namespace pf_script

#define SCRIPT_LUASYSTEM_POINTER \
pf_script::lua::System::getsingleton_pointer()

PF_API extern pf_script::lua::System *g_script_luasystem;

#endif //PF_SCRIPT_LUA_SYSTEM_H_
