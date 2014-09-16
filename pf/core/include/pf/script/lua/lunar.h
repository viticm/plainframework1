/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id lunar.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/25 17:59
 * @uses script c++ to lua
 */
/******************************************************* 
    Binding the C++ class to lua,
    get more infomation to visit: 
    http://lua-users.org/wiki/CppBindingWithLunar
 *******************************************************/
#ifndef PF_SCRIPT_LUA_LUNAR_H_
#define PF_SCRIPT_LUA_LUNAR_H_

#include "pf/script/lua/config.h"

#include <lua.hpp>
#include <stdio.h>
#include <string.h>

namespace pf_script {

namespace lua {

#if LUA_VERSION_NUM == 501

template <typename T> class Lunar {
 public:
   typedef struct { T *pT; } userdataType;
   typedef int32_t (T::*mfp)(lua_State *L);
   typedef struct { const char *name; mfp mfunc; } RegType;

   static void RegisterFunctions(lua_State *L) {
     luaL_openlib(L, T::className, T::functions, 0);
     lua_pop(L, 1);
   }

   static void Register(lua_State *L, const char *baseName = NULL) {
     lua_pushstring(L, T::className);
     lua_gettable(L, LUA_GLOBALSINDEX);
     if (lua_isnil(L, -1)) {
       lua_pop(L, 1);
       lua_newtable(L);
       lua_pushstring(L, T::className);
       lua_pushvalue(L, -2);
       //store method table in globals so that
       //scripts can add functions written in Lua.
       lua_settable(L, LUA_GLOBALSINDEX);
     }
     int32_t methods = lua_gettop(L);

     luaL_newmetatable(L, T::className);
     int32_t metatable = lua_gettop(L);

     //hide metatable from Lua getmetatable()
     lua_pushvalue(L, methods);
     set(L, metatable, "__metatable");

     lua_pushvalue(L, methods);
     set(L, metatable, "__index");

     lua_pushcfunction(L, tostring_T);
     set(L, metatable, "__tostring");

     lua_pushcfunction(L, gc_T);
     set(L, metatable, "__gc");
        
     lua_newtable(L);                // mt for method table
     lua_pushcfunction(L, new_T);
     lua_pushvalue(L, -1);           // dup new_T function
     set(L, methods, "c_new");       // add new_T to method table
     set(L, -3, "__call");           // mt.__call = new_T

     if (baseName){
       lua_pushstring(L, baseName);
       lua_gettable(L, LUA_GLOBALSINDEX);    // get base_table
       if (lua_isnil(L, -1)) {
         lua_pop(L, 1);
         lua_newtable(L);
         lua_pushstring(L, baseName);
         lua_pushvalue(L, -2);
         lua_settable(L, LUA_GLOBALSINDEX);
       }
         set(L, -3, "__index");      // mt.__index = base_table
     }

     lua_setmetatable(L, methods);

     // fill method table with methods from class T
     for (RegType *l = T::methods; l->name; l++) {
       lua_pushstring(L, l->name);
       lua_pushlightuserdata(L, (void*)l);
       lua_pushcclosure(L, thunk, 1);
       lua_settable(L, methods);
     }
       lua_pop(L, 2);  // drop metatable and method table
   }

   /*! call named lua method from userdata method table */
   static int32_t call_method(lua_State *L, 
                              const char *method,
                              int32_t nargs = 0, 
                              int32_t nresults = LUA_MULTRET, 
                              int32_t errfunc = 0) {
     int32_t base = lua_gettop(L) - nargs;  // userdata index
     if (!luaL_checkudata(L, base, T::className)) {
       lua_settop(L, base-1);           // drop userdata and args
       lua_pushfstring(L, "not a valid %s userdata", T::className);
       return -1;
     }

     lua_pushstring(L, method);         // method name
     lua_gettable(L, base);             // get method from userdata
     if (lua_isnil(L, -1)) {            // no method?
       lua_settop(L, base-1);         // drop userdata and args
       lua_pushfstring(L, "%s missing method '%s'", T::className, method);
       return -1;
     }
     lua_insert(L, base);               // put method under userdata, args

     int32_t status = lua_pcall(L, 1 + nargs, nresults, errfunc);  // call method
     if (status) {
       const char *msg = lua_tostring(L, -1);
       if (msg == NULL) msg = "(error with no message)";
         lua_pushfstring(L, "%s:%s status = %d\n%s",
             T::className, method, status, msg);
         lua_remove(L, base);           // remove old message
         return -1;
     }
       return lua_gettop(L) - base + 1;   // number of results
   }

   /*! push onto the Lua stack a userdata containing a pointer to T object */
   static int32_t push(lua_State *L, T *obj, bool gc = false) {
     if (!obj) { lua_pushnil(L); return 0; }
     luaL_getmetatable(L, T::className);  // lookup metatable in Lua registry
     if (lua_isnil(L, -1)) luaL_error(L, "%s missing metatable", T::className);
       int32_t mt = lua_gettop(L);
       subtable(L, mt, "userdata", "v");   
       userdataType *ud =
         static_cast<userdataType*>(pushuserdata(L, obj, sizeof(userdataType)));
       if (ud) {
         ud->pT = obj;  // store pointer to object in userdata
         lua_pushvalue(L, mt);
         lua_setmetatable(L, -2);
         if (gc == false) {
           lua_checkstack(L, 3);
           subtable(L, mt, "do not trash", "k");   
           lua_pushvalue(L, -2);
           lua_pushboolean(L, 1);
           lua_settable(L, -3);
           lua_pop(L, 1);
         }
       }
       lua_replace(L, mt);
       lua_settop(L, mt);
       return mt;  // index of userdata containing pointer to T object
   }

   /*! get userdata from Lua stack and return pointer to T object */
   static T *check(lua_State *L, int32_t narg) {
     userdataType *ud = 
       static_cast<userdataType*>(luaL_checkudata(L, narg, T::className));
     if (!ud) {
       luaL_typerror(L, narg, T::className);
       return NULL;
     }
     return ud->pT;  // pointer to T object
   }

   static T *nocheck(lua_State *L, int32_t narg ) {
     userdataType* ud = static_cast<userdataType*>(lua_touserdata(L, narg));
     return ud ? ud->pT : NULL;
   }
private:
   Lunar();  // hide default constructor

   /*! member function dispatcher */
   static int32_t thunk(lua_State *L) {
     // stack has userdata, followed by method args
     T *obj = check(L, 1);  // get 'self', or if you prefer, 'this'
     lua_remove(L, 1);  // remove self so member function args start at index 1
     // get member function from upvalue
     RegType *l = static_cast<RegType*>(lua_touserdata(L, lua_upvalueindex(1)));
     if (!obj || !l) {
       luaL_error( L, "Luar::calling unknown function of %s", T::className );
     }
     return (obj->*(l->mfunc))(L);  // call member function
   }

   /*! garbage collection metamethod */
   static int32_t gc_T(lua_State *L) {
     if (luaL_getmetafield(L, 1, "do not trash")) {
       lua_pushvalue(L, 1);  // dup userdata
       lua_gettable(L, -2);
       if (!lua_isnil(L, -1)) return 0;  // do not delete object
     }
     
      if (T::gc_delete_) {
        userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
        T *obj = ud->pT;
        if (obj) delete obj;  // call destructor for T objects
      }
      return 0;
   }

   // create a new T object and
   // push onto the Lua stack a userdata containing a pointer to T object
   static int32_t new_T(lua_State *L) {
     lua_remove(L, 1);   // use classname:new(), instead of classname.new()
     T *obj = new T();  // call constructor for T objects
     push(L, obj, true); // gc_T will delete this object
     return 1;           // userdata containing pointer to T object
   }

   static int32_t tostring_T(lua_State *L) {
     char buff[32];
     userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
     T *obj = ud->pT;
     sprintf(buff, "%p", obj);
     lua_pushfstring(L, "%s (%s)", T::className, buff);
     return 1;
   }

   static void set(lua_State *L, int32_t table_index, const char *key) {
     lua_pushstring(L, key);
     lua_insert(L, -2);  // swap value and key
     lua_settable(L, table_index);
   }

   static void weaktable(lua_State *L, const char *mode) {
     lua_newtable(L);
     lua_pushvalue(L, -1);  // table is its own metatable
     lua_setmetatable(L, -2);
     lua_pushliteral(L, "__mode");
     lua_pushstring(L, mode);
     lua_settable(L, -3);   // metatable.__mode = mode
   }

   static void subtable(lua_State *L, 
                        int32_t tindex, 
                        const char *name, 
                        const char *mode) {
     lua_pushstring(L, name);
     lua_gettable(L, tindex);
     if (lua_isnil(L, -1)) {
       lua_pop(L, 1);
       lua_checkstack(L, 3);
       weaktable(L, mode);
       lua_pushstring(L, name);
       lua_pushvalue(L, -2);
       lua_settable(L, tindex);
     }
   }

   static void *pushuserdata(lua_State *L, void *key, size_t sz) {
     void *ud = 0;
     lua_pushlightuserdata(L, key);
     lua_gettable(L, -2);     // lookup[key]
     if (lua_isnil(L, -1)) {
       lua_pop(L, 1);         // drop nil
       lua_checkstack(L, 3);
       ud = lua_newuserdata(L, sz);  // create new userdata
       lua_pushlightuserdata(L, key);
       lua_pushvalue(L, -2);  // dup userdata
       lua_settable(L, -4);   // lookup[key] = userdata
     }
       return ud;
   }
};

#elif LUA_VERSION_NUM == 502

template <class T> class Lunar {
 public:
   struct PropertyType {
     const char *name;
     int32_t (T::*getter) (lua_State *);
     int32_t (T::*setter) (lua_State *);
   };

   struct FunctionType {
     const char *name;
     int32_t (T::*func) (lua_State *);
   };

   /*
     Retrieves a wrapped class from the arguments passed to the func, 
     specified by narg (position).
     This func will raise an exception if 
     the argument is not of the correct type.
   */
   static T* check(lua_State *L, int narg) {
     T** obj = static_cast <T **>(luaL_checkudata(L, narg, T::className));
     if (!obj )
       return NULL; // lightcheck returns nullptr if not found.
     return *obj;    // pointer to T object
   }

   /*
    Retrieves a wrapped class from the arguments passed to the func, 
    specified by narg (position).
    This func will return nullptr if the argument is not of the correct type.  
    Useful for supporting
    multiple types of arguments passed to the func
   */ 
   static T* lightcheck(lua_State * L, int narg) {
     T** obj = static_cast <T **>(luaL_testudata(L, narg, T::className));
     if (!obj)
       return NULL; // lightcheck returns nullptr if not found.
     return *obj;    // pointer to T object
   }

   /*
    Registers your class with Lua.  
    Leave namespac "" if you want to load it into the global space.
   */
   // REGISTER CLASS AS A GLOBAL TABLE 
   static void Register(lua_State * L, const char *namespac = NULL) {
     if (namespac && strlen(namespac)) {
       lua_getglobal(L, namespac);
       if (lua_isnil(L,-1)) { // Create namespace if not present
         lua_newtable(L);
         lua_pushvalue(L,-1); // Duplicate table pointer 
                              // since setglobal pops the value
         lua_setglobal(L,namespac);
       }
       lua_pushcfunction(L, &Lunar<T>::constructor);
       lua_setfield(L, -2, T::className);
       lua_pop(L, 1);
     } else {
       lua_pushcfunction(L, &Lunar<T>::constructor);
       lua_setglobal(L, T::className);
     }
    
     luaL_newmetatable(L, T::className);
     int32_t metatable = lua_gettop(L);
    
     lua_pushstring(L, "__gc");
     lua_pushcfunction(L, &Lunar<T>::gc_obj);
     lua_settable(L, metatable);
    
     lua_pushstring(L, "__tostring");
     lua_pushcfunction(L, &Lunar<T>::to_string);
     lua_settable(L, metatable);

     lua_pushstring(L, "__eq"); // To be able to compare two Lunar objects 
                                //(not natively possible with full userdata)
     lua_pushcfunction(L, &Lunar<T>::equals);
     lua_settable(L, metatable);

     lua_pushstring(L, "__index");
     lua_pushcfunction(L, &Lunar<T>::property_getter);
     lua_settable(L, metatable);

     lua_pushstring(L, "__newindex");
     lua_pushcfunction(L, &Lunar<T>::property_setter);
     lua_settable(L, metatable);
    
     for (int32_t i = 0; T::properties[i].name; i++) { // Register some 
                                                       //properties in it
       lua_pushstring(L, T::properties[i].name);// Having some string 
                                               // associated with them
       lua_pushnumber(L, i);                    // And a number indexing 
                                               // which property it is
       lua_settable(L, metatable);
     }
    
     for (int32_t i = 0; T::methods[i].name; i++) {
       lua_pushstring(L, T::methods[i].name);  // Register some functions in it
       lua_pushnumber(L, i | ( 1 << 8 ) ); // Add a number indexing which func it is
       lua_settable(L, metatable);
     }
   }

   static int32_t constructor(lua_State * L) {
     T* ap = new T(L);
     T** a = static_cast<T**>(lua_newuserdata(L, sizeof(T *))); // Push value = userdata
     *a = ap;
     luaL_getmetatable(L, T::className); // Fetch global metatable T::classname
     lua_setmetatable(L, -2);
     return 1;
   }

   /*
    Loads an instance of the class into the Lua stack, 
    and provides you a pointer so you can modify it.
   */
   static void push(lua_State * L, T* instance) {
     T **a = static_cast<T **>(lua_newuserdata(L, sizeof(T *))); // Create userdata
     *a = instance;
     luaL_getmetatable(L, T::className); 
     lua_setmetatable(L, -2);
   }

   static int32_t property_getter(lua_State * L) {
     lua_getmetatable(L, 1); // Look up the index of a name
     lua_pushvalue(L, 2);  // Push the name
     lua_rawget(L, -2);    // Get the index
     if (lua_isnumber(L, -1)) { // Check if we got a valid index    
       int32_t _index = lua_tonumber(L, -1);
       T** obj = static_cast<T**>(lua_touserdata(L, 1));
       lua_pushvalue(L, 3);
       if (_index & (1 << 8)) { // A func
         lua_pushnumber(L, _index ^ ( 1 << 8 ) ); // Push the right func index
         lua_pushlightuserdata(L, obj);
         lua_pushcclosure(L, &Lunar<T>::function_dispatch, 2);
         return 1; // Return a func
       }
       lua_pop(L,2);    // Pop metatable and _index
       lua_remove(L,1); // Remove userdata
       lua_remove(L,1); // Remove [key]
       return ((*obj)->*(T::properties[_index].getter)) (L);
     }
     return 1;
   }

   static int32_t property_setter(lua_State * L) {
     lua_getmetatable(L, 1); // Look up the index from name
     lua_pushvalue(L, 2);  //
     lua_rawget(L, -2);    //
     if (lua_isnumber(L, -1)) { // Check if we got a valid index 
       int _index = lua_tonumber(L, -1); 
       T** obj = static_cast<T**>(lua_touserdata(L, 1));
       if (!obj || !*obj) {
         luaL_error( L , "Internal error, no object given!" );
         return 0;
       }
       if (_index >> 8) { // Try to set a func
         char c[128] = {0};
         sprintf(c , 
                 "Trying to set the method [%s] of class [%s]" , 
                 (*obj)->T::methods[_index^(1 << 8)].name , T::className);
         luaL_error(L, c);
         return 0;
       }
       lua_pop(L, 2);    // Pop metatable and _index
       lua_remove(L, 1); // Remove userdata
       lua_remove(L, 1); // Remove [key]
       return ((*obj)->*(T::properties[_index].setter))(L);
     }
     return 0;
   }

   static int32_t function_dispatch(lua_State * L) {
     int32_t i = static_cast<int32_t>(lua_tonumber(L, lua_upvalueindex(1)));
     T** obj = static_cast <T **>(lua_touserdata(L, lua_upvalueindex(2)));    
     return ((*obj)->*(T::methods[i].func))(L);
   }

   static int32_t gc_obj(lua_State * L) {
     T** obj = static_cast <T **>(lua_touserdata(L, -1));
     if (obj && *obj) delete(*obj);
     return 0;
   }
  
   static int to_string(lua_State* L) {
     T** obj = static_cast<T**>(lua_touserdata(L, -1));
     if (obj) 
       lua_pushfstring(L, "%s (%p)", T::className, (void*)*obj);
     else
       lua_pushstring(L, "Empty object");
     return 1;
   }

   /*
    * Method which compares two Luna objects.
    * The full userdatas (as opposed to light userdata) can't be 
    * natively compared one to other, we have to had this to do it.
    */
   static int equals(lua_State* L) {
     T** obj1 = static_cast<T**>(lua_touserdata(L, -1));
     T** obj2 = static_cast<T**>(lua_touserdata(L, 1));
     lua_pushboolean(L, *obj1 == *obj2);
     return 1;
   }
};

#endif
        
template <typename T>
inline void gettable_number(lua_State *L, 
                            int32_t table_index, 
                            const char *key, 
                            T& number);
template <typename T>
inline void gettable_bumber_byindex(lua_State *L, 
                                    int32_t table_index, 
                                    int32_t index, 
                                    T& number);
void gettable_string(lua_State *L, 
                     int32_t table_index, 
                     const char *key, 
                     char *string, 
                     int32_t length);
void gettable_string_byindex(lua_State *L, 
                             int32_t table_index, 
                             int32_t index, 
                             char *string, 
                             int32_t length);
int32_t gettable(lua_State *L, int32_t table_index, const char *key);
int32_t gettable(lua_State *L, int32_t table_index, int index);
int32_t dofile(lua_State *L, const char *filename);
int32_t dofile(lua_State *L);


template <typename T>
inline void gettable_number(lua_State *L, 
                            int32_t table_index, 
                            const char *key, 
                            T& number) {
  lua_pushvalue(L, table_index);
  lua_pushstring(L, key);
  lua_gettable(L, -2);
  if (!lua_isnil(L, -1) ){
    number = static_cast<T>(lua_tonumber(L, -1));
  }
  lua_pop(L, 2);
}

template <typename T>
inline void gettable_number_byindex(lua_State *L, 
                                    int32_t table_index, 
                                    int32_t index, 
                                    T& number) {
  lua_pushvalue(L, table_index);
  lua_rawgeti(L, -1, index);
  if (!lua_isnil(L, -1)) {
    number = static_cast<T>(lua_tonumber(L, -1));
  }
  lua_pop(L, 2);
}

#define LUNAR_WHILE_TABLE(L) lua_pushnil(L); while (lua_next(L, -2)) {
#define LUNAR_END_WHILE(L) lua_pop(L, 1); }
#define LUNAR_DECLARE_METHOD(Class, Name) {#Name, &Class::Name}

}; //namespace lua

}; //namespace pf_script

#endif  //PF_SCRIPT_LUA_LUNAR_H_
