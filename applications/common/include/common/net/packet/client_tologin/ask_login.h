/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id ask_login.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/09/10 20:10
 * @uses 请求登陆
 */
#ifndef COMMON_NET_PACKET_CLIENT_TOLOGIN_ASK_LOGIN_H_
#define COMMON_NET_PACKET_CLIENT_TOLOGIN_ASK_LOGIN_H_

#include "common/net/packet/client_tologin/config.h"
#include "pf/net/connection/base.h"
#include "pf/net/packet/base.h"
#include "pf/net/packet/factory.h"
#include "common/define/macros.h"

namespace common {

namespace net {

namespace packet {

namespace client_tologin {

class AskLogin : public pf_net::packet::Base {

 public:
   AskLogin();
   virtual ~AskLogin() {};

 public:  
   virtual bool read(pf_net::socket::InputStream &inputstream);
   virtual bool write(pf_net::socket::OutputStream &outputstream) const;
   virtual uint32_t execute(pf_net::connection::Base *connection);
   virtual uint16_t getid() const;
   virtual uint32_t getsize() const;
   
 public: 
   int8_t get_type() const;
   void set_type(int8_t type);
   int8_t get_typess() const;
   void set_typess(int8_t typess);
   int8_t get_types() const;
   void set_types(int8_t types);
   int8_t get_typesss() const;
   void set_typesss(int8_t typesss);
   const char *get_account();
   void set_account(const char *account);

 private:
   int8_t type_;
   int8_t typess_;
   int8_t types_;
   int8_t typesss_;
   char account_[64];

};

class AskLoginFactory : public pf_net::packet::Factory {

 public:
   pf_net::packet::Base *createpacket();
   uint16_t get_packetid() const;
   uint32_t get_packet_maxsize() const;

};

class AskLoginHandler {

 public:
   static uint32_t execute(AskLogin *packet, 
                           pf_net::connection::Base *connection);

};

}; //namespace client_tologin

}; //namespace packet

}; //namespace net

}; //namespace common

#endif //COMMON_NET_PACKET_CLIENT_TOLOGIN_ASK_LOGIN_H_
