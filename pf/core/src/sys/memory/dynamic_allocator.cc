#include "pf/base/log.h"
#include "pf/sys/memory/dynamic_allocator.h"

namespace pf_sys {

namespace memory {

DynamicAllocator::DynamicAllocator() {
  __ENTER_FUNCTION
    pointer_ = NULL;
    size_ = 0;
  __LEAVE_FUNCTION
}

DynamicAllocator::~DynamicAllocator() {
  __ENTER_FUNCTION
    free();
  __LEAVE_FUNCTION
}

void *DynamicAllocator::malloc(size_t size) {
  __ENTER_FUNCTION
    if (size_ == size) return pointer_;
    void *pointer = reinterpret_cast<void *>(new char[size]);
    if (NULL == pointer) {
      Assert(false);
      return NULL;
    }
    memset(pointer, 0, size);
    if (pointer_ != NULL) {
      size_t copysize = size > size_ ? size_ : size;
      memcpy(pointer, pointer_, copysize);
      free();
    }
    pointer_ = pointer;
    if (pointer_ != NULL) size_ = size;
    return pointer_;
  __LEAVE_FUNCTION
    return NULL;
}

size_t DynamicAllocator::getsize() const {
  return size_;
}

void DynamicAllocator::free() {
  __ENTER_FUNCTION
    char *pointer = reinterpret_cast<char*>(pointer_);
    SAFE_DELETE_ARRAY(pointer);
    size_ = 0;
  __LEAVE_FUNCTION
}

void *DynamicAllocator::getpointer() {
  __ENTER_FUNCTION
    return pointer_;
  __LEAVE_FUNCTION
    return NULL;
}

}; //namespace memory

}; //namespace pf_sys
