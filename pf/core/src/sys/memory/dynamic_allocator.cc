#include "pf/sys/memory/dynamic_allocator.h"

namespace pf_sys {

namespace memory {

DynamicAllocator::DynamicAllocator() {
  __ENTER_FUNCTION
    pointer_ = NULL;
    length_ = 0;
  __LEAVE_FUNCTION
}

DynamicAllocator::~DynamicAllocator() {
  __ENTER_FUNCTION
    free();
  __LEAVE_FUNCTION
}

void *DynamicAllocator::malloc(int64_t length) {
  __ENTER_FUNCTION
    if (length_ == length) return pointer_;
    if (pointer_) free();
#if __LINUX__
    pointer_ = reinterpret_cast<void *>(new char[length]);
#elif __WINDOWS__
    pointer_ = reinterpret_cast<void *>(new char[static_cast<uint32_t>(length)]);
#endif
    if (pointer_ != NULL) length_ = length;
    return pointer_;
  __LEAVE_FUNCTION
    return NULL;
}

void DynamicAllocator::free() {
  __ENTER_FUNCTION
    char *pointer = reinterpret_cast<char*>(pointer_);
    SAFE_DELETE_ARRAY(pointer);
    length_ = 0;
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
