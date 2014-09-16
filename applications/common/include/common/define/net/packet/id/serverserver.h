/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id serverserver.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/23 11:21
 * @uses 服务器与服务器之间的通讯包
 */
#ifndef COMMON_DEFINE_NET_PACKET_ID_SERVERSERVER_H_
#define COMMON_DEFINE_NET_PACKET_ID_SERVERSERVER_H_

#include "common/define/net/packet/id/config.h" //must include this

APPLICATION_DEFINE_NET_PACKET_ID_NAMESPACE_START

namespace serverserver { //服务器与服务器公用

typedef enum {
  kFirst = 0, //起始ID
  kConnect,
  kLast, /* the last packetid */
  kMax = 7,
 } packetid_enum;

}; //namespace serverserver

APPLICATION_DEFINE_NET_PACKET_ID_NAMESPACE_END

#endif //COMMON_DEFINE_NET_PACKET_ID_SERVERSERVER_H_
