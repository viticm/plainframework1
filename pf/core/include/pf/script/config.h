/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/25 19:06
 * @uses script module base config file
 */
#ifndef PF_SCRIPT_CONFIG_H_
#define PF_SCRIPT_CONFIG_H_

#include "pf/base/config.h"

#if __WINDOWS__
#define SCRIPT_ROOT_PATH_DEFAULT "public\\data\\script"
#define SCRIPT_WORK_PATH_DEFAULT "\\"
#elif __LINUX__
#define SCRIPT_ROOT_PATH_DEFAULT "public/data/script"
#define SCRIPT_WORK_PATH_DEFAULT "/"
#endif

#endif //PF_SCRIPT_CONFIG_H_
