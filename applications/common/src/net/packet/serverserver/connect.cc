#include "common/define/net/packet/id/serverserver.h"
#include "common/net/packet/serverserver/connect.h"

using namespace pf_net;
using namespace common::net::packet::serverserver;

Connect::Connect() {
  __ENTER_FUNCTION
  __LEAVE_FUNCTION
}

bool Connect::read(socket::InputStream& inputstream) {
  __ENTER_FUNCTION
    inputstream.read((char*)(&serverid_), sizeof(serverid_));
    inputstream.read((char*)(&centerid_), sizeof(centerid_));
    inputstream.read((char*)(&zoneid_), sizeof(zoneid_));
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Connect::write(socket::OutputStream& outputstream) const {
  __ENTER_FUNCTION
    outputstream.write((char*)(&serverid_), sizeof(serverid_));
    outputstream.write((char*)(&centerid_), sizeof(centerid_));
    outputstream.write((char*)(&zoneid_), sizeof(zoneid_));
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint32_t Connect::execute(connection::Base* connection) {
  __ENTER_FUNCTION
    uint32_t result = 0;
    result = ConnectHandler::execute(this, connection);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint16_t Connect::getid() const {
  using namespace 
    common::define::net::packet::id::serverserver;
  return kConnect;
}

uint32_t Connect::getsize() const {
  uint32_t result = sizeof(serverid_) +
                    sizeof(centerid_) +
                    sizeof(zoneid_);
  return result;
}

int16_t Connect::get_serverid() {
  return serverid_;
}
void Connect::set_serverid(int16_t serverid) {
  serverid_ = serverid;
}
int16_t Connect::get_centerid() {
  return centerid_;
}
void Connect::set_centerid(int16_t centerid) {
  centerid_ = centerid;
}
int16_t Connect::get_zoneid() {
  return zoneid_;
}
void Connect::set_zoneid(int16_t zoneid) {
  zoneid_ = zoneid;
}

packet::Base* ConnectFactory::createpacket() {
  __ENTER_FUNCTION
    return new Connect();
  __LEAVE_FUNCTION
    return NULL;
}

uint16_t ConnectFactory::get_packetid() const {
  using namespace 
    common::define::net::packet::id::serverserver;
  return kConnect;
}

uint32_t ConnectFactory::get_packet_maxsize() const {
  uint32_t result = sizeof(int16_t) +
                    sizeof(int16_t) +
                    sizeof(int16_t);
  return result;
}
