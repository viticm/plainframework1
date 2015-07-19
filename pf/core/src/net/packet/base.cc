#include "pf/net/packet/factorymanager.h"
#include "pf/net/packet/base.h"

namespace pf_net {

namespace packet {

queue_struct::queue_struct() {
  __ENTER_FUNCTION
    packet = NULL;
    connectionid = static_cast<uint16_t>(ID_INVALID);
    flag = kPacketFlagNone;
  __LEAVE_FUNCTION
}
  
queue_struct::~queue_struct() {
  __ENTER_FUNCTION
    SAFE_DELETE(packet);
  __LEAVE_FUNCTION
}

Base::Base() {
  //do nothing
}

Base::~Base() {
  //do nothing
}

int8_t Base::getindex() const {
  return index_;
}

void Base::setindex(int8_t index) {
  index_ = index;
}

uint8_t Base::getstatus() const {
  return status_;
}

void Base::setstatus(uint8_t status) {
  status_ = status;
}

void Base::setid(uint16_t) {
  //do nothing
}

void Base::setsize(uint32_t) {
  //do nothing
}

uint32_t Base::execute(connection::Base *connection) {
  __ENTER_FUNCTION
    uint32_t result = kPacketExecuteStatusError;
    if (!NET_PACKET_FACTORYMANAGER_POINTER) return result;
    result = NET_PACKET_FACTORYMANAGER_POINTER->packetexecute(connection, this);
    return result;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}



} //namespace packet

} //namespace pf_net
