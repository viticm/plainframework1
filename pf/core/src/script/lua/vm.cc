#ifndef PF_CORE_WITH_NOLUA

#include "pf/base/string.h"
#include "pf/base/log.h"
#include "pf/base/util.h"
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
    if (!memory.malloc(static_cast<size_t>(size + 4))) {
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

void VM::checkgc(int32_t freetime) {
  __ENTER_FUNCTION
    if (!lua_state_) return;
    int32_t delta = 0;
    int32_t turn = 0;
    uint32_t memorycount1;
    uint32_t memorycount2;
    int32_t havetime = freetime;
    int32_t reclaim = 0;
    uint32_t start_tickcount = TIME_MANAGER_POINTER->get_tickcount();
    for (turn = 0; turn < 3; ++turn) {
      memorycount1 = lua_gc(lua_state_, LUA_GCCOUNT, 0);
      reclaim = havetime * 1;
      if (1 == lua_gc(lua_state_, LUA_GCSTEP, reclaim)) {
        lua_gc(lua_state_, LUA_GCRESTART, 0);
      }
      memorycount2 = lua_gc(lua_state_, LUA_GCCOUNT, 0);
      delta += memorycount1 - memorycount2;
      uint32_t current_tickcount = TIME_MANAGER_POINTER->get_tickcount();
      havetime -= (current_tickcount - start_tickcount);
      if (havetime < 40 || delta <= reclaim) break;
    }
    if (delta > 1024 * 1024 * 500) {
      char temp[128] = {0};
      uint64_t size = static_cast<uint64_t>(delta);
      pf_base::util::get_sizestr(size, temp, sizeof(temp) - 1);
      FAST_DEBUGLOG(SCRIPT_MODULENAME,
                    "[script.lua] VM::checkgc success,"
                    " freetime: %d, memory: %s",
                    freetime,
                    temp);
    }
  __LEAVE_FUNCTION
}

bool VM::callfunction(const char *name,
                      pf_base::variable_array_t &params,
                      pf_base::variable_array_t &results) {
  __ENTER_FUNCTION
    using namespace pf_base;
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    lua_getglobal(lua_state_, name);
    uint32_t paramcount = static_cast<uint32_t>(params.size());
    uint32_t resultcount = static_cast<uint32_t>(results.size());
    for (uint32_t i = 0; i < paramcount; ++i) {
      int8_t type = params[i].type;
      switch (type) {
        case kVariableTypeInvalid:
          lua_pushnil(lua_state_);
          break;
        case kVariableTypeString:
          lua_pushstring(lua_state_, params[i].string());
          break;
        case kVariableTypeBool:
          lua_pushboolean(lua_state_, params[i]._bool());
          break;
        default:
          lua_pushnumber(lua_state_, params[i]._double());
          break;
      }
    }
    int32_t call_result = lua_pcall(lua_state_, paramcount, resultcount, 0);
    if (call_result != 0) {
      on_scripterror(kErrorCodeExecute, resultcount);
      return false;
    } else {
      for (uint32_t i = 0; i < resultcount; ++i) {
        int8_t type = results[i].type;
        switch (type) {
          case kVariableTypeString:
            results[i] = lua_tostring(lua_state_, i - 1);
            break;
          case kVariableTypeBool:
            results[i] = lua_toboolean(lua_state_, i - 1);
            break;
          default:
            results[i] = lua_tonumber(lua_state_, i - 1);
            break;
        }
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::callstring(const char *string) {
  __ENTER_FUNCTION
    using namespace pf_base;
    int32_t result = 1;
    if (!lua_state_) {
      on_scripterror(kErrorCodeStateIsNil);
      return false;
    }
    std::vector<std::string> array;
    string::explode(string, array, "\t", true, true);
    if (array.size() < 1) return false;
    std::vector<std::string> _array;
    string::explode(array[0].c_str(), _array, ".", true, true);
    if (2 == _array.size()) {
      const char *table = _array[0].c_str();
      const char *field = _array[1].c_str();
      if (!get_ref(table, field)) register_ref(table, field);
      if (!get_ref(table, field)) {
        SLOW_ERRORLOG(SCRIPT_MODULENAME,
                      "[script.lua] (VM::callfunction) can't get ref,"
                      " string: %s, table: %s, field: %s",
                      string,
                      table,
                      field);
        return false;
      }
    } else {
      lua_getglobal(lua_state_, array[0].c_str());
      if (lua_isnil(lua_state_, -1)) {
        SLOW_ERRORLOG(SCRIPT_MODULENAME,
                      "[script.lua] (VM::callfunction) can't get global value,"
                      " function: %s, string: %s",
                      string,
                      array[0].c_str());
        return false;
      }
    }
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
      lua_pcall(lua_state_, (int32_t)array.size() - 1, result, 0);
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

bool VM::register_ref(const char *table, const char *field) {
  __ENTER_FUNCTION
    lua_getglobal(lua_state_, table);
    if (lua_isnil(lua_state_, -1 )) return false;
    lua_getfield(lua_state_, -1, field);
    if (lua_isnil(lua_state_, -1 )) return false;
    pf_base::variable_t key = table;
    key += ".";
    key += field;
    if (refs_.find(key.string()) != refs_.end()) return false;
    int32_t index = luaL_ref(lua_state_, LUA_REGISTRYINDEX);
    refs_[key.string()] = index;
    lua_pop(lua_state_, 1);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::get_ref(const char *table, const char *field) {
  __ENTER_FUNCTION
    pf_base::variable_t key = table;
    key += ".";
    key += field;
    if (refs_.find(key.string()) == refs_.end()) return false;
    int32_t index = refs_[key.string()];
    if (LUA_NOREF == index) return false;
    lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, index);
    if (lua_isnil(lua_state_, -1 )) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool VM::unregister_ref(const char *table, const char *field) {
  __ENTER_FUNCTION
    pf_base::variable_t key = table;
    key += ".";
    key += field;
    if (refs_.find(key.string()) == refs_.end()) return false;
    int32_t index = refs_[key.string()];
    if (LUA_NOREF == index) return false;
    luaL_unref(lua_state_, LUA_REGISTRYINDEX, index);
    refs_.erase(refs_.find(key.string()));
    return true;
  __LEAVE_FUNCTION
    return false;
}

void VM::unregister_refs() {
  __ENTER_FUNCTION
    std::map<std::string, int32_t>::iterator _iterator;
    for (_iterator = refs_.begin(); _iterator != refs_.end(); ++_iterator) {
      luaL_unref(lua_state_, LUA_REGISTRYINDEX, _iterator->second);
    }
    refs_.clear();
  __LEAVE_FUNCTION
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
    unregister_refs();
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

#endif
