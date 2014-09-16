#include "pf/base/string.h"
#include "common/define/net/packet/id/clientlogin.h"
#include "common/define/enum.h"
#include "common/net/packet/login_toclient/turnstatus.h"

using namespace pf_net::socket;
using namespace common::net::packet::login_toclient;

TurnStatus::TurnStatus() {
  __ENTER_FUNCTION
    //do nothing
  __LEAVE_FUNCTION
}

bool TurnStatus::read(InputStream& inputstream) {
  __ENTER_FUNCTION
    turnnumber_ = inputstream.read_uint16();
    turnstatus_ = inputstream.read_uint8();
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool TurnStatus::write(OutputStream& outputstream) const {
  __ENTER_FUNCTION
    outputstream.write_uint16(turnnumber_);
    outputstream.write_uint8(turnstatus_);
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint32_t TurnStatus::execute(
    pf_net::connection::Base* connection) {
  __ENTER_FUNCTION
    uint32_t result = 0;
    result = TurnStatusHandler::execute(this, connection);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint16_t TurnStatus::getid() const {
  using namespace 
    common::define::net::packet::id::login_toclient; 
  return kTurnStatus;
}

uint32_t TurnStatus::getsize() const {
  uint32_t result = sizeof(turnnumber_) +
                    sizeof(turnstatus_);
  return result;
}

uint16_t TurnStatus::get_turnnumber() const {
  return turnnumber_;
}

void TurnStatus::set_turnnumber(uint16_t number) {
  turnnumber_ = number;
}

uint8_t TurnStatus::get_turnstatus() const {
  return turnstatus_;
}

void TurnStatus::set_turnstatus(uint8_t status) {
  turnstatus_ = status;
}

pf_net::packet::Base *TurnStatusFactory::createpacket() {
  __ENTER_FUNCTION
    pf_net::packet::Base *result = new TurnStatus();
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

uint16_t TurnStatusFactory::get_packetid() const {
  using namespace 
    common::define::net::packet::id::login_toclient; 
  return kTurnStatus;
}

uint32_t TurnStatusFactory::get_packet_maxsize() const {
  uint32_t result = sizeof(uint16_t) +
                    sizeof(uint8_t);
  return result;
}
