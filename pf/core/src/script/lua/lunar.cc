#include "pf/base/string.h"
#include "pf/script/lua/lunar.h"

namespace pf_script {

namespace lua {

void gettable_string(lua_State *L, 
                     int32_t table_index, 
                     const char *key, 
                     char *string, 
                     int32_t length) {
  __ENTER_FUNCTION
    using namespace pf_base;
    lua_pushvalue(L, table_index);
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    if (!lua_isnil(L, -1) ) {
      string::safecopy(string, lua_tostring(L, -1), length);
    }
    lua_pop(L, 2);
  __LEAVE_FUNCTION
}

void gettable_string_byindex(lua_State *L, 
                             int32_t table_index, 
                             int32_t index, 
                             char *string, 
                             int32_t length) {
  __ENTER_FUNCTION
    using namespace pf_base;
    lua_pushvalue(L, table_index);
    lua_rawgeti(L, -1, index);  
    if (!lua_isnil(L, -1)) {
      string::safecopy(string, lua_tostring(L, -1), length);
    }
    lua_pop(L, 2);
  __LEAVE_FUNCTION
}

int32_t gettable(lua_State *L, int32_t table_index, const char *key) {
  __ENTER_FUNCTION
    lua_pushvalue(L, table_index);    
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    if (lua_isnil(L, -1) || !lua_istable(L, -1)) {
      lua_pop(L, 2);
      return -1;
    }
    else {
      lua_insert(L, -2);
      lua_pop(L, 1);
      return lua_gettop(L);
    }
  __LEAVE_FUNCTION
    return -1;
}

int32_t gettable(lua_State *L, int32_t table_index, int32_t index) {
  __ENTER_FUNCTION
    lua_pushvalue(L, table_index);
    lua_pushnumber(L, index);
    lua_gettable(L, -2);
    if (lua_isnil(L, -1) || !lua_istable(L, -1)) {
      lua_pop(L, 2);
      return -1;
    }
    else {
      lua_insert(L, -2);
      lua_pop(L, 1);
      return lua_gettop(L);
    }
  __LEAVE_FUNCTION
    return -1;
}

int32_t dofile(lua_State* L, const char *filename ) {
  __ENTER_FUNCTION
    if (luaL_dofile(L, filename)) return 1;
    return 0;
  __LEAVE_FUNCTION
    return 0;
}

int32_t dofile(lua_State *L) {
  __ENTER_FUNCTION
    const char *fileName = lua_tostring(L, -1);
    int ret = dofile(L, fileName);
    lua_pushnumber(L, (lua_Number)ret);
    return 0;
  __LEAVE_FUNCTION
    return 0;
}

} //namespace lua

} //namespace pf_script
