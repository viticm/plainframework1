#include "pf/base/string.h"
#include "pf/net/socket/encryptor.h"

using namespace pf_net::socket;

Encryptor::Encryptor() {
  __ENTER_FUNCTION
    isenable_ = false;
    memset(key_, 0, sizeof(key_));
  __LEAVE_FUNCTION
}

Encryptor::~Encryptor() {
  //do nothing
}

void *Encryptor::encrypt(void *out, const void *in, uint32_t count) {
  __ENTER_FUNCTION
    void *result = out;
    while (count--) {
      *reinterpret_cast<char *>(out) = 
        *reinterpret_cast<char *>(const_cast<void *>(in));
      if (isenable()) { //enable with key
        uint8_t low = 0;
        uint8_t high = 0;
        low = (*reinterpret_cast<char *>(out)) & 0x0F;
        high = (*reinterpret_cast<char *>(out)) & 0xF0;
        high = high ^ (key_[low] & 0xF0);
        low = (((low ^ 0x0F) & 0x0F) + (key_[0] & 0x0F)) & 0x0F;
        *reinterpret_cast<char *>(out) = high + low;
      } else {
        *reinterpret_cast<char *>(out) = *reinterpret_cast<char *>(out) ^ 0xFF;
      }
      out = reinterpret_cast<char *>(out) + 1;
      in = reinterpret_cast<char *>(const_cast<void *>(in)) + 1;
    }
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

void *Encryptor::decrypt(void *out, const void *in, uint32_t count) {
  __ENTER_FUNCTION
    void *result = out;
    while (count--) {
      *reinterpret_cast<char *>(out) = 
        *reinterpret_cast<char *>(const_cast<void *>(in));
      if (isenable()) { //enable with key
        uint8_t low = 0;
        uint8_t high = 0;
        low = (*reinterpret_cast<char *>(out)) & 0x0F;
        high = (*reinterpret_cast<char *>(out)) & 0xF0;
        low = ((low - (key_[0] & 0x0F)) & 0x0F) ^ 0x0F;
        high = high ^ (key_[low] & 0xF0);
        *reinterpret_cast<char *>(out) = high + low;
      } else {
        *reinterpret_cast<char *>(out) = *reinterpret_cast<char *>(out) ^ 0xFF;
      }
      out = reinterpret_cast<char *>(out) + 1;
      in = reinterpret_cast<char *>(const_cast<void *>(in)) + 1;
    }
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

void Encryptor::setkey(const char *key) {
  __ENTER_FUNCTION
    pf_base::string::safecopy(key_, key, sizeof(key_));
  __LEAVE_FUNCTION
}

const char *Encryptor::getkey() {
  return key_;
}

bool Encryptor::isenable() const {
  return isenable_;
}

void Encryptor::enable(bool enable) {
  isenable_ = enable;
}
