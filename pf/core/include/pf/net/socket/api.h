/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id api.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/19 14:25
 * @uses socket apis
 */
#ifndef PF_NET_SOCKET_API_H_
#define PF_NET_SOCKET_API_H_

#include "pf/net/socket/config.h"

#if __LINUX__
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#elif __WINDOWS__
#include <winsock.h>
#endif
#include <errno.h>

#define SOCKET_ERROR_WOULD_BLOCK -100

//解决系统函数名和命名空间冲突
int32_t sys_socket(int32_t domain, int32_t type, int32_t protocol);

namespace pf_net {

namespace socket {

namespace api {

PF_API int32_t socketex(int32_t domain, int32_t type, int32_t protocol);

PF_API bool bindex(int32_t socketid, 
                   const struct sockaddr* name, 
                   uint32_t namelength);

PF_API bool connectex(int32_t socketid, 
                      const struct sockaddr* name, 
                      uint32_t namelength);

PF_API bool listenex(int32_t socketid, uint32_t backlog);

PF_API int32_t acceptex(int32_t socketid, 
                        struct sockaddr* addr, 
                        uint32_t* addrlength);

PF_API bool getsockopt_exb(int32_t socketid, 
                           int32_t level, 
                           int32_t optname, 
                           void *optval, 
                           uint32_t* optlength);

PF_API uint32_t getsockopt_exu(int32_t socketid, 
                               int32_t level, 
                               int32_t optname, 
                               void *optval, 
                               uint32_t* optlength);

PF_API bool setsockopt_ex(int32_t socketid,
                          int32_t level,
                          int32_t optname,
                          const void *optval,
                          uint32_t optlength);

PF_API int32_t sendex(int32_t socketid, 
                      const void *buffer, 
                      uint32_t length, 
                      uint32_t flag);

PF_API int32_t sendto_ex(int32_t socketid, 
                         const void *buffer, 
                         int32_t length, 
                         uint32_t flag, 
                         const struct sockaddr* to, 
                         int32_t tolength);

PF_API int32_t recvex(int32_t socketid, 
                      void *buffer, 
                      uint32_t length, 
                      uint32_t flag);

PF_API int32_t recvfrom_ex(int32_t socketid, 
                           void *buffer, 
                           int32_t length, 
                           uint32_t flag, 
                           struct sockaddr* from, 
                           uint32_t* fromlength);

PF_API bool closeex(int32_t socketid);

PF_API bool ioctlex(int32_t socketid, int64_t cmd, uint64_t* argp);

PF_API bool get_nonblocking_ex(int32_t socketid);

PF_API bool set_nonblocking_ex(int32_t socketid, bool on);

PF_API uint32_t availableex(int32_t socketid);

PF_API bool shutdown_ex(int32_t socketid, int32_t how);

PF_API int32_t selectex(int32_t maxfdp, 
                        fd_set* readset, 
                        fd_set* writeset, 
                        fd_set* exceptset, 
                        struct timeval* timeout);

PF_API int32_t getsockname_ex(int32_t socketid, 
                              struct sockaddr *name, 
                              int32_t *namelength);

PF_API int32_t getlast_errorcode();
PF_API void getlast_errormessage(char *buffer, uint16_t length);

}; //namespace api

}; //namespace socket

}; //namespace pf_net

#endif //PF_NET_SOCKET_API_H_
