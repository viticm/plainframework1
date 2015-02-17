#include "pf/base/string.h"
#include "pf/base/log.h"
#include "pf/sys/memory/dynamic_allocator.h"
#include "pf/script/lua/vm.h"

namespace pf_script {

namespace lua {

VM::VM(const char *rootpath, const char *workpath) {
  __ENTER_FUNCTION
    lua_state_ = NULL;
    if (rootpath != NULL) filebridge_.set_rootpath(rootpath);
    if (workpath != NULL) filebridge_.set_workpath(workpath);
  __LEAVE_FUNCTION
}

VM::~VM() {
  __ENTER_FUNCTION
    release();
  __LEAVE_FUNCTION
}

bool VM::loadbuffer(unsigned char *buffer, uint64_t length) {
  __ENTER_FUNCTION
    if (length < 0) {
      on_scripterror(kErrorCodeLength);
      return false;
    }
    char *_buffer = reinterpret_cast<char *>(buffer);
    if (luaL_loadbuffer(lua_state_, 
                        _buffer, 
                        static_cast<size_t>(length), 
                        NULL) != 0) {
      on_scripterror(kErrorCodeLoadBuffer);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::load(const char *filename) {
  __ENTER_FUNCTION
    using namespace pf_sys;
    uint64_t size;
    if (!filebridge_.open(filename)) {
      SLOW_ERRORLOG(SCRIPT_MODULENAME,
                    "[script.lua] (VM::load) open file %s failed",
                    filename);
      return false;
    }
    size = filebridge_.size();
    memory::DynamicAllocator memory;
    if (!memory.malloc(size + 4)) {
      SLOW_ERRORLOG(SCRIPT_MODULENAME,
                    "[script.lua] (VM::load) memory for file %s failed",
                    filename);
      filebridge_.close();
      return false;
    }
    if (filebridge_.read(memory.getpointer(), size) != size) {
      SLOW_ERRORLOG(SCRIPT_MODULENAME,
                    "[script.lua] (VM::load) read file %s failed",
                    filename);
      filebridge_.close();
      return false;
    }
    char *memory_pointer = reinterpret_cast<char *>(memory.getpointer());
    memory_pointer[size + 1] = '\0';
    filebridge_.close();
    try {
      unsigned char *temp_pointer = 
        reinterpret_cast<unsigned char *>(memory.getpointer());
      if (!loadbuffer(temp_pointer, size)) {
        SLOW_ERRORLOG(SCRIPT_MODULENAME,
                      "[script.lua] (VM::load) load file %s"
                      " buffer cache failed",
                      filename);
        return false;
      }
    } catch (...) {
      SLOW_ERRORLOG(SCRIPT_MODULENAME,
                    "[script.lua] (VM::load) load file %s"
                    " buffer cache have a exception",
                    filename);
      return false;
    }
    if (!executecode()) {
      SLOW_ERRORLOG(SCRIPT_MODULENAME,
                    "[script.lua] (VM::load) execute code"
                    " failed from file %s",
                    filename);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::executecode() {
  __ENTER_FUNCTION
    if (!lua_state_) {
      on_scripterror(kErrorCodeExecute);
      return false;
    }
    int32_t state = lua_pcall(lua_state_, 0, LUA_MULTRET, 0);
    if (state != 0) {
      on_scripterror(kErrorCodeExecute, state);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callfunction(const char *name, 
                      int32_t result, 
                      const char *format, ...) {
  __ENTER_FUNCTION
    va_list vlist;
    double number;
    char *string = NULL;
    void *pointer = NULL;
    lua_CFunction cfunction;
    int32_t i = 0;
    int32_t args_number = 0;
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    lua_getglobal(lua_state_, name);
    va_start(vlist, format);
    while (format[i]) {
      switch (format[i]) {
        case 'n': { //number double
          number = va_arg(vlist, double);
          lua_pushnumber(lua_state_, number);
          ++args_number;
          break;
        }
        case 'd': { //int
          number = static_cast<lua_Number>(va_arg(vlist, int64_t));
          lua_pushnumber(lua_state_, number);
          ++args_number;
          break;
        }
        case 's': { //string
          string = va_arg(vlist, char *);
          lua_pushstring(lua_state_, string);
          ++args_number;
          break;
        }
        case 'N': { //nil
          lua_pushnil(lua_state_);
          ++args_number;
          break;
        }
        case 'f': { //c function
          cfunction = va_arg(vlist, lua_CFunction);
          lua_pushcfunction(lua_state_, cfunction);
          ++args_number;
          break;
        }
        case 'v': { //stack index type
          number = va_arg(vlist, int32_t);
          int32_t index = static_cast<int32_t>(number);
          lua_pushvalue(lua_state_, index);
          ++args_number;
          break;
        }
        case 't': { //table
          break;
        }
        case 'p': { //pointer
          pointer = va_arg(vlist, void *);
          lua_pushlightuserdata(lua_state_, pointer);
          ++args_number;
          break;
        }
      }
      ++i;
    }
    va_end(vlist);
    int32_t call_result = lua_pcall(lua_state_, args_number, result, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, result);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callfunction(const char *function) {
  __ENTER_FUNCTION
    using namespace pf_base;
    int32_t result = 0;
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    std::vector<std::string> array;
    string::explode(function, array, "\t", true, true);
    if (array.size() < 1) return false;
    lua_getglobal(lua_state_, array[0].c_str());
    for (int32_t i = 1; i < static_cast<int32_t>(array.size()); ++i) {
      char value[512] = {0};
      string::safecopy(value, array[i].c_str(), sizeof(value));
      if ('\"' == value[0] && 
          '\"' == value[strlen(value) - 1]) {
        value[strlen(value) - 1] = '\0';
        lua_pushstring(lua_state_, value + 1);
      } else {
        lua_pushnumber(lua_state_, 
                       static_cast<lua_Number>(atof(value)));
      }
    }
    int32_t call_result = 
      lua_pcall(lua_state_, (int)array.size() - 1, result, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, result);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callfunction(const char *name, int32_t result) {
  __ENTER_FUNCTION
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    lua_getglobal(lua_state_, name);
    int32_t call_result = lua_pcall(lua_state_, 0, result, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, result);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callfunction(const char *name, int32_t result, int64_t param0) {
  __ENTER_FUNCTION
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    lua_getglobal(lua_state_, name);
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param0));
    int32_t call_result = lua_pcall(lua_state_, 1, result, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, result);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callfunction(const char *name, 
                      int32_t result, 
                      int64_t param0,
                      int64_t param1) {
  __ENTER_FUNCTION
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    lua_getglobal(lua_state_, name);
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param0));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param1));
    int32_t call_result = lua_pcall(lua_state_, 2, result, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, result);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callfunction(const char *name, 
                      int32_t result, 
                      int64_t param0,
                      int64_t param1,
                      int64_t param2) {
  __ENTER_FUNCTION
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    lua_getglobal(lua_state_, name);
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param0));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param1));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param2));
    int32_t call_result = lua_pcall(lua_state_, 3, result, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, result);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callfunction(const char *name, 
                      int32_t result, 
                      int64_t param0,
                      int64_t param1,
                      int64_t param2,
                      int64_t param3) {
  __ENTER_FUNCTION
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    lua_getglobal(lua_state_, name);
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param0));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param1));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param2));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param3));
    int32_t call_result = lua_pcall(lua_state_, 4, result, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, result);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callfunction(const char *name, 
                      int32_t result, 
                      int64_t param0,
                      int64_t param1,
                      int64_t param2,
                      int64_t param3,
                      int64_t param4) {
  __ENTER_FUNCTION
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    lua_getglobal(lua_state_, name);
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param0));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param1));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param2));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param3));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param4));
    int32_t call_result = lua_pcall(lua_state_, 5, result, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, result);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callfunction(const char *name, 
                      int32_t result, 
                      int64_t param0,
                      int64_t param1,
                      int64_t param2,
                      int64_t param3,
                      int64_t param4,
                      int64_t param5) {
  __ENTER_FUNCTION
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    lua_getglobal(lua_state_, name);
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param0));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param1));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param2));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param3));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param4));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param5));
    int32_t call_result = lua_pcall(lua_state_, 6, result, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, result);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callfunction(const char *name, 
                      int32_t result, 
                      int64_t param0,
                      int64_t param1,
                      int64_t param2,
                      int64_t param3,
                      int64_t param4,
                      int64_t param5,
                      int64_t param6) {
  __ENTER_FUNCTION
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    lua_getglobal(lua_state_, name);
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param0));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param1));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param2));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param3));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param4));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param5));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param6));
    int32_t call_result = lua_pcall(lua_state_, 7, result, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, result);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callfunction(const char *name, 
                      int32_t result, 
                      int64_t param0,
                      int64_t param1,
                      int64_t param2,
                      int64_t param3,
                      int64_t param4,
                      int64_t param5,
                      int64_t param6,
                      int64_t param7) {
  __ENTER_FUNCTION
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    lua_getglobal(lua_state_, name);
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param0));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param1));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param2));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param3));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param4));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param5));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param6));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param7));
    int32_t call_result = lua_pcall(lua_state_, 8, result, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, result);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callfunction(const char *name, 
                      int32_t result, 
                      int64_t param0,
                      int64_t param1,
                      float param2,
                      float param3) {
  __ENTER_FUNCTION
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    lua_getglobal(lua_state_, name);
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param0));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param1));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param2));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param3));
    int32_t call_result = lua_pcall(lua_state_, 4, result, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, result);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callfunction(const char *name, 
                      int32_t result, 
                      int64_t param0,
                      int64_t param1,
                      const char *param2,
                      const char *param3) {
  __ENTER_FUNCTION
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    lua_getglobal(lua_state_, name);
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param0));
    lua_pushnumber(lua_state_, static_cast<lua_Number>(param1));
    lua_pushstring(lua_state_, param2);
    lua_pushstring(lua_state_, param3);
    int32_t call_result = lua_pcall(lua_state_, 4, result, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, result);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::init(int32_t stacksize) {
  __ENTER_FUNCTION
    if (!lua_state_) {
      lua_state_ = luaL_newstate();
      if (!lua_state_) {
        on_scripterror(kErrorCodeCreate);
        return false;
      }
      if (lua_checkstack(lua_state_, stacksize) != 1) {
        on_scripterror(kErrorCodeResize);
        return false;
      }
    }
    on_init();
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::register_function(const char *name, void *pointer) {
  __ENTER_FUNCTION
    if (!lua_state_) return false;
    lua_register(lua_state_, name, reinterpret_cast<lua_CFunction>(pointer));
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::register_functiontable(
    const char *name, const struct luaL_Reg regtable[]) {
  __ENTER_FUNCTION
    if (!lua_state_) return false;
    lua_getglobal(lua_state_, name);
    if (lua_isnil(lua_state_, -1)) lua_newtable(lua_state_); //没有才创建
    for (; regtable->name != NULL; ++regtable) {
      lua_pushstring(lua_state_, regtable->name);
      lua_pushcfunction(lua_state_, regtable->func);
      lua_settable(lua_state_, -3);
    }
    lua_setglobal(lua_state_, name);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::register_globalvalue(const char *name, const char *value) {
  __ENTER_FUNCTION
    if (!lua_state_) return false;
    lua_getglobal(lua_state_, name);
    lua_pushstring(lua_state_, value);
    lua_setglobal(lua_state_, name);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::register_globalvalue(const char *name, lua_Number value) {
  __ENTER_FUNCTION
    if (!lua_state_) return false;
    lua_getglobal(lua_state_, name);
    lua_pushnumber(lua_state_, value);
    lua_setglobal(lua_state_, name);
    return true;
  __LEAVE_FUNCTION
    return false;
}

void VM::on_init() {
  __ENTER_FUNCTION
    if (!lua_state_) return;
    luaopen_base(lua_state_);
    luaL_openlibs(lua_state_);
  __LEAVE_FUNCTION
}

void VM::release() {
  __ENTER_FUNCTION
    if (!lua_state_) return;
    lua_close(lua_state_);
    lua_state_ = NULL;
  __LEAVE_FUNCTION
}

void VM::set_rootpath(const char *path) {
  __ENTER_FUNCTION
    filebridge_.set_rootpath(path);
  __LEAVE_FUNCTION
}

void VM::set_workpath(const char *path) {
  __ENTER_FUNCTION
    filebridge_.set_workpath(path);
  __LEAVE_FUNCTION
}

void VM::on_scripterror(int32_t error) {
  __ENTER_FUNCTION
    SLOW_ERRORLOG(SCRIPT_MODULENAME,
                  "[script.lua] (VM::on_scripterror) code: %d, message: %s",
                  error,
                  get_lastresult());
  __LEAVE_FUNCTION
}

void VM::on_scripterror(int32_t error, int32_t error1) {
  __ENTER_FUNCTION
    SLOW_ERRORLOG(SCRIPT_MODULENAME,
                  "[script.lua] (VM::on_scripterror) code: %d:[%d], message: %s",
                  error,
                  error1, 
                  get_lastresult());
  __LEAVE_FUNCTION
}

void VM::callfunction_enter(int32_t *index) {
  __ENTER_FUNCTION
    if (!lua_state_) return;
    *index = lua_gettop(lua_state_);
  __LEAVE_FUNCTION
}

void VM::callfunction_leave(int32_t index) {
  __ENTER_FUNCTION
    if (!lua_state_) return;
    lua_settop(lua_state_, index);
  __LEAVE_FUNCTION
}

const char *VM::get_rootpath() {
  __ENTER_FUNCTION
    const char *path = filebridge_.get_rootpath();
    return path;
  __LEAVE_FUNCTION
    return NULL;
}
const char *VM::get_lastresult() {
  const char *result= lua_tostring(lua_state_, lua_gettop(lua_state_));
  return result;  
}

} //namespace lua

} //namespace pf_script
