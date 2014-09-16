#include "pf/base/string.h"
#include "common/define/net/packet/id/clientlogin.h"
#include "common/net/packet/login_toclient/resultlogin.h"

using namespace pf_net::socket;
using namespace common::net::packet::login_toclient;

ResultLogin::ResultLogin() {
  __ENTER_FUNCTION
    result_ = 0;
    memset(account_, 0, sizeof(account_));
    memset(password_, 0, sizeof(password_));
  __LEAVE_FUNCTION
}

bool ResultLogin::read(InputStream& inputstream) {
  __ENTER_FUNCTION
    inputstream.read(account_, sizeof(account_));
    inputstream.read(password_, sizeof(password_));
    inputstream.read((char*)&result_, sizeof(result_));
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ResultLogin::write(OutputStream& outputstream) const {
  __ENTER_FUNCTION
    outputstream.write(account_, sizeof(account_));
    outputstream.write(password_, sizeof(password_));
    outputstream.write((char*)&result_, sizeof(result_));
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint32_t ResultLogin::execute(
    pf_net::connection::Base* connection) {
  __ENTER_FUNCTION
    uint32_t result = 0;
    result = ResultLoginHandler::execute(this, connection);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint16_t ResultLogin::getid() const {
  using namespace 
    common::define::net::packet::id::login_toclient;
  return kResultLogin;
}

uint32_t ResultLogin::getsize() const {
  uint32_t result = strlen(account_) +
                    strlen(password_) +
                    sizeof(result_);
  return result;
}

const char *ResultLogin::getaccount() {
  return account_;
}

void ResultLogin::setaccount(const char *account) {
  __ENTER_FUNCTION
    using namespace pf_base;
    string::safecopy(account_, account, sizeof(account_));
  __LEAVE_FUNCTION
}

const char *ResultLogin::getpassword() {
  return password_;
}

void ResultLogin::setpassword(const char *password) {
  __ENTER_FUNCTION
    using namespace pf_base;
    string::safecopy(password_, password, sizeof(password_));
  __LEAVE_FUNCTION
}

uint8_t ResultLogin::getresult() const {
  return result_;
}

void ResultLogin::setresult(uint8_t result) {
  result_ = result;
}

pf_net::packet::Base *ResultLoginFactory::createpacket() {
  __ENTER_FUNCTION
    pf_net::packet::Base *result = new ResultLogin();
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

uint16_t ResultLoginFactory::get_packetid() const {
  using namespace 
    common::define::net::packet::id::login_toclient;
  return kResultLogin;
}

uint32_t ResultLoginFactory::get_packet_maxsize() const {
  uint32_t result = sizeof(char) * ACCOUNT_LENGTH_MAX +
                    sizeof(char) * PASSWORD_LENGTH_MAX +
                    sizeof(uint8_t);
  return result;
}
