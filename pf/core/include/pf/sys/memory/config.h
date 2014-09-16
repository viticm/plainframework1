/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/25 20:41
 * @uses system memory base config file
 */
#ifndef PF_SYS_MEMORY_CONFIG_H_
#define PF_SYS_MEMORY_CONFIG_H_

#include "pf/sys/config.h"

typedef enum {
  kCmdModelClearAll = 1,
  kCmdModelLoadDump = 2,
  kCmdModelRecover = 3,
} cmd_model_t; //命令行模式
PF_API extern int32_t g_cmd_model;

#endif //PF_SYS_MEMORY_CONFIG_H_
