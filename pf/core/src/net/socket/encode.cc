#include "pf/net/socket/encode.h"

namespace pf_net {

namespace socket {

namespace encode {

bool make(encodeparam_t &encodeparam) {
  unsigned char const *in;
  uint32_t insize;
  unsigned char *out;
  uint32_t outsize;
  unsigned char const *key;
  uint32_t keysize;
  int32_t keyindex;
  int32_t index;
  in = encodeparam.in;
  if(NULL == in) {
    return false;
  }
  insize = encodeparam.insize;
  if(insize <= 0) {
    return false;
  }
  out = encodeparam.out;
  if(NULL == out) {
    return false;
  }
  outsize = encodeparam.outsize;
  if(outsize <= 0 || outsize < insize) {
    return false;
  }
  key = encodeparam.key;
  if(NULL == key) {
    return false;
  }
  keysize = encodeparam.keysize;
  if(keysize <= 0) {
    return false;
  }
  keyindex = encodeparam.param[0];
  for(index = 0; static_cast<int32_t>(insize) > index; ++index) {
    out[index] = in[index] ^ key[keyindex];
    ++keyindex;
    if(keyindex >= static_cast<int32_t>(keysize)) {
      keyindex -=keyindex;
    }
  }
  encodeparam.param[0] = keyindex;
  return true;
}

bool skip(encodeparam_t &encodeparam, int32_t length) {
  uint32_t keysize = 0;
  int32_t keyindex = 0;
  int32_t index = 0;
  keysize = encodeparam.keysize;
  if(keysize == 0) {
    return false;
  }
  keyindex = encodeparam.param[0];
  for(index = 0; index < length; ++index) {
    ++keyindex;
    if(keyindex >= static_cast<int32_t>(keysize)) {
      keyindex -= keysize;
    }
  }
  encodeparam.param[0] = keyindex;
  return true;
}

}; //namespace encode

}; //namespace socket

}; //namespace pf_net
