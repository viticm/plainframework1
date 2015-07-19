/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/23 11:05
 * @uses the net packet config file
 */
#ifndef PF_NET_PACKET_CONFIG_H_
#define PF_NET_PACKET_CONFIG_H_

#include "pf/net/config.h"

#define NET_PACKET_DYNAMIC_ONCESIZE (1024) //动态网络包每次重新增加的内存大小
#define NET_PACKET_DYNAMIC_SIZEMAX (1024 * 100) //动态网络包的最大内存大小

#endif //PF_NET_PACKET_CONFIG_H_
