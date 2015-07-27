/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/pap )
 * $Id bitflag.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/06 21:20
 * @uses bit flag class
 *       cn: 位标记操作类，如16位00000000000000011，主要做综合的标记位
 *           真假的标记位
 */
#ifndef PF_UTIL_BITFLAG_H_
#define PF_UTIL_BITFLAG_H_

#include "pf/util/config.h"

namespace pf_util {

template <uint32_t size>
class BitFlags {

 public:
   BitFlags() {
     memset(flags_, 0, sizeof(flags_));
   };
   BitFlags(BitFlags const &object) {
     memcpy(flags_, object.getflags(), sizeof(flags_));
   };
   ~BitFlags() {};

 public:
   enum {kBitSize = size, kByteSize = 1 + kBitSize / 8,};

 public:
   BitFlags &operator = (BitFlags const &object);
   void markall();
   void clearall();
   bool get(int32_t index);
   void clear(int32_t index);
   void mark(int32_t index);
   uint32_t get_bitsize() const;
   uint32_t get_bytesize() const;
   const char *getflags() const;

 private:
   char flags_[kByteSize];

};

template <uint32_t size>
BitFlags<size> &BitFlags<size>::operator = (BitFlags<size> const &object) {
  memcpy(flags_, object.getflags(), sizeof(flags_));
  return *this;
}

template <uint32_t size>
void BitFlags<size>::markall() {
  memset(flags_, 0xFF, sizeof(flags_));
}

template <uint32_t size>
void BitFlags<size>::clearall() {
  memset(flags_, 0, sizeof(flags_));
}

template <uint32_t size>
bool BitFlags<size>::get(int32_t index) {
  __ENTER_FUNCTION
    bool result = false;
    if (index < 0 || index >= kBitSize) return result;
    uint32_t _index = static_cast<uint32_t>(index);
    result = (flags_[_index >> 3] & static_cast<char>(1 << _index % 8)) != 0;
    return result;
  __LEAVE_FUNCTION
    return false;
}

template <uint32_t size>
void BitFlags<size>::clear(int32_t index) {
  __ENTER_FUNCTION
    if (index < 0 || index >= kBitSize) return;
    flags_[index >> 3] &= ~(0x01 << (index % 8));
  __LEAVE_FUNCTION
}

template <uint32_t size>
void BitFlags<size>::mark(int32_t index) {
  __ENTER_FUNCTION
    if (index < 0 || index >= kBitSize) return;
    flags_[index >> 3] |= ~(0x01 << (index % 8));
  __LEAVE_FUNCTION
}

template <uint32_t size>
uint32_t BitFlags<size>::get_bitsize() const {
  return kBitSize;
}

template <uint32_t size>
uint32_t BitFlags<size>::get_bytesize() const {
  return kByteSize;
}

template <uint32_t size>
const char *BitFlags<size>::getflags() const {
  return flags_;
}

}; //namespace pf_util

#endif //PF_UTIL_BITFLAG_H_
