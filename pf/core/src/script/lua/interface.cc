#ifndef PF_CORE_WITH_NOLUA

#include "pf/base/string.h"
#include "pf/base/log.h"
#include "pf/script/cache/manager.h"
#include "pf/script/lua/interface.h"

namespace pf_script {

namespace lua {

#define LUA_DISABLE_VERIFYFUNCTION

bool Interface::lua_reloadscript_always_ = true;
bool Interface::lua_recordscript_step_ = false;
const char *Interface::kLuaScriptStepFileName = "SCRIPT_STEP_FILENAME";
const int32_t Interface::kLuaScriptCountMax = 4096;

Interface::Interface() {
  __ENTER_FUNCTION
    using namespace pf_base;
    memset(global_filename_, 0, sizeof(global_filename_));
    string::safecopy(global_filename_, 
                     SCRIPT_LUA_GLOBAL_VAR_FILE_DEFAULT, 
                     sizeof(global_filename_));
  __LEAVE_FUNCTION
}

Interface::~Interface() {
  __ENTER_FUNCTION
    release();
  __LEAVE_FUNCTION
}

void Interface::set_globalfile(const char *filename) {
  __ENTER_FUNCTION
    using namespace pf_base;
    string::safecopy(global_filename_, filename, sizeof(global_filename_));
  __LEAVE_FUNCTION
}

void Interface::set_rootpath(const char *path) {
  __ENTER_FUNCTION
    VM_.set_rootpath(path); 
  __LEAVE_FUNCTION
}

void Interface::set_workpath(const char *path) {
  __ENTER_FUNCTION
    VM_.set_workpath(path);
  __LEAVE_FUNCTION
}

const char *Interface::get_rootpath() {
  __ENTER_FUNCTION
    const char *path = VM_.get_rootpath();
    return path;
  __LEAVE_FUNCTION
    return NULL;
}
   
VM *Interface::get_vm() {
  VM *vm = &VM_;
  return vm;
}

bool Interface::init() {
  __ENTER_FUNCTION
    bool result = true;
    int32_t stacksize = 1024 * 4;
#if LUA_VERSION_NUM >= 502
    stacksize = 1024 * 100;
#endif
    result = VM_.init(stacksize);
    char msg[128] = {0};
    snprintf(msg, sizeof(msg) - 1, "VM_.init(%d) failed", stacksize);
    AssertEx(result, msg);
    result = loadscript(global_filename_);
    script_loaded_.init(kLuaScriptCountMax);
    return result;
  __LEAVE_FUNCTION
    return false;
}

void Interface::release() {
  __ENTER_FUNCTION
    VM_.release();
  __LEAVE_FUNCTION
}

bool Interface::find_function(lua_State *L, const char *name) {
  __ENTER_FUNCTION
    lua_getglobal(L, name);
    if (lua_isnil(L, -1)) {
      lua_pop(L, 1);
      return false;
    }
    lua_pop(L, 1);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Interface::find_function(const char *functionname) {
  __ENTER_FUNCTION
    bool result = find_function(VM_.lua_state_, functionname);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Interface::verify_function(const char **functionname) {
  __ENTER_FUNCTION
    bool result = verify_function(VM_.lua_state_, functionname);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Interface::verify_function(lua_State *L, const char **names) {
  __ENTER_FUNCTION
    USE_PARAM(L);
    if (is_null(*names) || 0 == strlen(*names)) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}
   
bool Interface::runfunction(const char *name,
                            pf_base::variable_array_t &params,
                            pf_base::variable_array_t &results) {
  __ENTER_FUNCTION
    enter_runstep(name);
    if (!verify_function(&name)) return 0;
    bool result = true;
    { //mutli thread
      pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
      int32_t topindex = 0;
      VM_.callfunction_enter(&topindex);
      result = VM_.callfunction(name, params, results);
      VM_.callfunction_leave(topindex);
    }
    if (!result) {
      FAST_ERRORLOG("error",
                    "[script.lua] (Interface::runfunction) lua stack leave"
                    " have some error, name: %s,"
                    " param count: %d, result count: %d",
                    name,
                    params.size(),
                    results.size());
    }
    leave_runstep(name);
    return true;
  __LEAVE_FUNCTION
    return false;
}

cache::Base *Interface::getscript_filedata(int32_t scriptid) {
  __ENTER_FUNCTION
    cache::Base *filedata = NULL;
    filedata = SCRIPT_CACHE_MANAGER_POINTER->get_filedata(scriptid);
    return filedata;
  __LEAVE_FUNCTION
    return NULL;
}

int64_t Interface::runstring(const char *string) {
  __ENTER_FUNCTION
    if (!verify_function(&string)) return 0;
    bool result = true;
    int64_t _result = 0;
    {
      pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
      int32_t topindex = 0;
      VM_.callfunction_enter(&topindex);
      result = VM_.callstring(string);
      _result = static_cast<int64_t>(lua_tonumber(VM_.lua_state_, -1));
      VM_.callfunction_leave(topindex);
    }
    if (!result) {
      FAST_ERRORLOG("error",
                    "[script.lua] (Interface::runstring) lua stack leave"
                    " have some error, string: %s"
                    " params: (void)",
                    string);
    }
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

void Interface::enter_runstep(int32_t scriptid, const char *functionname) {
  __ENTER_FUNCTION
    if (!lua_recordscript_step_) return;
    lua_Debug debug;
    int32_t topindex = lua_gettop(VM_.lua_state_);
    int32_t stackspace = lua_getstack(VM_.lua_state_, 2, &debug);
    FAST_LOG(SCRIPT_MODULENAME,
             "[script.lua] (Interface::enter_runstep)"
             " topindex: %d, stackspace: %d, scriptid: %d, functionname: %s",
             topindex,
             stackspace,
             scriptid,
             functionname);
  __LEAVE_FUNCTION
}

void Interface::leave_runstep(int32_t scriptid, const char *functionname) {
  __ENTER_FUNCTION
    if (!lua_recordscript_step_) return;
    lua_Debug debug;
    int32_t topindex = lua_gettop(VM_.lua_state_);
    int32_t stackspace = lua_getstack(VM_.lua_state_, 2, &debug);
    FAST_LOG(SCRIPT_MODULENAME,
             "[script.lua] (Interface::enter_runstep)"
             " topindex: %d, stackspace: %d, scriptid: %d, functionname: %s",
             topindex,
             stackspace,
             scriptid,
             functionname);
  __LEAVE_FUNCTION
}   

void Interface::enter_runstep(const char *functionname) {
  __ENTER_FUNCTION
    if (!lua_recordscript_step_) return;
    lua_Debug debug;
    int32_t topindex = lua_gettop(VM_.lua_state_);
    int32_t stackspace = lua_getstack(VM_.lua_state_, 2, &debug);
    FAST_LOG(SCRIPT_MODULENAME,
             "[script.lua] (Interface::enter_runstep)"
             " topindex: %d, stackspace: %d, functionname: %s",
             topindex,
             stackspace,
             functionname);
  __LEAVE_FUNCTION
}
   
void Interface::leave_runstep(const char *functionname) {
  __ENTER_FUNCTION
    if (!lua_recordscript_step_) return;
    lua_Debug debug;
    int32_t topindex = lua_gettop(VM_.lua_state_);
    int32_t stackspace = lua_getstack(VM_.lua_state_, 2, &debug);
    FAST_LOG(SCRIPT_MODULENAME,
             "[script.lua] (Interface::enter_runstep)"
             " topindex: %d, stackspace: %d, functionname: %s",
             topindex,
             stackspace,
             functionname);
  __LEAVE_FUNCTION
}
   
bool Interface::reload(const char *filename) {
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
    if (!loadscript(filename)) return false;
    VM_.unregister_refs();
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Interface::reloadscript(int32_t scriptid) {
  __ENTER_FUNCTION
    cache::Base *cachedata = 
      reinterpret_cast<cache::Base *>(getscript_byid(scriptid));
    if (NULL == cachedata) {
      cachedata = getscript_filedata(scriptid);
      if (NULL == cachedata) return false;
      script_loaded_.add(scriptid, cachedata);
    }
    if (cachedata) {
      const char *filename = cachedata->get_filename();
      AssertEx(filename, filename);
      if (filename) {
        bool result = loadscript(filename);
        if (!result && !lua_reloadscript_always_) {
          FAST_LOG("debug",
                   "[script.lua] (Interface::reloadscript)"
                   " filename: %s, scriptid: %d",
                   filename,
                   scriptid);
        }
        return result;
      }
    }
    return false;
  __LEAVE_FUNCTION
    return false;
}

void *Interface::getscript_byid(int32_t scriptid) {
  __ENTER_FUNCTION
    return script_loaded_.get(scriptid);
  __LEAVE_FUNCTION
    return NULL;
}

bool Interface::addscript(int32_t scriptid, void *data) {
  __ENTER_FUNCTION
    return script_loaded_.add(scriptid, data);
  __LEAVE_FUNCTION
    return false;
}

bool Interface::loadscript(const char *filename) {
  __ENTER_FUNCTION
    return VM_.load(filename);
  __LEAVE_FUNCTION
    return false;
}

bool Interface::check_paramnumber(lua_State *L, int32_t count) {
  __ENTER_FUNCTION
    if (!L) return false;
    if (lua_gettop(L) < count) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

int32_t Interface::getscript_stepid(lua_State *L) {
  USE_PARAM(L); //以后扩展后去掉此宏
  return -1;
}

bool Interface::is_paramnumber(lua_State *L, 
                               int32_t index, 
                               const char *functionname) {
  __ENTER_FUNCTION
    if (!L) {
      FAST_ERRORLOG("error",
                    "[script.lua] (Interface::is_paramnumber)"
                    " lua state is nil");
    }
    if (!lua_isnumber(L, index)) {
      FAST_ERRORLOG("error",
                    "[script.lua] (Interface::is_paramnumber)"
                    " function: %s, param %d is illegal!",
                    functionname,
                    index);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Interface::is_paramstring(lua_State *L, 
                               int32_t index, 
                               const char *functionname) {
  __ENTER_FUNCTION
    if (!L) {
      FAST_ERRORLOG("error",
                    "[script.lua] (Interface::is_paramnumber)"
                    " lua state is nil");
    }
    if (!lua_isstring(L, index)) {
      FAST_ERRORLOG("error",
                    "[script.lua] (Interface::is_paramnumber)"
                    " function: %s, param %d is illegal!",
                    functionname,
                    index);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Interface::mark_scriptname(lua_State *L, const char *name) {
#ifdef _DEBUG
  __ENTER_FUNCTION
    if (!L || !name) return;
    lua_pushstring(L, name);
    lua_setglobal(L, kLuaScriptStepFileName);
  __LEAVE_FUNCTION
#else
  USE_PARAM(L);
  USE_PARAM(name);
#endif
}

void Interface::check_scriptvalid(void *file, 
                                  int32_t scriptid, 
                                  const char *functionname) {
#ifdef _DEBUG
  __ENTER_FUNCTION
    char buffer[256] = {0};
    snprintf(buffer, 
             sizeof(buffer) - 1, 
             "%s, scriptid=%d", 
             functionname, 
             scriptid);
    USE_PARAM(file);
    //AssertEx(file, buffer);
  __LEAVE_FUNCTION
#else
  USE_PARAM(file);
  USE_PARAM(scriptid);
  USE_PARAM(functionname);
#endif
}

}; //namespace lua

}; //namespace pf_script

#endif
