#include "pf/base/string.h"
#include "common/define/net/packet/id/gatewaylogin.h"
#include "common/net/packet/login_togateway/playerleave.h"

using namespace pf_net::socket;
using namespace common::net::packet::login_togateway;

PlayerLeave::PlayerLeave() {
  __ENTER_FUNCTION
    memset(account_, 0, sizeof(account_));
  __LEAVE_FUNCTION
}

bool PlayerLeave::read(InputStream& inputstream) {
  __ENTER_FUNCTION
    inputstream.read_string(account_, sizeof(account_));
    guid_ = inputstream.read_int64();
    centerid_ = inputstream.read_int16();
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool PlayerLeave::write(OutputStream& outputstream) const {
  __ENTER_FUNCTION
    outputstream.write_string(account_);
    outputstream.write_int64(guid_);
    outputstream.write_int16(centerid_);
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint32_t PlayerLeave::execute(
    pf_net::connection::Base* connection) {
  __ENTER_FUNCTION
    uint32_t result = 0;
    result = PlayerLeaveHandler::execute(this, connection);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint16_t PlayerLeave::getid() const {
  using namespace 
    common::define::net::packet::id::login_togateway; 
  return kPlayerLeave;
}

uint32_t PlayerLeave::getsize() const {
  uint32_t result = strlen(account_) +
                    sizeof(guid_) +
                    sizeof(centerid_);
  return result;
}

const char *PlayerLeave::getaccount() {
  return account_;
}

void PlayerLeave::setaccount(const char *account) {
  __ENTER_FUNCTION
    using namespace pf_base;
    string::safecopy(account_, account, sizeof(account_));
  __LEAVE_FUNCTION
}

void PlayerLeave::setguid(int64_t guid) {
  guid_ = guid;
}

int64_t PlayerLeave::getguid() const {
  return guid_;
}

int16_t PlayerLeave::get_centerid() const {
  return centerid_;
}

void PlayerLeave::set_centerid(int16_t centerid) {
  centerid_ = centerid;
}

pf_net::packet::Base *PlayerLeaveFactory::createpacket() {
  __ENTER_FUNCTION
    pf_net::packet::Base *result = new PlayerLeave();
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

uint16_t PlayerLeaveFactory::get_packetid() const {
  using namespace 
    common::define::net::packet::id::login_togateway; 
  return kPlayerLeave;
}

uint32_t PlayerLeaveFactory::get_packet_maxsize() const {
  uint32_t result = sizeof(char) * ACCOUNT_LENGTH_MAX +
                    sizeof(int64_t) +
                    sizeof(int16_t);
  return result;
}
