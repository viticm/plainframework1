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
    bool result = VM_.init(4 * 1024);
    Assert(result && "VM_.init(4 * 1024) failed");
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

bool Interface::find_function(lua_State *L, const char *functionname) {
  __ENTER_FUNCTION
    lua_getglobal(L, functionname);
    if (lua_isnil(L, -1)) {
      lua_pop(L, 1);
      return false;
    }
    lua_pop(L, 1);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Interface::verify_function(lua_State *L, const char **functionname) {
  __ENTER_FUNCTION
    if (!find_function(L, *functionname)) {
#ifdef LUA_DISABLE_VERIFYFUNCTION /* { */
      const char *src = strchr(*functionname, '_') + 1;
      memmove(const_cast<char *>(*functionname), src, strlen(src) + 1);
      if (!find_function(L, *functionname)) {
#ifdef _DEBUG
        char buffer[256] = {0};
        lua_getglobal(L, kLuaScriptStepFileName);
        const char *filename = lua_tostring(L, 1);
        snprintf(buffer, 
                 sizeof(buffer) - 1, 
                 "can't find function(%s) in file(%s)",
                 *functionname,
                 filename);
        lua_pop(L, 1);
        AssertEx(false, buffer);
#endif
        return false;
      }
#else /* }{ */

#ifdef _DEBUG
      char buffer[256] = {0};
      char *filename = lua_getglobal(L, kLuaScriptStepFileName);
      snprintf(buffer, 
               sizeof(buffer) - 1, 
               "can't find function(%s) in file(%s)",
               *functionname,
               filename);
      lua_pop(L, 1);
      AssertEx(false, buffer);
#endif

#endif /* } */
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

int64_t Interface::run_filefunction(const char *filename, 
                                 const char *functionname, 
                                 bool load) {
  __ENTER_FUNCTION
    USE_PARAM(load); //以后扩展后去掉此宏
    mark_scriptname(VM_.lua_state_, filename);
    if (!verify_function(VM_.lua_state_, &functionname)) return 0;
    int32_t topindex = 0;
    VM_.callfunction_enter(&topindex);
    bool result = VM_.callfunction(functionname, 1);
    int64_t _result = static_cast<int64_t>(lua_tonumber(VM_.lua_state_, -1));
    VM_.callfunction_leave(topindex);
    if (!result) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::run_filefunction) lua stack leave"
                    " have some error, file: %s, functionname: %s",
                    filename,
                    functionname);
    }
    return _result;
  __LEAVE_FUNCTION
    return 0;
}


cache::Base *Interface::getscript_filedata(int32_t scriptid) {
  __ENTER_FUNCTION
    cache::Base *filedata = NULL;
    filedata = SCRIPT_CACHE_MANAGER_POINTER->get_filedata(scriptid);
    return filedata;
  __LEAVE_FUNCTION
    return NULL;
}

