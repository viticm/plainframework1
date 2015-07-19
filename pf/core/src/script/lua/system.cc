#include "pf/base/log.h"
#include "pf/script/lua/vm.h"
#include "pf/script/lua/system.h"

pf_script::lua::System *g_script_luasystem = NULL;

template <> 
pf_script::lua::System 
  *pf_base::Singleton<pf_script::lua::System>::singleton_ = NULL;

namespace pf_script {

namespace lua {

System *System::getsingleton_pointer() {
  return singleton_;
}

System &System::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

System::System() {
  //do nothing
}

System::~System() {
  //do nothing
}

void System::registerfunctions() {
  __ENTER_FUNCTION
    if (function_registers_) function_registers_();
  __LEAVE_FUNCTION
}

void System::set_function_registers(function_registers function) {
  function_registers_ = function;
}

void System::tick(uint32_t time) {
  __ENTER_FUNCTION
#ifndef PF_CORE_WITH_NOLUA
    if (time > 0) get_vm()->checkgc(time);
#else
    USE_PARAM(time);
#endif
  __LEAVE_FUNCTION
}

} //namespace lua

} //namespace pf_script
