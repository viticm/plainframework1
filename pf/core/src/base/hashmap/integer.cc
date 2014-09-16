#include "pf/base/hashmap/integer.h"

namespace pf_base {

namespace hashmap {

Integer::Integer() {
  maxcount_ = 0;
}

Integer::~Integer() {
  maxcount_ = 0;
}

void Integer::init(uint32_t count) {
  __ENTER_FUNCTION
    hashmap_.clear();
    maxcount_ = count;
  __LEAVE_FUNCTION
}

bool Integer::add(uint32_t id, void *pointer) {
  __ENTER_FUNCTION
    if (hashmap_.size() > maxcount_) return false;
    hashmap_.insert(hashmap_t::value_type(id, pointer));
    return true;
  __LEAVE_FUNCTION
    return false;
}

void *Integer::get(uint32_t id) {
  __ENTER_FUNCTION
    iterator_t iterator = hashmap_.find(id);
    if (iterator != hashmap_.end()) return iterator->second;
    return NULL;
  __LEAVE_FUNCTION
    return NULL;
}

bool Integer::remove(uint32_t id) {
  __ENTER_FUNCTION
    iterator_t iterator = hashmap_.find(id);
    if (iterator != hashmap_.end()) {
      hashmap_.erase(iterator);
      return true;
    }
    return false;
  __LEAVE_FUNCTION
    return false;
}

void Integer::clear() {
  __ENTER_FUNCTION
    hashmap_.clear();
  __LEAVE_FUNCTION
}

uint32_t Integer::getcount() const {
  uint32_t count = static_cast<uint32_t>(hashmap_.size());
  return count;
}

uint32_t Integer::get_maxcount() const {
  return maxcount_;
}

Integer::iterator_t Integer::begin() {
  return hashmap_.begin();
}

Integer::iterator_t Integer::end() {
  return hashmap_.end();
}

}; //namespace hashmap

}; //namespace pf_base
