/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id connect.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/23 10:58
 * @uses server connect server packet class
 */
#ifndef COMMON_NET_PACKET_SERVERSERVER_CONNECT_H_
#define COMMON_NET_PACKET_SERVERSERVER_CONNECT_H_

#include "common/net/packet/serverserver/config.h"
#include "pf/net/connection/base.h"
#include "pf/net/packet/base.h"
#include "pf/net/packet/factory.h"

namespace common {

namespace net {

namespace packet {

namespace serverserver {

class Connect : public pf_net::packet::Base {

 public:
   Connect();
   virtual ~Connect() {};

 public:  
   virtual bool read(pf_net::socket::InputStream& inputstream);
   virtual bool write(pf_net::socket::OutputStream& outputstream) const;
   virtual uint32_t execute(pf_net::connection::Base* connection);
   virtual uint16_t getid() const;
   virtual uint32_t getsize() const;
   
 public: 
   int16_t get_serverid();
   void set_serverid(int16_t serverid);
   int16_t get_centerid();
   void set_centerid(int16_t centerid);
   int16_t get_zoneid();
   void set_zoneid(int16_t zoneid);

 private:
   int16_t serverid_; //服务器ID
   int16_t centerid_; //中心服务器ID
   int16_t zoneid_; //区域ID

};

class ConnectFactory : public pf_net::packet::Factory {

 public:
   pf_net::packet::Base* createpacket();
   uint16_t get_packetid() const;
   uint32_t get_packet_maxsize() const;

};

class ConnectHandler {

 public:
   static uint32_t execute(Connect* packet, 
                           pf_net::connection::Base* connection);

};

}; //namespace serverserver

}; //namespace packet

}; //namespace net 

}; //namespace common

#endif //COMMON_NET_PACKET_SERVERSERVER_CONNECT_H_
