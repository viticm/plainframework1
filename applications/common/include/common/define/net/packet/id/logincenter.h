/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id logincenter.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/23 19:35
 * @uses net packets id define of login and center server
 */
#ifndef COMMON_DEFINE_NET_PACKET_ID_LOGINCENTER_H_
#define COMMON_DEFINE_NET_PACKET_ID_LOGINCENTER_H_

#include "common/define/net/packet/id/config.h"

APPLICATION_DEFINE_NET_PACKET_ID_NAMESPACE_START

namespace logincenter {

enum packetid_enum {
  kFirst = LOGINCENTER_PACKETID_MIN,
  kLast,
  kMax = LOGINCENTER_PACKETID_MAX
};

}; //namespace logincenter

namespace login_tocenter {

enum packetid_enum {
  kFirst = LOGIN_TOCENTER_PACKETID_MIN,
  kRequirePlayerLogin,
  kLast,
  kMax = LOGIN_TOCENTER_PACKETID_MAX
};

}; //namespace login_tocenter

namespace center_tologin {

enum packetid_enum {
  kFirst = CENTER_TOLOGIN_PACKETID_MIN,
  kLast,
  kMax = CENTER_TOLOGIN_PACKETID_MAX
};

}; //namespace center_tologin

APPLICATION_DEFINE_NET_PACKET_ID_NAMESPACE_END

#endif //COMMON_DEFINE_NET_PACKET_ID_LOGINCENTER_H_
