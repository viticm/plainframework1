/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id base.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/23 21:22
 * @uses net connect information
 */
#ifndef PF_NET_CONNECTION_BASE_H_
#define PF_NET_CONNECTION_BASE_H_

#include "pf/net/connection/config.h"
#include "pf/net/packet/base.h"
#include "pf/net/socket/base.h"
#include "pf/net/socket/inputstream.h"
#include "pf/net/socket/outputstream.h"

struct packet_async_t {
  pf_net::packet::Base* packet;
  uint16_t packetid;
  uint32_t flag;
  packet_async_t() {
    packet = NULL;
    packetid = 0;//ID_INVALID;
    flag = kPacketFlagNone;
  };

  ~packet_async_t() {
    SAFE_DELETE(packet);
    packetid = 0;//ID_INVALID;
    flag = kPacketFlagNone;
  };
};

namespace pf_net {

namespace connection {

class PF_API Base {

 public:
   Base();
   ~Base();

 public:
   virtual bool init(); //初始化，主要是socket

 public:
   virtual bool processinput();
   virtual bool processoutput();
   virtual bool processcommand(bool option = true);
   virtual bool heartbeat(uint32_t time = 0, uint32_t flag = 0);
   virtual bool sendpacket(packet::Base* packet);

 public:
   //以下两个方法用来区分连接是服务器还是玩家连接的
   virtual bool isserver() const;
   virtual bool isplayer() const;
   //读取设置玩家连接ID的接口，该ID是由connection::Pool类分配出来的索引值
   //用于标识一个客户端连接在整个系统中的数据位置信息
   int16_t getid();
   void setid(int16_t id);
   //读取设置玩家UID的接口，userid是由UserPool类分配出来的索引值
   //用于标识玩家的游戏存储位置信息
   int16_t get_userid();
   void set_userid(int16_t id);
   //读取设置玩家连接管理器ID，managerid 是由connection::Manager类分配的索引值
   //用于标识当前玩家所在的某个connection::Manager中的位置信息，每个
   //ConnectionManager位于一个独立的线程里
   int16_t get_managerid();
   void set_managerid(int16_t id);
   //读取当前连接的socket对象
   socket::Base *getsocket();
   //断开网络连接
   virtual void disconnect();
   //当前连接是否有效
   virtual bool isvalid();
   virtual void cleanup();
   //判断当前连接是否为空块，是则释放用于新连接
   bool isempty();
   void setempty(bool status = true);
   bool isdisconnect();
   void setdisconnect(bool status = true);
   virtual void resetkick();
   uint8_t get_execute_count_pretick() const;
   void set_execute_count_pretick(uint8_t count);
   bool isinit() const;
   void setstatus(uint32_t status);
   uint32_t getstatus() const;

 public:
   uint32_t get_receive_bytes(); //获取流中接收的字节数，获取一次则重新计数
   uint32_t get_send_bytes(); //获取流中发送的字节数，获取一次则重新计数

 protected:
   int16_t id_;
   int16_t userid_;
   int16_t managerid_;
   socket::Base *socket_;
   socket::InputStream *socket_inputstream_;
   socket::OutputStream *socket_outputstream_;
   int8_t packetindex_;
   uint8_t execute_count_pretick_;
   uint32_t status_;

 private:
   bool isempty_;
   bool isdisconnect_;
   bool isinit_;
   uint32_t receive_bytes_;
   uint32_t send_bytes_;

};

}; //namespace connection

}; //namespace pap_common_net

#endif //PF_NET_CONNECTION_BASE_H_
