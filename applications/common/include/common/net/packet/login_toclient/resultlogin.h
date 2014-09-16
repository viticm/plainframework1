/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id playerleave.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/11 20:22
 * @uses net packet login to center result for login class
 *       cn: 登陆结果消息
 */
#ifndef COMMON_NET_PACKET_LOGIN_TOCLIENT_RESULTLOGIN_H_
#define COMMON_NET_PACKET_LOGIN_TOCLIENT_RESULTLOGIN_H_

#include "common/net/packet/login_toclient/config.h"
#include "pf/net/connection/base.h"
#include "pf/net/packet/base.h"
#include "pf/net/packet/factory.h"
#include "common/define/macros.h"

namespace common {

namespace net {

namespace packet {

namespace login_toclient {

class ResultLogin : public pf_net::packet::Base {

 public:
   ResultLogin();
   virtual ~ResultLogin() {};

 public:  
   virtual bool read(pf_net::socket::InputStream &inputstream);
   virtual bool write(pf_net::socket::OutputStream &outputstream) const;
   virtual uint32_t execute(pf_net::connection::Base *connection);
   virtual uint16_t getid() const;
   virtual uint32_t getsize() const;
   
 public: 
   const char *getaccount();
   void setaccount(const char *account);
   const char *getpassword();
   void setpassword(const char *password);
   uint8_t getresult() const;
   void setresult(uint8_t result);

 private:
   char account_[ACCOUNT_LENGTH_MAX];
   char password_[PASSWORD_LENGTH_MAX];
   uint8_t result_;

};

class ResultLoginFactory : public pf_net::packet::Factory {

 public:
   pf_net::packet::Base *createpacket();
   uint16_t get_packetid() const;
   uint32_t get_packet_maxsize() const;

};

class ResultLoginHandler {

 public:
   static uint32_t execute(ResultLogin *packet, 
                           pf_net::connection::Base *connection);

};

}; //namespace login_toclient

}; //namespace packet

}; //namespace net

}; //namespace common

#endif //COMMON_NET_PACKET_LOGIN_TOCLIENT_RESULTLOGIN_H_
