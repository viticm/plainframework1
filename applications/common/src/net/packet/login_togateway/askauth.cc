#include "pf/base/string.h"
#include "common/define/net/packet/id/gatewaylogin.h"
#include "common/net/packet/login_togateway/askauth.h"

using namespace pf_net::socket;
using namespace common::net::packet::login_togateway;

AskAuth::AskAuth() {
  __ENTER_FUNCTION
    memset(account_, 0, sizeof(account_));
    memset(password_, 0, sizeof(password_));
  __LEAVE_FUNCTION
}

bool AskAuth::read(InputStream& inputstream) {
  __ENTER_FUNCTION
    inputstream.read(account_, sizeof(account_));
    inputstream.read(password_, sizeof(password_));
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool AskAuth::write(OutputStream& outputstream) const {
  __ENTER_FUNCTION
    outputstream.write(account_, sizeof(account_));
    outputstream.write(password_, sizeof(password_));
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint32_t AskAuth::execute(
    pf_net::connection::Base* connection) {
  __ENTER_FUNCTION
    uint32_t result = 0;
    result = AskAuthHandler::execute(this, connection);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint16_t AskAuth::getid() const {
  using namespace 
    common::define::net::packet::id::login_togateway; 
  return kAskAuth;
}

uint32_t AskAuth::getsize() const {
  uint32_t result = strlen(account_) +
                    strlen(password_);
  return result;
}

const char *AskAuth::getaccount() {
  return account_;
}

void AskAuth::setaccount(const char *account) {
  __ENTER_FUNCTION
    using namespace pf_base;
    string::safecopy(account_, account, sizeof(account_));
  __LEAVE_FUNCTION
}

const char *AskAuth::getpassword() {
  return password_;
}

void AskAuth::setpassword(const char *password) {
  __ENTER_FUNCTION
    using namespace pf_base;
    string::safecopy(password_, password, sizeof(password_));
  __LEAVE_FUNCTION
}

pf_net::packet::Base *AskAuthFactory::createpacket() {
  __ENTER_FUNCTION
    pf_net::packet::Base *result = new AskAuth();
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

uint16_t AskAuthFactory::get_packetid() const {
  using namespace 
    common::define::net::packet::id::login_togateway; 
  return kAskAuth;
}

uint32_t AskAuthFactory::get_packet_maxsize() const {
  uint32_t result = sizeof(char) * ACCOUNT_LENGTH_MAX +
                    sizeof(char) * PASSWORD_LENGTH_MAX;
  return result;
}
