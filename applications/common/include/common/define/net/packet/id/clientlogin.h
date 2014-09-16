/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id client_tologin.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/23 19:09
 * @uses client to login id defines
 *       cn: kFirst 和 kLast 格式必须统一，后面的逗号不能省。
 *           是为了自动生成代码时能迅速的找到其位置。
 */
#ifndef COMMON_DEFINE_NET_PACKET_ID_CLIENTLOGIN_H_
#define COMMON_DEFINE_NET_PACKET_ID_CLIENTLOGIN_H_

#include "common/define/net/packet/id/config.h" //must include this

APPLICATION_DEFINE_NET_PACKET_ID_NAMESPACE_START

namespace clientlogin {

enum packetid_enum {
  kFirst = CLIENTLOGIN_PACKETID_MIN,
  kConnect,
  kLast,
  kMax = CLIENTLOGIN_PACKETID_MAX,
};

}; //namespace clientlogin

namespace client_tologin {

enum packetid_enum {
  kFirst = CLIENT_TOLOGIN_PACKETID_MIN,
  kAskLogin,
  kLast,
  kMax = CLIENT_TOLOGIN_PACKETID_MAX,
};

}; //namespace client_tologin

namespace login_toclient {

enum packetid_enum {
  kFirst = LOGIN_TOCLIENT_PACKETID_MIN,
  kTurnStatus, //排队状态
  kResultLogin, //登陆结果
  kLast,
  kMax = LOGIN_TOCLIENT_PACKETID_MAX,
};

}; //namespace login_toclient

APPLICATION_DEFINE_NET_PACKET_ID_NAMESPACE_END

#endif //COMMON_DEFINE_NET_PACKET_ID_CLIENTLOGIN_H_
