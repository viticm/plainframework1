/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id enum.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/06 15:17
 * @uses the common define for applications
 */
#ifndef COMMON_DEFINE_ENUM_H_
#define COMMON_DEFINE_ENUM_H_

#ifdef __SERVER__

typedef enum {
  kPlayerStatusLoginEmpty = 1,
  kPlayerStatusLoginProcessTurn, //登陆排队中
  kPlayerStatusLoginConnect, //登陆连接中
  kPlayerStatusLoginServerReady, //登陆服务器已准备
  kPlayerStatusLoginNormal, //登陆正常
} playerstatus_t;

typedef enum {
  kServerTypeServer = 0, //游戏服务器
  kServerTypeLogin, //登陆服务器
  kServerTypeShareMemory, //共享内存
  kServerTypeCenter, //中心服务器
  kServerTypeGateway, //网关服务器
  kServerTypeNumber,
} servertype_t;

#endif

typedef enum {
  kLoginTurnStatusInTurn = 0, //登陆排队中
  kLoginTurnStatusNormal
} login_turnstatus_t;

#endif //COMMON_DEFINE_ENUM_H_
