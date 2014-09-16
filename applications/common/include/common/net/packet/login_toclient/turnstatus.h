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
#ifndef COMMON_NET_PACKET_LOGIN_TOCLIENT_TURNSTATUS_H_
#define COMMON_NET_PACKET_LOGIN_TOCLIENT_TURNSTATUS_H_

#include "common/net/packet/login_toclient/config.h"
#include "pf/net/connection/base.h"
#include "pf/net/packet/base.h"
#include "pf/net/packet/factory.h"
#include "common/define/macros.h"

namespace common {

namespace net {

namespace packet {

namespace login_toclient {

class TurnStatus : public pf_net::packet::Base {

 public:
   TurnStatus();
   virtual ~TurnStatus() {};

 public:  
   virtual bool read(pf_net::socket::InputStream &inputstream);
   virtual bool write(pf_net::socket::OutputStream &outputstream) const;
   virtual uint32_t execute(pf_net::connection::Base *connection);
   virtual uint16_t getid() const;
   virtual uint32_t getsize() const;
   
 public: 
   uint16_t get_turnnumber() const;
   void set_turnnumber(uint16_t number);
   uint8_t get_turnstatus() const;
   void set_turnstatus(uint8_t status);

 private:
   uint16_t turnnumber_;
   uint8_t turnstatus_;

};

class TurnStatusFactory : public pf_net::packet::Factory {

 public:
   pf_net::packet::Base *createpacket();
   uint16_t get_packetid() const;
   uint32_t get_packet_maxsize() const;

};

class TurnStatusHandler {

 public:
   static uint32_t execute(TurnStatus *packet, 
                           pf_net::connection::Base *connection);

};

}; //namespace login_toclient

}; //namespace packet

}; //namespace net

}; //namespace common

#endif //COMMON_NET_PACKET_LOGIN_TOCLIENT_TURNSTATUS_H_
