/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id server.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/12 14:31
 * @uses the net manager for server class
 *       cn: 服务器网络管理器类，继承框架网络管理器，最终继承了网络连接管理器
 *           网络管理器在框架设计中其实与网络连接管理器是相同的慨念
 *           外部如需使用连接管理器最好继承相关的网络管理器，
 *           比如这个管理器的类型为服务器的网络管理器
 */
#ifndef COMMON_NET_MANAGER_SERVER_H_
#define COMMON_NET_MANAGER_SERVER_H_

#include "common/net/manager/config.h"
#include "pf/net/manager.h"
#include "common/net/connection/server.h"
#include "common/setting.h"

typedef struct centerid_hash_struct {
  int16_t connectionid;
  int16_t centerid;
  centerid_hash_struct() {
    connectionid = centerid = ID_INVALID;
  }
} centerid_hash_t; //方便以后扩展

namespace common {

namespace net {

namespace manager {

class Server : public pf_net::Manager {

 public:
   Server();
   virtual ~Server();

 public:
   virtual bool remove(pf_net::connection::Base *connection);
   virtual int16_t get_current_serverid() const = 0; //子类必须实现

 public:
   server_data_t *get_current_serverinfo();
   server_data_t *get_serverinfo(int16_t serverid);
   common::net::connection::Server *get_serverconnection(int16_t serverid);
   void add_serverconnection(int16_t serverid, int16_t connectionid);
   void remove_serverconenction(int16_t serverid);
   void remove_serverconenction(pf_net::connection::Base *connection);
   void broadcast(pf_net::packet::Base *packet);
   void broadcast_center(pf_net::packet::Base *packet);
   void broadcast_server(pf_net::packet::Base *packet);
   void broadcast_login(pf_net::packet::Base *packet);
   bool connect_toserver(const char *ip, 
                         uint16_t port, 
                         int16_t serverid, 
                         int16_t &connectionid, 
                         bool create = true, 
                         bool sendconnect = true);
   //组连接服务器，在大区内连接服务器，实现跨服通讯
   pf_net::connection::Base *connect_toserver_forgroup(const char *ip, 
                                                       uint16_t port, 
                                                       int16_t serverid, 
                                                       int16_t &connectionid);

 protected:
   int16_t connectionids_[OVER_SERVER_MAX]; //以服务器ID为键的连接ID数组
   server_data_t *current_serverinfo_;

};

}; //namespace manager

}; //namespace net

}; //namespace common

#endif //COMMON_NET_MANAGER_SERVER_H_
