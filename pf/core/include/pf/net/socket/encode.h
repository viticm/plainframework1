/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id encode.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/20 19:11
 * @uses simple socket stream buffer encode and decode functions
 */
#ifndef PF_NET_SOCKET_ENCODE_H_
#define PF_NET_SOCKET_ENCODE_H_

#include "pf/net/socket/config.h"

namespace pf_net {

namespace socket {

namespace encode {

PF_API bool make(encodeparam_t &encodeparam);
PF_API bool skip(encodeparam_t &encodeparam, int32_t length);

}; //namespace encode

}; //namespace socket

}; //namespace pf_net

#endif //PF_NET_SOCKET_ENCODE_H_
