/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/23 13:47
 * @uses the net connection base config file
 */
#ifndef PF_NET_CONNECTION_CONFIG_H_
#define PF_NET_CONNECTION_CONFIG_H_

#include "pf/net/config.h"

//默认每帧执行的消息数量上限
#define NET_CONNECTION_EXECUTE_COUNT_PRE_TICK_DEFAULT 12
#define NET_CONNECTION_MAX 1024
#define NET_CONNECTION_CACHESIZE_MAX 1024
#define NET_CONNECTION_KICKTIME 6000000 //超过该时间则断开连接
#define NET_CONNECTION_INCOME_KICKTIME 60000
#define NET_CONNECTION_POOL_SIZE_DEFAULT 1280 //连接池默认大小

enum {
  kConnectionCompressNone = 0, //不压缩
  kConnectionCompressInput, //只压缩输入流
  kConnectionCompressOutput, //只压缩输出流
  kConnectionCompressAll, //输入流和输出流都压缩
}; //连接压缩模式

#endif //PF_NET_CONNECTION_CONFIG_H_
