/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id auth.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/10 16:31
 * @uses net packets login to gateway auth class
 */
#ifndef COMMON_NET_PACKET_LOGIN_TOGATEWAY_ASKAUTH_H_
#define COMMON_NET_PACKET_LOGIN_TOGATEWAY_ASKAUTH_H_

#include "common/net/packet/login_togateway/config.h"
#include "pf/net/connection/base.h"
#include "pf/net/packet/base.h"
#include "pf/net/packet/factory.h"
#include "common/define/macros.h"

namespace common {

namespace net {

namespace packet {

namespace login_togateway {

class AskAuth : public pf_net::packet::Base {

 public:
   AskAuth();
   virtual ~AskAuth() {};

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

 private:
   char account_[ACCOUNT_LENGTH_MAX];
   char password_[PASSWORD_LENGTH_MAX];

};

class AskAuthFactory : public pf_net::packet::Factory {

 public:
   pf_net::packet::Base *createpacket();
   uint16_t get_packetid() const;
   uint32_t get_packet_maxsize() const;

};

class AskAuthHandler {

 public:
   static uint32_t execute(AskAuth *packet, 
                           pf_net::connection::Base *connection);

};

}; //namespace login_togateway

}; //namespace packet

}; //namespace net

}; //namespace common

#endif //COMMON_NET_PACKET_LOGIN_TOGATEWAY_ASKAUTH_H_
