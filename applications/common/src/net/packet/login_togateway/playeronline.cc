#include "pf/base/string.h"
#include "common/define/net/packet/id/gatewaylogin.h"
#include "common/net/packet/login_togateway/playeronline.h"

using namespace pf_net::socket;
using namespace common::net::packet::login_togateway;

PlayerOnline::PlayerOnline() {
  __ENTER_FUNCTION
    online_ = 0;
    centerid_ = ID_INVALID;
  __LEAVE_FUNCTION
}

bool PlayerOnline::read(InputStream& inputstream) {
  __ENTER_FUNCTION
    centerid_ = inputstream.read_int16();
    online_ = inputstream.read_uint16();
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool PlayerOnline::write(OutputStream& outputstream) const {
  __ENTER_FUNCTION
    outputstream.write_int16(centerid_);
    outputstream.write_uint16(online_);
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint32_t PlayerOnline::execute(
    pf_net::connection::Base* connection) {
  __ENTER_FUNCTION
    uint32_t result = 0;
    result = PlayerOnlineHandler::execute(this, connection);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint16_t PlayerOnline::getid() const {
  using namespace 
    common::define::net::packet::id::login_togateway; 
  return kPlayerOnline;
}

uint32_t PlayerOnline::getsize() const {
  uint32_t result = sizeof(centerid_) +
                    sizeof(online_);
  return result;
}


int16_t PlayerOnline::get_centerid() const {
  return centerid_;
}

void PlayerOnline::set_centerid(int16_t centerid) {
  centerid_ = centerid;
}

void PlayerOnline::setonline(uint16_t online) {
  online_ = online;
}

uint16_t PlayerOnline::getonline() const {
  return online_;
}

pf_net::packet::Base *PlayerOnlineFactory::createpacket() {
  __ENTER_FUNCTION
    pf_net::packet::Base *result = new PlayerOnline();
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

uint16_t PlayerOnlineFactory::get_packetid() const {
  using namespace 
    common::define::net::packet::id::login_togateway; 
  return kPlayerOnline;
}

uint32_t PlayerOnlineFactory::get_packet_maxsize() const {
  uint32_t result = sizeof(int16_t) +
                    sizeof(uint16_t);
  return result;
}
