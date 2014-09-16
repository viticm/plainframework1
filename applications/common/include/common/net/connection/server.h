/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id server.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2013-2013 viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/06/23 14:13
 * @uses net server connection class
 *       cn: 服务器连接模块，用于服务器之间的连接，因为各个服务器都会用到，
 *           所以挪到此处，消费（网关）服务器与世界（中心）服务器需重写
 */
#ifndef COMMON_NET_CONNECTION_SERVER_H_
#define COMMON_NET_CONNECTION_SERVER_H_

#include "common/net/connection/config.h"
#include "pf/net/connection/base.h"
#include "common/setting.h"

namespace common {

namespace net {

namespace connection {

class Server : public pf_net::connection::Base {

 public:
   Server();
   ~Server();

 public:
   virtual bool init();
   virtual bool processinput();
   virtual bool processoutput();
   virtual bool processcommand(bool option = true);
   virtual void cleanup();
   virtual bool heartbeat(uint32_t time = 0);
   virtual void freeown() {};

 public:
   virtual bool isserver() const;
   virtual bool isplayer() const;
   virtual bool sendpacket(pf_net::packet::Base *packet);

 public:
   server_data_t *get_serverdata();
   void set_serverdata(server_data_t *data);
   int16_t get_serverid() const;

 private:
   server_data_t *serverdata_;

};

}; //namespace connection

}; //namespace net

}; //namespace common

#endif //COMMON_NET_CONNECTION_SERVER_H_
