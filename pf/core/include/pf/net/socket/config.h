/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/19 14:52
 * @uses socket module base config
 */
#ifndef PF_NET_SOCKET_CONFIG_H_
#define PF_NET_SOCKET_CONFIG_H_

#include "pf/net/config.h"

#define SOCKETINPUT_BUFFERSIZE_DEFAULT (64*1024) //default size
#define SOCKETINPUT_DISCONNECT_MAXSIZE (96*1024) //if buffer more than it,
                                                 //will disconnect this socket.
#define SOCKETOUTPUT_BUFFERSIZE_DEFAULT (8*1024)   //default size
#define SOCKETOUTPUT_DISCONNECT_MAXSIZE (100*1024)//if buffer more than it,
                                                  //will disconnect this socket.
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#ifndef SOCKET_INVALID
#define SOCKET_INVALID -1
#endif

#if __WINDOWS__
#ifndef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#endif
#ifndef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOC
#endif
#endif

#define SOCKET_WOULD_BLOCK EWOULDBLOCK //api use SOCKET_ERROR_WOULD_BLOCK
#define SOCKET_CONNECT_ERROR EINPROGRESS
#define SOCKET_CONNECT_TIMEOUT 10

#endif //PF_NET_SOCKET_CONFIG_H_
