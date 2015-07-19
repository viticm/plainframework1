/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id manager.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/23 14:00
 * @uses connection manager class
 */
#ifndef PF_NET_CONNECTION_MANAGER_BASE_H_
#define PF_NET_CONNECTION_MANAGER_BASE_H_

#include "pf/net/connection/manager/config.h"
#include "pf/sys/thread.h"
#include "pf/net/socket/base.h"
#include "pf/net/socket/server.h"
#include "pf/net/packet/base.h"
#include "pf/net/connection/base.h"
#include "pf/net/connection/pool.h"

namespace pf_net {

namespace connection {

namespace manager {

class PF_API Base {

 public:
   Base();
   virtual ~Base();
   
 public:
   uint64_t threadid_;
 
 public:
   bool init(uint16_t maxcount = NET_CONNECTION_MAX,
             uint16_t listenport = 0,
             const char *listenip = NULL);
   bool init_pool(uint16_t connectionmax = NET_CONNECTION_MAX);
   void init_pool(connection::Pool *pool);
   bool add(connection::Base *connection);
   bool add(int16_t id);
   connection::Base *get(int16_t id);
   virtual connection::Base *accept(); //新连接接受处理
   //以下为连接方法，只应工作在非服务器模式下
   virtual connection::Base *connect(const char *ip, uint16_t port);
   virtual connection::Base *connectgroup(const char *ip, uint16_t port);
   virtual bool heartbeat(uint32_t time = 0);
   //设置poll池，如果是外部设置服务器模式则一定要在此方法之前。
   virtual bool set_poll_maxcount(uint16_t maxcount);
   //从管理器中移除连接
   virtual bool remove(int16_t id);
   //删除连接包括管理器、socket
   virtual bool erase(connection::Base *connection);
   //彻底删除连接，管理器、socket、pool
   virtual bool remove(connection::Base *connection);
   //清除管理器中所有连接
   virtual bool destroy();
   virtual bool addsocket(int32_t socketid, int16_t connectionid) = 0;
   virtual bool removesocket(int32_t socketid) = 0;

 public:
   int16_t *get_idset();
   uint16_t getcount() const;
   uint16_t get_maxcount() const { return maxcount_; }
   bool hash();
   uint16_t get_listenport() const;
   uint16_t get_connectionmax() const;
   connection::Base *get(uint16_t id);
   connection::Pool *getpool();
   int32_t get_onestep_accept() const;
   void set_onestep_accept(int32_t count);
   uint64_t get_send_bytes();
   uint64_t get_receive_bytes();
   bool isinit() const;
   void set_servermode(bool flag);

 public: //Packet queue, can work in mutli thread.
   virtual bool sendpacket(packet::Base *packet, 
                           uint16_t connectionid, 
                           uint32_t flag = kPacketFlagNone);
   virtual bool process_cachecommand();
   virtual bool recvpacket(packet::Base *&packet, 
                           uint16_t &connectionid, 
                           uint32_t &flag);
   bool cacheresize();

 protected:
   uint16_t listenport_;
   uint16_t connectionmax_;
   bool is_servermode_; //是否为服务器模式，否则为客户端模式
   //用于侦听的服务器Socket
   socket::Server *serversocket_;
   //用于侦听的服务器SOCKET句柄值（此数据即serversocket_内拥有的SOCKET句柄值）
   int32_t socketid_;
   int32_t fdsize_; //实际的网络连接数量，正在连接的，
                    //其实和count_一样，不过此值只用于轮询模式
   bool isinit_; //是否初始化，防止再次初始化

 protected:
   int16_t *connection_idset_; //连接的ID数组
   uint16_t maxcount_; //连接的最大数量
   uint16_t count_; //连接的当前数量
   uint64_t send_bytes_; //发送字节数
   uint64_t receive_bytes_; //接收字节数
   int32_t onestep_accept_; //一帧内接受的新连接数量, -1无限制
   connection::Pool *pool_;   
   cache_t cache_;
   pf_sys::ThreadLock lock_;

};

}; //namespace manager

}; //namespace connection

}; //namespace pf_net

#endif //PF_NET_CONNECTION_MANAGER_BASE_H_
