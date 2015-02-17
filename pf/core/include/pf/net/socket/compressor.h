/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id compressor.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/viticm.ti@gmail.com
 * @date 2015/02/15 17:09
 * @uses the compressor of net socket stream
 *       This object instance just use compress or decompress in one object.
 *       cn:
 *       在一个实例中接口compress和decompress只能使用一个，如果两个要同时使用，
 *       需重写该类实现。
*/
#ifndef PF_NET_SOCKET_COMPRESSOR_H_
#define PF_NET_SOCKET_COMPRESSOR_H_

#include "pf/net/socket/config.h"
#include "pf/net/socket/encryptor.h"
#include "pf/util/compressor/assistant.h"

#define NET_SOCKET_COMPRESSOR_HEADER_SIZE 2
#define NET_SOCKET_COMPRESSOR_IN_SIZE (1024 * 20)
#define NET_SOCKET_COMPRESSOR_OUT_SIZE \
  (NET_SOCKET_COMPRESSOR_IN_SIZE + \
   NET_SOCKET_COMPRESSOR_IN_SIZE/16 + 64 + 3 + \
   NET_SOCKET_COMPRESSOR_HEADER_SIZE)
#define NET_SOCKET_COMPRESSOR_SIZE_MIN 100

namespace pf_net {

namespace socket {

class PF_API Compressor {

 public:
   Compressor();
   ~Compressor();

 public:
   bool compress(const char *in, uint32_t insize, char *out, uint32_t &outsize);
   bool decompress(const char *in, uint32_t insize, char *out, uint32_t &outsize);

 public:
   void clear();
   bool alloc(uint32_t size);
   char *getbuffer();
   char *getheader();
   uint32_t getsize() const;
   uint32_t get_maxsize() const;
   uint32_t get_freesize() const;

 public:
   bool pushback(uint32_t size);
   bool pophead(uint32_t size);
   void sethead(uint32_t head);
   void settail(uint32_t tail);
   void add_packetheader();
   void encrypt();
   void resetposition();
   void setencryptor(Encryptor *encryptor);
   pf_util::compressor::Assistant *getassistant();

 private:
   char *buffer_;
   uint32_t head_;
   uint32_t tail_;
   uint32_t maxsize_;
   Encryptor *encryptor_;
   pf_util::compressor::Assistant assistant_;

};

}; //namespace socket

}; //namespace pf_net

#endif //PF_NET_SOCKET_COMPRESSOR_H_
