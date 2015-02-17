/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id encryptor.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/viticm.ti@gmail.com
 * @date 2015/01/25 22:06
 * @uses encryptor of net socket stream
*/
#ifndef PF_NET_SOCKET_ENCRYPTOR_H_
#define PF_NET_SOCKET_ENCRYPTOR_H_

#include "pf/net/socket/config.h"

namespace pf_net {

namespace socket {

class PF_API Encryptor {

 public:
   Encryptor();
   ~Encryptor();

 public:
   enum { kKeyLength = 16, };

 public:
   void *encrypt(void *out, const void *in, uint32_t count);
   void *decrypt(void *out, const void *in, uint32_t count);

 public:
   void setkey(const char *key);
   const char *getkey();
   void enable(bool enable);
   bool isenable() const;

 private:
   char key_[kKeyLength];
   bool isenable_;

};

}; //namespace socket

}; //namespace pf_net

#endif //PF_NET_SOCKET_ENCRYPTOR_H_
