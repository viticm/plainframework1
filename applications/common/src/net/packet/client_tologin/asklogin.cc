#include "pf/base/string.h"
#include "common/define/net/packet/id/clientlogin.h"
#include "common/define/enum.h"
#include "common/net/packet/client_tologin/asklogin.h"

using namespace pf_net::socket;
using namespace common::net::packet::client_tologin;

AskLogin::AskLogin() {
  __ENTER_FUNCTION
    //do nothing
  __LEAVE_FUNCTION
}

bool AskLogin::read(InputStream& inputstream) {
  __ENTER_FUNCTION
    inputstream.read_string(account_, sizeof(account_));
    inputstream.read_string(password_, sizeof(password_));
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool AskLogin::write(OutputStream& outputstream) const {
  __ENTER_FUNCTION
    outputstream.write_string(account_);
    outputstream.write_string(password_);
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint32_t AskLogin::execute(
    pf_net::connection::Base* connection) {
  __ENTER_FUNCTION
    uint32_t result = 0;
    result = AskLoginHandler::execute(this, connection);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint16_t AskLogin::getid() const {
  using namespace 
    common::define::net::packet::id::client_tologin; 
  return kAskLogin;
}

uint32_t AskLogin::getsize() const {
  uint32_t result = strlen(account_) +
                    strlen(password_);
  return result;
}

const char *AskLogin::getaccount() {
  return account_;
}
   
void AskLogin::setaccount(const char *account) {
  pf_base::string::safecopy(account_, account, sizeof(account_));
}
   
const char *AskLogin::getpassword() {
  return password_;
}
   
void AskLogin::setpassword(const char *password) {
  pf_base::string::safecopy(password_, password, sizeof(password_));
}

pf_net::packet::Base *AskLoginFactory::createpacket() {
  __ENTER_FUNCTION
    pf_net::packet::Base *result = new AskLogin();
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

uint16_t AskLoginFactory::get_packetid() const {
  using namespace 
    common::define::net::packet::id::client_tologin; 
  return kAskLogin;
}

uint32_t AskLoginFactory::get_packet_maxsize() const {
  uint32_t result = sizeof(uint16_t) +
                    sizeof(uint8_t);
  return result;
}
