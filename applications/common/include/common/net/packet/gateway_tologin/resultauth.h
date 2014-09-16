/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id resultauth.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/10 19:11
 * @uses net packets module gateway to login result auth packet class
 */
#ifndef COMMON_NET_PACKET_GATEWAY_TOLOGIN_RESULTAUTH_H_
#define COMMON_NET_PACKET_GATEWAY_TOLOGIN_RESULTAUTH_H_

#include "common/net/packet/gateway_tologin/config.h"
#include "pf/net/connection/base.h"
#include "pf/net/packet/base.h"
#include "pf/net/packet/factory.h"
#include "common/define/macros.h"

namespace common {

namespace net {

namespace packet {

namespace gateway_tologin {

class ResultAuth : public pf_net::packet::Base {

 public:
   ResultAuth();
   virtual ~ResultAuth() {};

 public:  
   virtual bool read(pf_net::socket::InputStream& inputstream);
   virtual bool write(pf_net::socket::OutputStream& outputstream) const;
   virtual uint32_t execute(pf_net::connection::Base *connection);
   virtual uint16_t getid() const;
   virtual uint32_t getsize() const;
   
 public: 
   const char *getaccount();
   void setaccount(const char *account);
   const char *getpassword();
   void setpassword(const char *password);
   void setresult(uint8_t result);
   uint8_t getresult() const;

 private:
   char account_[ACCOUNT_LENGTH_MAX];
   char password_[PASSWORD_LENGTH_MAX];
   uint8_t result_;

};

class ResultAuthFactory : public pf_net::packet::Factory {

 public:
   pf_net::packet::Base *createpacket();
   uint16_t get_packetid() const;
   uint32_t get_packet_maxsize() const;

};

class ResultAuthHandler {

 public:
   static uint32_t execute(ResultAuth *packet, 
                           pf_net::connection::Base *connection);

};

}; //namespace gateway_tologin

}; //namespace packet

}; //namespace net

}; //namespace common

#endif //COMMON_NET_PACKET_GATEWAY_TOLOGIN_RESULTAUTH_H_
