#include "pf/base/log.h"
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

int32_t System::call_noclosure(lua_State *L) {
  __ENTER_FUNCTION
    int32_t argc = lua_gettop(L);
    AssertEx(argc >= 2, "params lest than 2");
    if (!is_paramnumber(L, 1, "call_noclosure")) {
      lua_pushnumber(L, -1);
      return 1;
    }
    if (!is_paramstring(L, 2, "call_noclosure")) {
      lua_pushnumber(L, -1);
      return 1;
    }
    if (!SCRIPT_LUASYSTEM_POINTER) {
      lua_pushnumber(L, -1);
      return 1;
    }
    int32_t scriptid = static_cast<int32_t>(lua_tonumber(L, 1));
    const char *functionname = lua_tostring(L, 2);
    try {
      SCRIPT_LUASYSTEM_POINTER->enter_runstep(scriptid, functionname);
      bool loaded = false;
      cache::Base *cachedata = reinterpret_cast<cache::Base *>(
          SCRIPT_LUASYSTEM_POINTER->getscript_byid(scriptid));
      if (NULL == cachedata) {
        cachedata = getscript_filedata(scriptid);
        if (cachedata) {
          SCRIPT_LUASYSTEM_POINTER->addscript(scriptid, cachedata);
          loaded = true;
        } else {
          lua_pushnumber(L, -1);
          SCRIPT_LUASYSTEM_POINTER->leave_runstep(scriptid, functionname);
          return 1;
        }
      }
      const char *filename = cachedata->get_filename();
      if (lua_reloadscript_always_) loaded = true;
      if (loaded && filename) {
        bool result = SCRIPT_LUASYSTEM_POINTER->loadscript(filename);
        if (!result) {
          FAST_ERRORLOG(kErrorLogFile, 
                        "[script.lua] (Interface::call_noclosure)"
                        " load script: %s failed", 
                        filename);
          lua_pushnumber(L, -1);
          SCRIPT_LUASYSTEM_POINTER->leave_runstep(scriptid, functionname);
          return 1;
        }
        if (false == lua_reloadscript_always_) {
          FAST_LOG(kDebugLogFile,
                   "[script.lua] (Interface::call_noclosure)"
                   " script(%d), function(%s)",
                   scriptid,
                   functionname);
        }
      }
      if (cachedata && filename) {
        char functionname_x[128] = {0};
        snprintf(functionname_x,
                 sizeof(functionname_x) - 1,
                 "x%.6d_%s",
                 scriptid,
                 functionname);
        char *functionname_x_pointer = functionname_x;
        mark_scriptname(L, filename);
        if (!SCRIPT_LUASYSTEM_POINTER->verify_function(
              L, (const char **)&functionname_x_pointer)) {
          FAST_ERRORLOG(kErrorLogFile,
                        "[script.lua] (Interface::call_noclosure)"
                        " SCRIPT_LUASYSTEM_POINTER->verify_function(%s) error",
                        functionname_x);
          lua_pushnumber(L, -1);
          SCRIPT_LUASYSTEM_POINTER->leave_runstep(scriptid, functionname);
          return 1;
        }
        try {
          lua_getglobal(L, functionname_x_pointer);
          int32_t paramindex_begin = 3;
          for (int32_t index = paramindex_begin; index < argc; ++index) {
            switch (lua_type(L, index)) {
              case LUA_TUSERDATA:
                Assert(false);
                break;
              case LUA_TNIL:
                lua_pushnil(L);
                break;
              case LUA_TNUMBER: {
                int32_t number = static_cast<int32_t>(lua_tonumber(L, index));
                lua_pushnumber(L, number);
                break;
              }
              case LUA_TSTRING: {
                const char *str = lua_tostring(L, index);
                lua_pushstring(L, str);
                break;
              }
              case LUA_TTABLE:
                Assert(false);
                break;
              case LUA_TFUNCTION: 
                lua_pushcfunction(L, lua_tocfunction(L, index));
                break;
              default:
                lua_pushnil(L);
                break;
            } //switch
          } //for
          int32_t _result = -1;
          int32_t call_result = 
            lua_pcall(L, argc - paramindex_begin, _result, 0);
          int32_t argnow = lua_gettop(L);
          _result = argnow - argc;
          for (int32_t index = 1; index <= _result; ++index) {
            switch (lua_type(L, index)) {
              case LUA_TUSERDATA:
                Assert(false);
                break;
              case LUA_TNIL:
                lua_pushnil(L);
                break;
              case LUA_TNUMBER: {
                int32_t number = static_cast<int32_t>(lua_tonumber(L, index));
                lua_pushnumber(L, number);
                break;
              }
              case LUA_TSTRING: {
                const char *str = lua_tostring(L, index);
                lua_pushstring(L, str);
                break;
              }
              case LUA_TTABLE:
                Assert(false);
                break;
              case LUA_TFUNCTION: 
                lua_pushcfunction(L, lua_tocfunction(L, index));
                break;
              default:
                lua_pushnil(L);
                break;
            } //switch
          } //for
          if (call_result != 0) {
            FAST_ERRORLOG(kErrorLogFile,
                          "[script.lua] (Interface::call_noclosure) error"
                          " call_result: %d, file: %s, functionname: %s",
                          call_result,
                          filename,
                          functionname_x_pointer);
            SCRIPT_LUASYSTEM_POINTER->leave_runstep(scriptid, functionname);
            return _result;
          }
        } catch(...) {
          FAST_ERRORLOG(kErrorLogFile,
                        "[script.lua] (Interface::call_noclosure) error"
                        "lua_call get a exception, file: %s, functionname: %s",
                        filename,
                        functionname);
        }
      }
    } catch (...) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::call_noclosure) error"
                    "lua_call get a exception, scriptid: %d, functionname: %s",
                    scriptid,
                    functionname);

    }
    lua_pushnumber(L, -1);
    return 1;
  __LEAVE_FUNCTION
    lua_pushnumber(L, -1);
    return 1;
}

} //namespace lua

} //namespace pf_script
