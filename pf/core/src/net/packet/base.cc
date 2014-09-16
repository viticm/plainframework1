#include "pf/net/packet/base.h"

namespace pf_net {

namespace packet {

Base::Base() {
  //do nothing
}

Base::~Base() {
  //do nothing
}

int8_t Base::getindex() const {
  return index_;
}

void Base::setindex(int8_t index) {
  index_ = index;
}

uint8_t Base::getstatus() const {
  return status_;
}

void Base::setstatus(uint8_t status) {
  status_ = status;
}

} //namespace packet

} //namespace pf_net
