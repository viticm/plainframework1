/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id playerleave.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/11 20:22
 * @uses net packet login to center require player login class
 */
#ifndef COMMON_NET_PACKET_LOGIN_TOCENTER_REQUIRE_PLAYERLOGIN_H_
#define COMMON_NET_PACKET_LOGIN_TOCENTER_REQUIRE_PLAYERLOGIN_H_

#include "common/net/packet/login_tocenter/config.h"
#include "pf/net/connection/base.h"
#include "pf/net/packet/base.h"
#include "pf/net/packet/factory.h"
#include "common/define/macros.h"

namespace common {

namespace net {

namespace packet {

namespace login_tocenter {

class RequirePlayerLogin : public pf_net::packet::Base {

 public:
   RequirePlayerLogin();
   virtual ~RequirePlayerLogin() {};

 public:  
   virtual bool read(pf_net::socket::InputStream &inputstream);
   virtual bool write(pf_net::socket::OutputStream &outputstream) const;
   virtual uint32_t execute(pf_net::connection::Base *connection);
   virtual uint16_t getid() const;
   virtual uint32_t getsize() const;
   
 public: 
   const char *getaccount();
   void setaccount(const char *account);
   int64_t getguid() const;
   void setguid(int64_t guid);
   int16_t getpid() const;
   void setpid(int16_t id);

 private:
   char account_[ACCOUNT_LENGTH_MAX];
   int64_t guid_; //玩家guid，唯一标示
   int16_t pid_; //连接ID

};

class RequirePlayerLoginFactory : public pf_net::packet::Factory {

 public:
   pf_net::packet::Base *createpacket();
   uint16_t get_packetid() const;
   uint32_t get_packet_maxsize() const;

};

class RequirePlayerLoginHandler {

 public:
   static uint32_t execute(RequirePlayerLogin *packet, 
                           pf_net::connection::Base *connection);

};

}; //namespace login_tocenter

}; //namespace packet

}; //namespace net

}; //namespace common

#endif //COMMON_NET_PACKET_LOGIN_TOGATEWAY_PLAYERLEAVE_H_
