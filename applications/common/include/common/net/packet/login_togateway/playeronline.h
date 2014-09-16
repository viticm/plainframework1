/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id PlayerOnline.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/11 20:22
 * @uses net packets login to gateway player leave class
 */
#ifndef COMMON_NET_PACKET_LOGIN_TOGATEWAY_PLAYERONLINE_H_
#define COMMON_NET_PACKET_LOGIN_TOGATEWAY_PLAYERONLINE_H_

#include "common/net/packet/login_togateway/config.h"
#include "pf/net/connection/base.h"
#include "pf/net/packet/base.h"
#include "pf/net/packet/factory.h"
#include "common/define/macros.h"

namespace common {

namespace net {

namespace packet {

namespace login_togateway {

class PlayerOnline : public pf_net::packet::Base {

 public:
   PlayerOnline();
   virtual ~PlayerOnline() {};

 public:  
   virtual bool read(pf_net::socket::InputStream &inputstream);
   virtual bool write(pf_net::socket::OutputStream &outputstream) const;
   virtual uint32_t execute(pf_net::connection::Base *connection);
   virtual uint16_t getid() const;
   virtual uint32_t getsize() const;
   
 public: 
   int16_t get_centerid() const;
   void set_centerid(int16_t centerid);
   uint16_t getonline() const;
   void setonline(uint16_t online);

 private:
   uint16_t online_; //在线数量
   int16_t centerid_; //中心ID

};

class PlayerOnlineFactory : public pf_net::packet::Factory {

 public:
   pf_net::packet::Base *createpacket();
   uint16_t get_packetid() const;
   uint32_t get_packet_maxsize() const;

};

class PlayerOnlineHandler {

 public:
   static uint32_t execute(PlayerOnline *packet, 
                           pf_net::connection::Base *connection);

};

}; //namespace login_togateway

}; //namespace packet

}; //namespace net

}; //namespace common

#endif //COMMON_NET_PACKET_LOGIN_TOGATEWAY_PLAYERONLINE_H_
