#include "pf/base/log.h"
#include "pf/sys/memory/static_allocator.h"

namespace pf_sys {

namespace memory {

StaticAllocator::StaticAllocator() {
  __ENTER_FUNCTION
    buffer_ = NULL;
    size_ = 0;
    offset_ = 0;
  __LEAVE_FUNCTION
}

StaticAllocator::~StaticAllocator() {
  //do nothing
}

void StaticAllocator::clear() {
  offset_ = 0;
}

void StaticAllocator::init(char *buffer, size_t size) {
  __ENTER_FUNCTION
    buffer_ = buffer;
    size_ = size;
  __LEAVE_FUNCTION
}

void *StaticAllocator::malloc(size_t size) {
  __ENTER_FUNCTION
    using namespace pf_base;
    if (offset_ + size > size_) {
      SLOW_ERRORLOG("error",
                    "[sys.memory] (StaticAllocator::malloc)"
                    " out of memory allocating %d bytes",
                    size);
      Assert(false);
      return NULL;
    }
    char *pointer = &buffer_[offset_]; 
    offset_ += size;
    return reinterpret_cast<void*>(pointer);
  __LEAVE_FUNCTION
    return NULL;
}

void *StaticAllocator::calloc(size_t count, size_t size) {
  __ENTER_FUNCTION
    void *pointer = malloc(count * size);
    memset(pointer, 0, count * size);
    return reinterpret_cast<void*>(pointer);
  __LEAVE_FUNCTION
    return NULL;
}

void *StaticAllocator::realloc(void *data, size_t newsize) {
  __ENTER_FUNCTION
    Assert(data >= buffer_ && data < buffer_ + size_);
    size_t size_ofdata = 
      offset_ - static_cast<size_t>(reinterpret_cast<char*>(data) - buffer_);
    size_t size = newsize - size_ofdata;
    if (offset_ + size > size_) {
      SLOW_ERRORLOG("error",
                    "[sys.memory] (StaticAllocator::malloc)"
                    " out of memory allocating %d bytes",
                    size);
      Assert(false);
      return NULL;
    }
    else {
      offset_ += size;
      return data;
    }
  __LEAVE_FUNCTION
    return NULL;
}

void StaticAllocator::free(void *data) {
  __ENTER_FUNCTION
    USE_PARAM(data);
    Assert(data >= buffer_ && data < buffer_ + size_);
  __LEAVE_FUNCTION
}

}; //namespace memory

}; //namespace pf_sys
