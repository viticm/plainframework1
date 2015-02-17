/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id outputstream.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/21 12:03
 * @uses socket output stream class
 */
#ifndef PF_NET_SOCKET_OUTPUTSTREAM_H_
#define PF_NET_SOCKET_OUTPUTSTREAM_H_

#include "pf/net/packet/base.h"
#include "pf/net/socket/base.h"
#include "pf/net/socket/stream.h"

//提供给write_*系列方法的最大缓存包大小，即写一个包最大的大小为10k
//注意：write_*方法需要手动调用方法（add_tostream）加入到输出流中，
//否则缓存数据不会被发送，出于性能考虑暂不使用此种方式
//#define NET_SOCKET_OUTPUTSTREAM_CACHESIZE_MAX (1024*10)

namespace pf_net {

namespace socket {

class PF_API OutputStream : public Stream {

 public:
   OutputStream(
     socket::Base* socket, 
       uint32_t bufferlength = SOCKETOUTPUT_BUFFERSIZE_DEFAULT,
       uint32_t bufferlength_max = SOCKETOUTPUT_DISCONNECT_MAXSIZE)
     : Stream(socket, bufferlength, bufferlength_max), tail_(0) {};
   virtual ~OutputStream() {};

 public:
   void cleanup();

 public:
   uint32_t write(const char *buffer, uint32_t length);
   bool writepacket(const packet::Base *packet);
   int32_t flush();

 public: //write_*常用方法
   bool write_int8(int8_t value);
   bool write_uint8(uint8_t value);
   bool write_int16(int16_t value);
   bool write_uint16(uint16_t value);
   bool write_int32(int32_t value);
   bool write_uint32(uint32_t value);
   bool write_int64(int64_t value);
   bool write_uint64(uint64_t value);
   bool write_string(const char *value);
   bool write_float(float value);
   bool write_dobule(double value);

 private: //compress mode is enable, use this functions replace normals.
   uint32_t get_floortail();
   void compressenable(bool enable);
   bool compress(uint32_t tail);
   int32_t compressflush();
   int32_t rawflush();
   bool raw_isempty() const;
   void rawprepare(uint32_t tail);


 private:
   uint32_t tail_; //compress mode is enable, tail_ will replace streamdata.tail

};

}; //namespace socket

}; //namespace pf_net


#endif //PF_NET_SOCKET_OUTPUTSTREAM_H_
