/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id base.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/20 11:54
 * @uses server and client net pakcet class
 */
#ifndef PF_NET_PACKET_BASE_H_
#define PF_NET_PACKET_BASE_H_

#include "pf/net/socket/config.h"
#include "pf/net/socket/inputstream.h"
#include "pf/net/socket/outputstream.h"

#define NET_PACKET_GETINDEX(a) ((a) >> 24)
#define NET_PACKET_SETINDEX(a,index) ((a) = (((a) & 0xffffff) + ((index) << 24)))
#define NET_PACKET_GETLENGTH(a) ((a) & 0xffffff)
#define NET_PACKET_SETLENGTH(a,length) ((a) = ((a) & 0xff000000) + (length))
//note cn:
//消息头中包括：uint16_t - 2字节；uint32_t - 4字节中高位一个字节为消息序列号，
//其余三个字节为消息长度
//通过GET_PACKETINDEX和GET_PACKETLENGTH宏，
//可以取得UINT数据里面的消息序列号和长度
//通过SET_PACKETINDEX和SET_PACKETLENGTH宏，
//可以设置UINT数据里面的消息序列号和长度
#define NET_PACKET_HEADERSIZE (sizeof(uint16_t) + sizeof(uint32_t))

typedef enum {
  kPacketExecuteStatusError = 0, //表示出现严重错误，当前连接需要被强制断开 
  kPacketExecuteStatusBreak, //表示返回后剩下的消息将不在当前处理循环里处理
  kPacketExecuteStatusContinue, //表示继续在当前循环里执行剩下的消息
  kPacketExecuteStatusNotRemove, //表示继续在当前循环里执行剩下的消息,
                                  //但是不回收当前消息
  kPacketExecuteStatusNotRemoveError,
} packet_executestatus_t;

typedef enum {
  kPacketFlagNone = 0,
  kPacketFlagRemove,
} packetflag_t;

namespace pf_net {

namespace packet {

class PF_API Base {

 public:
   Base();
   virtual ~Base();
 
 public:
   int8_t status_;
   int8_t index_;

 public:
   virtual void cleanup() {};
   virtual bool read(socket::InputStream &inputstream) = 0;
   virtual bool write(socket::OutputStream &outputstream) = 0;
   virtual uint32_t execute(connection::Base *connection);
   virtual uint16_t getid() const = 0;
   virtual uint32_t getsize() const = 0;
   virtual void setid(uint16_t id);
   virtual void setsize(uint32_t size);
   int8_t getindex() const;
   void setindex(int8_t index);
   uint8_t getstatus() const;
   void setstatus(uint8_t status);

};

}; //namespace packet

}; //namespace pf_net

#endif //PF_NET_PACKET_BASE_H_
