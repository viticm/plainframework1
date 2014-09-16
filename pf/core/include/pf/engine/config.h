/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/30 15:41
 * @uses the engine module config file
 */
#ifndef PF_ENGINE_CONFIG_H_
#define PF_ENGINE_CONFIG_H_

#include "pf/base/config.h"

#define ENGINE_KERNEL_FRAME 100 //引擎核心的帧率

/* bool config { */
#define ENGINE_CONFIG_DB_ISACTIVE 1
#define ENGINE_CONFIG_NET_ISACTIVE 2
#define ENGINE_CONFIG_SCRIPT_ISACTIVE 3
#define ENGINE_CONFIG_PERFORMANCE_ISACTIVE 4
#define ENGINE_CONFIG_DB_RUN_ASTHREAD 5
#define ENGINE_CONFIG_NET_RUN_ASTHREAD 6
#define ENGINE_CONFIG_SCRIPT_RUN_ASTHREAD 7
#define ENGINE_CONFIG_PERFORMANCE_RUN_ASTHREAD 8
#define ENGINE_CONFIG_BASEMODULE_HAS_INIT 9 //扩展基础模块，可以独立引擎初始化
#define ENGINE_CONFIG_BOOL_MAX 20 //引擎核心布尔值配置最大的索引值
/* } bool config */

/* int32_t config { */
#define ENGINE_CONFIG_DB_CONNECTOR_TYPE 1
#define ENGINE_CONFIG_NET_LISTEN_PORT 2
#define ENGINE_CONFIG_NET_CONNECTION_MAX 3
#define ENGINE_CONFIG_INT32_MAX 20 //引擎核心整型配置最大的索引值 
/* } int32_t config */

/* string config { */
#define ENGINE_CONFIG_DB_CONNECTION_OR_DBNAME 1
#define ENGINE_CONFIG_DB_USERNAME 2
#define ENGINE_CONFIG_DB_PASSWORD 3
#define ENGINE_CONFIG_SCRIPT_ROOTPATH 4
#define ENGINE_CONFIG_SCRIPT_WORKPATH 5
#define ENGINE_CONFIG_SCRIPT_GLOBALFILE 6
#define ENGINE_CONFIG_NET_LISTEN_IP 7
#define ENGINE_CONFIG_STRING_MAX 20 //引擎核心字符串配置最大的索引值  
/* } string config*/

#endif //PF_ENGINE_CONFIG_H_