int64_t Interface::run_filefunction(const char *filename, 
                                    const char *functionname, 
                                    int64_t param0,
                                    bool load) {
  __ENTER_FUNCTION
    USE_PARAM(load); //以后扩展后去掉此宏
    mark_scriptname(VM_.lua_state_, filename);
    if (!verify_function(VM_.lua_state_, &functionname)) return 0;
    int32_t topindex = 0;
    VM_.callfunction_enter(&topindex);
    bool result = VM_.callfunction(functionname, 1, param0);
    int64_t _result = static_cast<int64_t>(lua_tonumber(VM_.lua_state_, -1));
    VM_.callfunction_leave(topindex);
    if (!result) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::run_filefunction) lua stack leave"
                    " have some error, file: %s, functionname: %s"
                    " params: (%d)",
                    filename,
                    functionname,
                    param0);
    }
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_filefunction(const char *filename, 
                                    const char *functionname, 
                                    int64_t param0,
                                    int64_t param1,
                                    bool load) {
  __ENTER_FUNCTION
    USE_PARAM(load); //以后扩展后去掉此宏
    mark_scriptname(VM_.lua_state_, filename);
    if (!verify_function(VM_.lua_state_, &functionname)) return 0;
    int32_t topindex = 0;
    VM_.callfunction_enter(&topindex);
    bool result = VM_.callfunction(functionname, 1, param0, param1);
    int64_t _result = static_cast<int64_t>(lua_tonumber(VM_.lua_state_, -1));
    VM_.callfunction_leave(topindex);
    if (!result) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::run_filefunction) lua stack leave"
                    " have some error, file: %s, functionname: %s"
                    " params: (%d, %d)",
                    filename,
                    functionname,
                    param0,
                    param1);
    }
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_filefunction(const char *filename, 
                                    const char *functionname, 
                                    int64_t param0,
                                    int64_t param1,
                                    int64_t param2,
                                    bool load) {
  __ENTER_FUNCTION
    USE_PARAM(load); //以后扩展后去掉此宏
    mark_scriptname(VM_.lua_state_, filename);
    if (!verify_function(VM_.lua_state_, &functionname)) return 0;
    int32_t topindex = 0;
    VM_.callfunction_enter(&topindex);
    bool result = VM_.callfunction(functionname, 1, param0, param1, param2);
    int64_t _result = static_cast<int64_t>(lua_tonumber(VM_.lua_state_, -1));
    VM_.callfunction_leave(topindex);
    if (!result) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::run_filefunction) lua stack leave"
                    " have some error, file: %s, functionname: %s"
                    " params: (%d, %d, %d)",
                    filename,
                    functionname,
                    param0,
                    param1,
                    param2);
    }
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_filefunction(const char *filename, 
                                    const char *functionname, 
                                    int64_t param0,
                                    int64_t param1,
                                    int64_t param2,
                                    int64_t param3,
                                    bool load) {
  __ENTER_FUNCTION
    USE_PARAM(load); //以后扩展后去掉此宏
    mark_scriptname(VM_.lua_state_, filename);
    if (!verify_function(VM_.lua_state_, &functionname)) return 0;
    int32_t topindex = 0;
    VM_.callfunction_enter(&topindex);
    bool result = VM_.callfunction(functionname, 
                                   1, 
                                   param0, 
                                   param1,
                                   param2,
                                   param3);
    int64_t _result = static_cast<int64_t>(lua_tonumber(VM_.lua_state_, -1));
    VM_.callfunction_leave(topindex);
    if (!result) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::run_filefunction) lua stack leave"
                    " have some error, file: %s, functionname: %s"
                    " params: (%d, %d, %d, %d)",
                    filename,
                    functionname,
                    param0,
                    param1,
                    param2,
                    param3);
    }
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_filefunction(const char *filename, 
                                    const char *functionname, 
                                    int64_t param0,
                                    int64_t param1,
                                    int64_t param2,
                                    int64_t param3,
                                    int64_t param4,
                                    bool load) {
  __ENTER_FUNCTION
    USE_PARAM(load); //以后扩展后去掉此宏
    mark_scriptname(VM_.lua_state_, filename);
    if (!verify_function(VM_.lua_state_, &functionname)) return 0;
    int32_t topindex = 0;
    VM_.callfunction_enter(&topindex);
    bool result = VM_.callfunction(functionname, 
                                   1, 
                                   param0, 
                                   param1,
                                   param2,
                                   param3,
                                   param4);
    int64_t _result = static_cast<int64_t>(lua_tonumber(VM_.lua_state_, -1));
    VM_.callfunction_leave(topindex);
    if (!result) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::run_filefunction) lua stack leave"
                    " have some error, file: %s, functionname: %s"
                    " params: (%d, %d, %d, %d, %d)",
                    filename,
                    functionname,
                    param0,
                    param1,
                    param2,
                    param3,
                    param4);
    }
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_filefunction(const char *filename, 
                                    const char *functionname, 
                                    int64_t param0,
                                    int64_t param1,
                                    int64_t param2,
                                    int64_t param3,
                                    int64_t param4,
                                    int64_t param5,
                                    bool load) {
  __ENTER_FUNCTION
    USE_PARAM(load); //以后扩展后去掉此宏
    mark_scriptname(VM_.lua_state_, filename);
    if (!verify_function(VM_.lua_state_, &functionname)) return 0;
    int32_t topindex = 0;
    VM_.callfunction_enter(&topindex);
    bool result = VM_.callfunction(functionname, 
                                   1, 
                                   param0, 
                                   param1,
                                   param2,
                                   param3,
                                   param4,
                                   param5);
    int64_t _result = static_cast<int64_t>(lua_tonumber(VM_.lua_state_, -1));
    VM_.callfunction_leave(topindex);
    if (!result) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::run_filefunction) lua stack leave"
                    " have some error, file: %s, functionname: %s"
                    " params: (%d, %d, %d, %d, %d)",
                    filename,
                    functionname,
                    param0,
                    param1,
                    param2,
                    param3,
                    param4,
                    param5);
    }
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_filefunction(const char *filename, 
                                    const char *functionname, 
                                    int64_t param0,
                                    int64_t param1,
                                    int64_t param2,
                                    int64_t param3,
                                    int64_t param4,
                                    int64_t param5,
                                    int64_t param6,
                                    bool load) {
  __ENTER_FUNCTION
    USE_PARAM(load); //以后扩展后去掉此宏
    mark_scriptname(VM_.lua_state_, filename);
    if (!verify_function(VM_.lua_state_, &functionname)) return 0;
    int32_t topindex = 0;
    VM_.callfunction_enter(&topindex);
    bool result = VM_.callfunction(functionname, 
                                   1, 
                                   param0, 
                                   param1,
                                   param2,
                                   param3,
                                   param4,
                                   param5,
                                   param6);
    int64_t _result = static_cast<int64_t>(lua_tonumber(VM_.lua_state_, -1));
    VM_.callfunction_leave(topindex);
    if (!result) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::run_filefunction) lua stack leave"
                    " have some error, file: %s, functionname: %s"
                    " params: (%d, %d, %d, %d, %d, %d, %d)",
                    filename,
                    functionname,
                    param0,
                    param1,
                    param2,
                    param3,
                    param4,
                    param5,
                    param6);
    }
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_filefunction(const char *filename, 
                                    const char *functionname, 
                                    int64_t param0,
                                    int64_t param1,
                                    int64_t param2,
                                    int64_t param3,
                                    int64_t param4,
                                    int64_t param5,
                                    int64_t param6,
                                    int64_t param7,
                                    bool load) {
  __ENTER_FUNCTION
    USE_PARAM(load); //以后扩展后去掉此宏
    mark_scriptname(VM_.lua_state_, filename);
    if (!verify_function(VM_.lua_state_, &functionname)) return 0;
    int32_t topindex = 0;
    VM_.callfunction_enter(&topindex);
    bool result = VM_.callfunction(functionname, 
                                   1, 
                                   param0, 
                                   param1,
                                   param2,
                                   param3,
                                   param4,
                                   param5,
                                   param6,
                                   param7);
    int64_t _result = static_cast<int64_t>(lua_tonumber(VM_.lua_state_, -1));
    VM_.callfunction_leave(topindex);
    if (!result) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::run_filefunction) lua stack leave"
                    " have some error, file: %s, functionname: %s"
                    " params: (%d, %d, %d, %d, %d, %d, %d, %d, %d)",
                    filename,
                    functionname,
                    param0,
                    param1,
                    param2,
                    param3,
                    param4,
                    param5,
                    param6,
                    param7);
    }
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_filefunction(const char *filename, 
                                    const char *functionname, 
                                    int64_t param0,
                                    int64_t param1,
                                    float param2,
                                    float param3,
                                    bool load) {
  __ENTER_FUNCTION
    USE_PARAM(load); //以后扩展后去掉此宏
    mark_scriptname(VM_.lua_state_, filename);
    if (!verify_function(VM_.lua_state_, &functionname)) return 0;
    int32_t topindex = 0;
    VM_.callfunction_enter(&topindex);
    bool result = VM_.callfunction(functionname, 
                                   1, 
                                   param0, 
                                   param1,
                                   param2,
                                   param3);
    int64_t _result = static_cast<int64_t>(lua_tonumber(VM_.lua_state_, -1));
    VM_.callfunction_leave(topindex);
    if (!result) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::run_filefunction) lua stack leave"
                    " have some error, file: %s, functionname: %s"
                    " params: (%d, %d, %f, %f)",
                    filename,
                    functionname,
                    param0,
                    param1,
                    param2,
                    param3);
    }
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_filefunction(const char *filename, 
                                    const char *functionname, 
                                    int64_t param0,
                                    int64_t param1,
                                    const char *param2,
                                    const char *param3,
                                    bool load) {
  __ENTER_FUNCTION
    USE_PARAM(load); //以后扩展后去掉此宏
    mark_scriptname(VM_.lua_state_, filename);
    if (!verify_function(VM_.lua_state_, &functionname)) return 0;
    int32_t topindex = 0;
    VM_.callfunction_enter(&topindex);
    bool result = VM_.callfunction(functionname, 
                                   1, 
                                   param0, 
                                   param1,
                                   param2,
                                   param3);
    int64_t _result = static_cast<int64_t>(lua_tonumber(VM_.lua_state_, -1));
    VM_.callfunction_leave(topindex);
    if (!result) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::run_filefunction) lua stack leave"
                    " have some error, file: %s, functionname: %s"
                    " params: (%d, %d, %s, %s)",
                    filename,
                    functionname,
                    param0,
                    param1,
                    param2,
                    param3);
    }
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_scriptfunction(int32_t scriptid, 
                                      const char *functionname) {
  __ENTER_FUNCTION
    //进入步骤
    enter_runstep(scriptid, functionname);
    //取得脚本载入状态
    bool loaded = false;
    cache::Base *cachedata = 
      reinterpret_cast<cache::Base *>(getscript_byid(scriptid));
    if (NULL == cachedata) {
      cachedata = getscript_filedata(scriptid);
      script_loaded_.add(scriptid, cachedata);
      loaded = true;
    }
    //检查脚本载入结果
    check_scriptvalid(cachedata, scriptid, functionname);
    //载入失败
    if (NULL == cachedata) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    //载入成功
    const char *filename = cachedata->get_filename();
    if (NULL == filename) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    char functionname_x[128] = {0};
    snprintf(functionname_x, 
             sizeof(functionname_x) - 1, 
             "x%.6d_%s", 
             scriptid, 
             functionname);
    if (lua_reloadscript_always_) loaded = true;
    bool result = true;
    if (loaded) {
      result = loadscript(filename);
      if (!result) {
        leave_runstep(scriptid, functionname);
        return 0;
      }
      if (!lua_reloadscript_always_) {
        FAST_LOG(kDebugLogFile,
                 "[script.lua] (Interface::run_scriptfunction)"
                 " scriptid: %d, functionname: %s"
                 " params: ()",
                 scriptid,
                 functionname);
      }
    }
    int64_t _result = run_filefunction(filename, functionname_x, loaded);
    leave_runstep(scriptid, functionname);
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_scriptfunction(int32_t scriptid, 
                                      const char *functionname,
                                      int64_t param0) {
  __ENTER_FUNCTION
    //进入步骤
    enter_runstep(scriptid, functionname);
    //取得脚本载入状态
    bool loaded = false;
    cache::Base *cachedata = 
      reinterpret_cast<cache::Base *>(getscript_byid(scriptid));
    if (NULL == cachedata) {
      cachedata = getscript_filedata(scriptid);
      script_loaded_.add(scriptid, cachedata);
      loaded = true;
    }
    //检查脚本载入结果
    check_scriptvalid(cachedata, scriptid, functionname);
    //载入失败
    if (NULL == cachedata) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    //载入成功
    const char *filename = cachedata->get_filename();
    if (NULL == filename) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    char functionname_x[128] = {0};
    snprintf(functionname_x, 
             sizeof(functionname_x) - 1, 
             "x%.6d_%s", 
             scriptid, 
             functionname);
    if (lua_reloadscript_always_) loaded = true;
    bool result = true;
    if (loaded) {
      result = loadscript(filename);
      if (!result) {
        leave_runstep(scriptid, functionname);
        return 0;
      }
      if (!lua_reloadscript_always_) {
        FAST_LOG(kDebugLogFile,
                 "[script.lua] (Interface::run_scriptfunction)"
                 " scriptid: %d, functionname: %s"
                 " params: (%d)",
                 scriptid,
                 functionname,
                 param0);
      }
    }
    int64_t _result = run_filefunction(filename, 
                                       functionname_x, 
                                       param0, 
                                       loaded);
    leave_runstep(scriptid, functionname);
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_scriptfunction(int32_t scriptid, 
                                      const char *functionname,
                                      int64_t param0,
                                      int64_t param1) {
  __ENTER_FUNCTION
    //进入步骤
    enter_runstep(scriptid, functionname);
    //取得脚本载入状态
    bool loaded = false;
    cache::Base *cachedata = 
      reinterpret_cast<cache::Base *>(getscript_byid(scriptid));
    if (NULL == cachedata) {
      cachedata = getscript_filedata(scriptid);
      script_loaded_.add(scriptid, cachedata);
      loaded = true;
    }
    //检查脚本载入结果
    check_scriptvalid(cachedata, scriptid, functionname);
    //载入失败
    if (NULL == cachedata) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    //载入成功
    const char *filename = cachedata->get_filename();
    if (NULL == filename) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    char functionname_x[128] = {0};
    snprintf(functionname_x, 
             sizeof(functionname_x) - 1, 
             "x%.6d_%s", 
             scriptid, 
             functionname);
    if (lua_reloadscript_always_) loaded = true;
    bool result = true;
    if (loaded) {
      result = loadscript(filename);
      if (!result) {
        leave_runstep(scriptid, functionname);
        return 0;
      }
      if (!lua_reloadscript_always_) {
        FAST_LOG(kDebugLogFile,
                 "[script.lua] (Interface::run_scriptfunction)"
                 " scriptid: %d, functionname: %s"
                 " params: (%d, %d)",
                 scriptid,
                 functionname,
                 param0,
                 param1);
      }
    }
    int64_t _result = run_filefunction(filename, 
                                       functionname_x, 
                                       param0, 
                                       param1, 
                                       loaded);
    leave_runstep(scriptid, functionname);
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_scriptfunction(int32_t scriptid, 
                                      const char *functionname,
                                      int64_t param0,
                                      int64_t param1,
                                      int64_t param2) {
  __ENTER_FUNCTION
    //进入步骤
    enter_runstep(scriptid, functionname);
    //取得脚本载入状态
    bool loaded = false;
    cache::Base *cachedata = 
      reinterpret_cast<cache::Base *>(getscript_byid(scriptid));
    if (NULL == cachedata) {
      cachedata = getscript_filedata(scriptid);
      script_loaded_.add(scriptid, cachedata);
      loaded = true;
    }
    //检查脚本载入结果
    check_scriptvalid(cachedata, scriptid, functionname);
    //载入失败
    if (NULL == cachedata) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    //载入成功
    const char *filename = cachedata->get_filename();
    if (NULL == filename) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    char functionname_x[128] = {0};
    snprintf(functionname_x, 
             sizeof(functionname_x) - 1, 
             "x%.6d_%s", 
             scriptid, 
             functionname);
    if (lua_reloadscript_always_) loaded = true;
    bool result = true;
    if (loaded) {
      result = loadscript(filename);
      if (!result) {
        leave_runstep(scriptid, functionname);
        return 0;
      }
      if (!lua_reloadscript_always_) {
        FAST_LOG(kDebugLogFile,
                 "[script.lua] (Interface::run_scriptfunction)"
                 " scriptid: %d, functionname: %s"
                 " params: (%d, %d, %d)",
                 scriptid,
                 functionname,
                 param0,
                 param1,
                 param2);
      }
    }
    int64_t _result = run_filefunction(filename, 
                                       functionname_x, 
                                       param0, 
                                       param1, 
                                       param2, 
                                       loaded);
    leave_runstep(scriptid, functionname);
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_scriptfunction(int32_t scriptid, 
                                      const char *functionname,
                                      int64_t param0,
                                      int64_t param1,
                                      int64_t param2,
                                      int64_t param3) {
  __ENTER_FUNCTION
    //进入步骤
    enter_runstep(scriptid, functionname);
    //取得脚本载入状态
    bool loaded = false;
    cache::Base *cachedata = 
      reinterpret_cast<cache::Base *>(getscript_byid(scriptid));
    if (NULL == cachedata) {
      cachedata = getscript_filedata(scriptid);
      script_loaded_.add(scriptid, cachedata);
      loaded = true;
    }
    //检查脚本载入结果
    check_scriptvalid(cachedata, scriptid, functionname);
    //载入失败
    if (NULL == cachedata) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    //载入成功
    const char *filename = cachedata->get_filename();
    if (NULL == filename) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    char functionname_x[128] = {0};
    snprintf(functionname_x, 
             sizeof(functionname_x) - 1, 
             "x%.6d_%s", 
             scriptid, 
             functionname);
    if (lua_reloadscript_always_) loaded = true;
    bool result = true;
    if (loaded) {
      result = loadscript(filename);
      if (!result) {
        leave_runstep(scriptid, functionname);
        return 0;
      }
      if (!lua_reloadscript_always_) {
        FAST_LOG(kDebugLogFile,
                 "[script.lua] (Interface::run_scriptfunction)"
                 " scriptid: %d, functionname: %s"
                 " params: (%d, %d, %d, %d)",
                 scriptid,
                 functionname,
                 param0,
                 param1,
                 param2,
                 param3);
      }
    }
    int64_t _result = run_filefunction(filename, 
                                       functionname_x, 
                                       param0, 
                                       param1, 
                                       param2, 
                                       param3, 
                                       loaded);
    leave_runstep(scriptid, functionname);
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_scriptfunction(int32_t scriptid, 
                                      const char *functionname,
                                      int64_t param0,
                                      int64_t param1,
                                      int64_t param2,
                                      int64_t param3,
                                      int64_t param4) {
  __ENTER_FUNCTION
    //进入步骤
    enter_runstep(scriptid, functionname);
    //取得脚本载入状态
    bool loaded = false;
    cache::Base *cachedata = 
      reinterpret_cast<cache::Base *>(getscript_byid(scriptid));
    if (NULL == cachedata) {
      cachedata = getscript_filedata(scriptid);
      script_loaded_.add(scriptid, cachedata);
      loaded = true;
    }
    //检查脚本载入结果
    check_scriptvalid(cachedata, scriptid, functionname);
    //载入失败
    if (NULL == cachedata) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    //载入成功
    const char *filename = cachedata->get_filename();
    if (NULL == filename) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    char functionname_x[128] = {0};
    snprintf(functionname_x, 
             sizeof(functionname_x) - 1, 
             "x%.6d_%s", 
             scriptid, 
             functionname);
    if (lua_reloadscript_always_) loaded = true;
    bool result = true;
    if (loaded) {
      result = loadscript(filename);
      if (!result) {
        leave_runstep(scriptid, functionname);
        return 0;
      }
      if (!lua_reloadscript_always_) {
        FAST_LOG(kDebugLogFile,
                 "[script.lua] (Interface::run_scriptfunction)"
                 " scriptid: %d, functionname: %s"
                 " params: (%d, %d, %d, %d, %d)",
                 scriptid,
                 functionname,
                 param0,
                 param1,
                 param2,
                 param3,
                 param4);
      }
    }
    int64_t _result = run_filefunction(filename, 
                                       functionname_x, 
                                       param0, 
                                       param1, 
                                       param2, 
                                       param3, 
                                       param4, 
                                       loaded);
    leave_runstep(scriptid, functionname);
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_scriptfunction(int32_t scriptid, 
                                      const char *functionname,
                                      int64_t param0,
                                      int64_t param1,
                                      int64_t param2,
                                      int64_t param3,
                                      int64_t param4,
                                      int64_t param5) {
  __ENTER_FUNCTION
    //进入步骤
    enter_runstep(scriptid, functionname);
    //取得脚本载入状态
    bool loaded = false;
    cache::Base *cachedata = 
      reinterpret_cast<cache::Base *>(getscript_byid(scriptid));
    if (NULL == cachedata) {
      cachedata = getscript_filedata(scriptid);
      script_loaded_.add(scriptid, cachedata);
      loaded = true;
    }
    //检查脚本载入结果
    check_scriptvalid(cachedata, scriptid, functionname);
    //载入失败
    if (NULL == cachedata) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    //载入成功
    const char *filename = cachedata->get_filename();
    if (NULL == filename) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    char functionname_x[128] = {0};
    snprintf(functionname_x, 
             sizeof(functionname_x) - 1, 
             "x%.6d_%s", 
             scriptid, 
             functionname);
    if (lua_reloadscript_always_) loaded = true;
    bool result = true;
    if (loaded) {
      result = loadscript(filename);
      if (!result) {
        leave_runstep(scriptid, functionname);
        return 0;
      }
      if (!lua_reloadscript_always_) {
        FAST_LOG(kDebugLogFile,
                 "[script.lua] (Interface::run_scriptfunction)"
                 " scriptid: %d, functionname: %s"
                 " params: (%d, %d, %d, %d, %d, %d)",
                 scriptid,
                 functionname,
                 param0,
                 param1,
                 param2,
                 param3,
                 param4,
                 param5);
      }
    }
    int64_t _result = run_filefunction(filename, 
                                       functionname_x, 
                                       param0, 
                                       param1, 
                                       param2, 
                                       param3, 
                                       param4, 
                                       param5, 
                                       loaded);
    leave_runstep(scriptid, functionname);
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_scriptfunction(int32_t scriptid, 
                                      const char *functionname,
                                      int64_t param0,
                                      int64_t param1,
                                      int64_t param2,
                                      int64_t param3,
                                      int64_t param4,
                                      int64_t param5,
                                      int64_t param6) {
  __ENTER_FUNCTION
    //进入步骤
    enter_runstep(scriptid, functionname);
    //取得脚本载入状态
    bool loaded = false;
    cache::Base *cachedata = 
      reinterpret_cast<cache::Base *>(getscript_byid(scriptid));
    if (NULL == cachedata) {
      cachedata = getscript_filedata(scriptid);
      script_loaded_.add(scriptid, cachedata);
      loaded = true;
    }
    //检查脚本载入结果
    check_scriptvalid(cachedata, scriptid, functionname);
    //载入失败
    if (NULL == cachedata) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    //载入成功
    const char *filename = cachedata->get_filename();
    if (NULL == filename) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    char functionname_x[128] = {0};
    snprintf(functionname_x, 
             sizeof(functionname_x) - 1, 
             "x%.6d_%s", 
             scriptid, 
             functionname);
    if (lua_reloadscript_always_) loaded = true;
    bool result = true;
    if (loaded) {
      result = loadscript(filename);
      if (!result) {
        leave_runstep(scriptid, functionname);
        return 0;
      }
      if (!lua_reloadscript_always_) {
        FAST_LOG(kDebugLogFile,
                 "[script.lua] (Interface::run_scriptfunction)"
                 " scriptid: %d, functionname: %s"
                 " params: (%d, %d, %d, %d, %d, %d, %d)",
                 scriptid,
                 functionname,
                 param0,
                 param1,
                 param2,
                 param3,
                 param4,
                 param5,
                 param6);
      }
    }
    int64_t _result = run_filefunction(filename, 
                                       functionname_x, 
                                       param0, 
                                       param1, 
                                       param2, 
                                       param3, 
                                       param4, 
                                       param5, 
                                       param6, 
                                       loaded);
    leave_runstep(scriptid, functionname);
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_scriptfunction(int32_t scriptid, 
                                      const char *functionname,
                                      int64_t param0,
                                      int64_t param1,
                                      int64_t param2,
                                      int64_t param3,
                                      int64_t param4,
                                      int64_t param5,
                                      int64_t param6,
                                      int64_t param7) {
  __ENTER_FUNCTION
    //进入步骤
    enter_runstep(scriptid, functionname);
    //取得脚本载入状态
    bool loaded = false;
    cache::Base *cachedata = 
      reinterpret_cast<cache::Base *>(getscript_byid(scriptid));
    if (NULL == cachedata) {
      cachedata = getscript_filedata(scriptid);
      script_loaded_.add(scriptid, cachedata);
      loaded = true;
    }
    //检查脚本载入结果
    check_scriptvalid(cachedata, scriptid, functionname);
    //载入失败
    if (NULL == cachedata) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    //载入成功
    const char *filename = cachedata->get_filename();
    if (NULL == filename) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    char functionname_x[128] = {0};
    snprintf(functionname_x, 
             sizeof(functionname_x) - 1, 
             "x%.6d_%s", 
             scriptid, 
             functionname);
    if (lua_reloadscript_always_) loaded = true;
    bool result = true;
    if (loaded) {
      result = loadscript(filename);
      if (!result) {
        leave_runstep(scriptid, functionname);
        return 0;
      }
      if (!lua_reloadscript_always_) {
        FAST_LOG(kDebugLogFile,
                 "[script.lua] (Interface::run_scriptfunction)"
                 " scriptid: %d, functionname: %s"
                 " params: (%d, %d, %d, %d, %d, %d, %d, %d)",
                 scriptid,
                 functionname,
                 param0,
                 param1,
                 param2,
                 param3,
                 param4,
                 param5,
                 param6,
                 param7);
      }
    }
    int64_t _result = run_filefunction(filename, 
                                       functionname_x, 
                                       param0, 
                                       param1, 
                                       param2, 
                                       param3, 
                                       param4, 
                                       param5, 
                                       param6, 
                                       param7, 
                                       loaded);
    leave_runstep(scriptid, functionname);
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_scriptfunction(int32_t scriptid, 
                                      const char *functionname,
                                      int64_t param0,
                                      int64_t param1,
                                      float param2,
                                      float param3) {
  __ENTER_FUNCTION
    //进入步骤
    enter_runstep(scriptid, functionname);
    //取得脚本载入状态
    bool loaded = false;
    cache::Base *cachedata = 
      reinterpret_cast<cache::Base *>(getscript_byid(scriptid));
    if (NULL == cachedata) {
      cachedata = getscript_filedata(scriptid);
      script_loaded_.add(scriptid, cachedata);
      loaded = true;
    }
    //检查脚本载入结果
    check_scriptvalid(cachedata, scriptid, functionname);
    //载入失败
    if (NULL == cachedata) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    //载入成功
    const char *filename = cachedata->get_filename();
    if (NULL == filename) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    char functionname_x[128] = {0};
    snprintf(functionname_x, 
             sizeof(functionname_x) - 1, 
             "x%.6d_%s", 
             scriptid, 
             functionname);
    if (lua_reloadscript_always_) loaded = true;
    bool result = true;
    if (loaded) {
      result = loadscript(filename);
      if (!result) {
        leave_runstep(scriptid, functionname);
        return 0;
      }
      if (!lua_reloadscript_always_) {
        FAST_LOG(kDebugLogFile,
                 "[script.lua] (Interface::run_scriptfunction)"
                 " scriptid: %d, functionname: %s"
                 " params: (%d, %d, %f, %f)",
                 scriptid,
                 functionname,
                 param0,
                 param1,
                 param2,
                 param3);
      }
    }
    int64_t _result = run_filefunction(filename, 
                                       functionname_x, 
                                       param0, 
                                       param1, 
                                       param2, 
                                       param3, 
                                       loaded);
    leave_runstep(scriptid, functionname);
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

int64_t Interface::run_scriptfunction(int32_t scriptid, 
                                      const char *functionname,
                                      int64_t param0,
                                      int64_t param1,
                                      const char *param2,
                                      const char *param3) {
  __ENTER_FUNCTION
    //进入步骤
    enter_runstep(scriptid, functionname);
    //取得脚本载入状态
    bool loaded = false;
    cache::Base *cachedata = 
      reinterpret_cast<cache::Base *>(getscript_byid(scriptid));
    if (NULL == cachedata) {
      cachedata = getscript_filedata(scriptid);
      script_loaded_.add(scriptid, cachedata);
      loaded = true;
    }
    //检查脚本载入结果
    check_scriptvalid(cachedata, scriptid, functionname);
    //载入失败
    if (NULL == cachedata) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    //载入成功
    const char *filename = cachedata->get_filename();
    if (NULL == filename) {
      leave_runstep(scriptid, functionname);
      return 0;
    }
    char functionname_x[128] = {0};
    snprintf(functionname_x, 
             sizeof(functionname_x) - 1, 
             "x%.6d_%s", 
             scriptid, 
             functionname);
    if (lua_reloadscript_always_) loaded = true;
    bool result = true;
    if (loaded) {
      result = loadscript(filename);
      if (!result) {
        leave_runstep(scriptid, functionname);
        return 0;
      }
      if (!lua_reloadscript_always_) {
        FAST_LOG(kDebugLogFile,
                 "[script.lua] (Interface::run_scriptfunction)"
                 " scriptid: %d, functionname: %s"
                 " params: (%d, %d, %s, %s)",
                 scriptid,
                 functionname,
                 param0,
                 param1,
                 param2,
                 param3);
      }
    }
    int64_t _result = run_filefunction(filename, 
                                       functionname_x, 
                                       param0, 
                                       param1, 
                                       param2, 
                                       param3, 
                                       loaded);
    leave_runstep(scriptid, functionname);
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
    FAST_LOG(kScriptLogFile,
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
    FAST_LOG(kScriptLogFile,
             "[script.lua] (Interface::enter_runstep)"
             " topindex: %d, stackspace: %d, scriptid: %d, functionname: %s",
             topindex,
             stackspace,
             scriptid,
             functionname);
  __LEAVE_FUNCTION
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
          FAST_LOG(kDebugLogFile,
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
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::is_paramnumber)"
                    " lua state is nil");
    }
    if (!lua_isnumber(L, index)) {
      FAST_ERRORLOG(kErrorLogFile,
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
      FAST_ERRORLOG(kErrorLogFile,
                    "[script.lua] (Interface::is_paramnumber)"
                    " lua state is nil");
    }
    if (!lua_isstring(L, index)) {
      FAST_ERRORLOG(kErrorLogFile,
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
    AssertEx(file, buffer);
  __LEAVE_FUNCTION
#else
  USE_PARAM(file);
  USE_PARAM(scriptid);
  USE_PARAM(functionname);
#endif
}

}; //namespace lua

}; //namespace pf_script
