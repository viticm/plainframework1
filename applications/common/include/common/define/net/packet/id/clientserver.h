/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id client_toserver.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/23 19:10
 * @uses plian server packets id define of client to server
 */
#ifndef COMMON_DEFINE_NET_PACKET_ID_CLIENTSERVER_H_
#define COMMON_DEFINE_NET_PACKET_ID_CLIENTSERVER_H_

#include "common/define/net/packet/id/config.h"

APPLICATION_DEFINE_NET_PACKET_ID_NAMESPACE_START

namespace clientserver {

enum packetid_enum {
  kFirst = CLIENTSERVER_PACKETID_MIN,
  kLast,
  kMax = CLIENTSERVER_PACKETID_MAX,
};

}; //namespace clientserver

namespace client_toserver {

enum packetid_enum {
  kFirst = CLIENT_TOSERVER_PACKETID_MIN,
  kHeartBeat,
  kLast,
  kMax = CLIENT_TOSERVER_PACKETID_MAX,
};

}; //namespace client_toserver

namespace server_toclient {

enum packetid_enum {
  kFirst = SERVER_TOCLIENT_PACKETID_MIN,
  kLast,
  kMax = SERVER_TOCLIENT_PACKETID_MAX
};

}; //namespace server_toclient

APPLICATION_DEFINE_NET_PACKET_ID_NAMESPACE_END

#endif //COMMON_DEFINE_NET_PACKET_ID_CLIENTSERVER_H_
