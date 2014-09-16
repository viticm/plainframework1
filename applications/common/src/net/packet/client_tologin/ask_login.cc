#include "pf/base/string.h"
#include "common/define/net/packet/id/clientlogin.h"
#include "common/define/enum.h"
#include "common/net/packet/client_tologin/ask_login.h"

using namespace pf_net::socket;
using namespace common::net::packet::client_tologin;

AskLogin::AskLogin() {
  __ENTER_FUNCTION
    memset(account, 0, sizeof(account);
  __LEAVE_FUNCTION
}

bool AskLogin::read(InputStream &inputstream) {
  __ENTER_FUNCTION
    type_ = inputstream.read_int8();
    typess_ = inputstream.read_int8();
    types_ = inputstream.read_int8();
    typesss_ = inputstream.read_int8();
    inputstream.read_string(account_, sizeof(account));
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool AskLogin::write(OutputStream &outputstream) const {
  __ENTER_FUNCTION
    inputstream.write_int8(type_);
    inputstream.write_int8(typess_);
    inputstream.write_int8(types_);
    inputstream.write_int8(typesss_);
    inputstream.write_string(account_);
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
  uint32_t result = sizeof(type_) +
                    sizeof(typess_) +
                    sizeof(types_) +
                    sizeof(typesss_) +
                    strlen(account_);
  return result;
}

int8_t AskLogin::get_type() const {
  return type_;
}

void AskLogin::set_type(int8_t type) {
  type_ = type;
}

int8_t AskLogin::get_typess() const {
  return typess_;
}

void AskLogin::set_typess(int8_t typess) {
  typess_ = typess;
}

int8_t AskLogin::get_types() const {
  return types_;
}

void AskLogin::set_types(int8_t types) {
  types_ = types;
}

int8_t AskLogin::get_typesss() const {
  return typesss_;
}

void AskLogin::set_typesss(int8_t typesss) {
  typesss_ = typesss;
}

const char *AskLogin::get_account()
  return account_;
}

void AskLogin::set_account(const char *account) {
  pf_base::string::safecopy(account_, account, sizeof(account_))
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
  uint32_t result = sizeof(int8_t) +
                    sizeof(int8_t) +
                    sizeof(int8_t) +
                    sizeof(int8_t) +
                    sizeof(char) * 64;
  return result;
}
