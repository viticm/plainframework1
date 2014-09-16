#include "pf/base/string.h"
#include "common/define/net/packet/id/logincenter.h"
#include "common/net/packet/login_tocenter/require_playerlogin.h"

using namespace pf_net::socket;
using namespace common::net::packet::login_tocenter;

RequirePlayerLogin::RequirePlayerLogin() {
  __ENTER_FUNCTION
    memset(account_, 0, sizeof(account_));
  __LEAVE_FUNCTION
}

bool RequirePlayerLogin::read(InputStream& inputstream) {
  __ENTER_FUNCTION
    inputstream.read_string(account_, sizeof(account_));
    guid_ = inputstream.read_int64();
    pid_ = inputstream.read_int16();
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool RequirePlayerLogin::write(OutputStream& outputstream) const {
  __ENTER_FUNCTION
    outputstream.write_string(account_);
    outputstream.write_int64(guid_);
    outputstream.write_int16(pid_);
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint32_t RequirePlayerLogin::execute(
    pf_net::connection::Base* connection) {
  __ENTER_FUNCTION
    uint32_t result = 0;
    result = RequirePlayerLoginHandler::execute(this, connection);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint16_t RequirePlayerLogin::getid() const {
  using namespace 
    common::define::net::packet::id::login_tocenter; 
  return kRequirePlayerLogin;
}

uint32_t RequirePlayerLogin::getsize() const {
  uint32_t result = strlen(account_) +
                    sizeof(guid_) +
                    sizeof(pid_);
  return result;
}

const char *RequirePlayerLogin::getaccount() {
  return account_;
}

void RequirePlayerLogin::setaccount(const char *account) {
  __ENTER_FUNCTION
    using namespace pf_base;
    string::safecopy(account_, account, sizeof(account_));
  __LEAVE_FUNCTION
}

void RequirePlayerLogin::setguid(int64_t guid) {
  guid_ = guid;
}

int64_t RequirePlayerLogin::getguid() const {
  return guid_;
}

int16_t RequirePlayerLogin::getpid() const {
  return pid_;
}

void RequirePlayerLogin::setpid(int16_t id) {
  pid_ = id;
}

pf_net::packet::Base *RequirePlayerLoginFactory::createpacket() {
  __ENTER_FUNCTION
    pf_net::packet::Base *result = new RequirePlayerLogin();
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

uint16_t RequirePlayerLoginFactory::get_packetid() const {
  using namespace 
    common::define::net::packet::id::login_tocenter; 
  return kRequirePlayerLogin;
}

uint32_t RequirePlayerLoginFactory::get_packet_maxsize() const {
  uint32_t result = sizeof(char) * ACCOUNT_LENGTH_MAX +
                    sizeof(int64_t) +
                    sizeof(int16_t);
  return result;
}
