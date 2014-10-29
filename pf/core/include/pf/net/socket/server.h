/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id server.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/20 11:18
 * @uses server net model socket class
 */
#ifndef PF_NET_SOCKET_SERVER_H_
#define PF_NET_SOCKET_SERVER_H_

#include "pf/net/socket/base.h"

namespace pf_net {

namespace socket {

class PF_API Server {

 public:
   Server(uint16_t port, const char *ip = NULL, uint32_t backlog = 5);
   ~Server();

 public:
   void close();
   bool accept(pf_net::socket::Base *socket);
   uint32_t getlinger() const;
   bool setlinger(uint32_t lingertime);
   bool is_nonblocking() const;
   bool set_nonblocking(bool on = true);
   uint32_t getreceive_buffersize() const;
   bool setreceive_buffersize(uint32_t size);
   uint32_t getsend_buffersize() const;
   bool setsend_buffersize(uint32_t size);
   int32_t getid() const;
   uint16_t getport() const;

 protected:
   pf_net::socket::Base* socket_;

};

}; //namespace socket

}; //namespace pf_net

#endif //PF_NET_SOCKET_SERVER_H_
