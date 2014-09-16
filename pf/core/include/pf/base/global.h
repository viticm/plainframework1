/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id global.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/28 15:57
 * @uses the base module for global defines
 */
#ifndef PF_BASE_GLOBAL_H_
#define PF_BASE_GLOBAL_H_

#include "pf/base/config.h"

namespace pf_base {

namespace global {

extern char net_modulename[FILENAME_MAX];
extern char script_modulename[FILENAME_MAX];
extern char performance_modulename[FILENAME_MAX];
extern char db_modulename[FILENAME_MAX];
extern char engine_modulename[FILENAME_MAX];
extern char applicationname[FILENAME_MAX]; //应用全局名称
extern uint8_t applicationtype; //应用的类型 0 服务器 1 客户端

PF_API const char *get_net_modulename();
PF_API const char *get_script_modulename();
PF_API const char *get_performanace_modulename();
PF_API const char *get_db_modulename();
PF_API const char *get_engine_modulename();
PF_API const char *get_applicationname();
PF_API void set_applicationname(const char *name);
PF_API uint8_t get_applicationtype();
PF_API void set_applicationtype(uint8_t type);

}; //namespace global

}; //namespace pf_base

#define NET_MODULENAME pf_base::global::get_net_modulename()
#define SCRIPT_MODULENAME pf_base::global::get_script_modulename()
#define PERFORMANCE_MODULENAME pf_base::global::get_performanace_modulename()
#define DB_MODULENAME pf_base::global::get_db_modulename()
#define ENGINE_MODULENAME pf_base::global::get_engine_modulename()
#define APPLICATION_NAME pf_base::global::get_applicationname()
#define APPLICATION_TYPE pf_base::global::get_applicationtype()

#endif //PF_BASE_GLOBAL_H_
