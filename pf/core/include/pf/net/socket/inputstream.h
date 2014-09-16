/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id inputstream.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/20 11:45
 * @uses socket inputstream class
 */
#ifndef PF_NET_SOCKET_INPUTSTREAM_H_
#define PF_NET_SOCKET_INPUTSTREAM_H_

#include "pf/net/packet/base.h"
#include "pf/net/socket/stream.h"

namespace pf_net {

namespace socket {

class PF_API InputStream : public Stream {

 public: //construct and destruct
   InputStream(
       Base* socket, 
       uint32_t bufferlength = SOCKETINPUT_BUFFERSIZE_DEFAULT, 
       uint32_t bufferlength_max = SOCKETINPUT_DISCONNECT_MAXSIZE)
     : Stream(socket, bufferlength, bufferlength_max) {};
   virtual ~InputStream() {};
   
 public:
   uint32_t read(char *buffer, uint32_t length);
   bool readpacket(packet::Base *packet);
   bool peek(char *buffer, uint32_t length);
   bool skip(uint32_t length);
   int32_t fill();

 public:
   int8_t read_int8();
   uint8_t read_uint8();
   int16_t read_int16();
   uint16_t read_uint16();
   int32_t read_int32();
   uint32_t read_uint32();
   int64_t read_int64();
   uint64_t read_uint64();
   void read_string(char *buffer, size_t size);
   float read_float();
   double read_double();

};

}; //namespace socket

}; //namespace pf_net

#endif //PF_NET_SOCKET_INPUTSTREAM_H_
