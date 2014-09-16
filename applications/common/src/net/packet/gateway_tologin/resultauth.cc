#include "pf/base/string.h"
#include "common/define/net/packet/id/gatewaylogin.h"
#include "common/net/packet/gateway_tologin/resultauth.h"

using namespace pf_net::socket;
using namespace common::net::packet::gateway_tologin;

ResultAuth::ResultAuth() {
  __ENTER_FUNCTION
    result_ = 0;
    memset(account_, 0, sizeof(account_));
    memset(password_, 0, sizeof(password_));
  __LEAVE_FUNCTION
}

bool ResultAuth::read(InputStream& inputstream) {
  __ENTER_FUNCTION
    inputstream.read_string(account_, sizeof(account_));
    inputstream.read_string(password_, sizeof(password_));
    inputstream.read((char*)&result_, sizeof(result_));
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ResultAuth::write(OutputStream& outputstream) const {
  __ENTER_FUNCTION
    outputstream.write_string(account_);
    outputstream.write_string(password_);
    outputstream.write((char*)&result_, sizeof(result_));
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint32_t ResultAuth::execute(
    pf_net::connection::Base* connection) {
  __ENTER_FUNCTION
    uint32_t result = 0;
    result = ResultAuthHandler::execute(this, connection);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint16_t ResultAuth::getid() const {
  using namespace 
    common::define::net::packet::id::gateway_tologin;
  return kResultAuth;
}

uint32_t ResultAuth::getsize() const {
  uint32_t result = strlen(account_) +
                    strlen(password_) +
                    sizeof(result_);
  return result;
}

const char *ResultAuth::getaccount() {
  return account_;
}

void ResultAuth::setaccount(const char *account) {
  __ENTER_FUNCTION
    using namespace pf_base;
    string::safecopy(account_, account, sizeof(account_));
  __LEAVE_FUNCTION
}

const char *ResultAuth::getpassword() {
  return password_;
}

void ResultAuth::setpassword(const char *password) {
  __ENTER_FUNCTION
    using namespace pf_base;
    string::safecopy(password_, password, sizeof(password_));
  __LEAVE_FUNCTION
}

uint8_t ResultAuth::getresult() const {
  return result_;
}

void ResultAuth::setresult(uint8_t result) {
  result_ = result;
}

pf_net::packet::Base *ResultAuthFactory::createpacket() {
  __ENTER_FUNCTION
    pf_net::packet::Base *result = new ResultAuth();
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

uint16_t ResultAuthFactory::get_packetid() const {
  using namespace 
    common::define::net::packet::id::gateway_tologin;
  return kResultAuth;
}

uint32_t ResultAuthFactory::get_packet_maxsize() const {
  uint32_t result = sizeof(char) * ACCOUNT_LENGTH_MAX +
                    sizeof(char) * PASSWORD_LENGTH_MAX +
                    sizeof(uint8_t);
  return result;
}
