/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/25 19:04
 * @uses script lua module config file
 */
#ifndef PF_SCRIPT_LUA_CONFIG_H_
#define PF_SCRIPT_LUA_CONFIG_H_

#include "pf/script/config.h"

#define SCRIPT_LUA_STACK_STEP_MAX 48
#define SCRIPT_LUA_STACK_FUNCTION_LENGTH_MAX 256
#define SCRIPT_LUA_SCENE_MAX 4096
#define SCRIPT_LUA_GLOBAL_VAR_FILE_DEFAULT "/global.lua"
//使用该宏需包含lua头文件，以及包含base/log.h，用来检测lua堆栈中的参数
#define SCRIPT_LUA_CHECKARGC(L, argc) { \
  Assert(L); \
  int32_t _argc = lua_gettop(L); \
  if (_argc != argc) { \
    SLOW_ERRORLOG(SCRIPT_MODULENAME, \
                  "[script.lua] file: %s, line: %d, argc: %d, expect: %d", \
                  __FILE__, \
                  __LINE__, \
                  _argc, \
                  argc);\
    return 0; \
  } \
}
#define LUA_COMPAT_MODULE //兼容新老版本

#endif //PF_SCRIPT_LUA_CONFIG_H_
