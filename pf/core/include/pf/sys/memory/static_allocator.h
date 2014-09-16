/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id static_allocator.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/25 20:47
 * @uses the system memory allocator
 */
#ifndef PF_SYS_MEMORY_STATIC_ALLOCATOR_H_
#define PF_SYS_MEMORY_STATIC_ALLOCATOR_H_

#include "pf/sys/config.h"

namespace pf_sys {

namespace memory {

class PF_API StaticAllocator {

 public:
   StaticAllocator();
   ~StaticAllocator();

 public:
   void init(char *buffer, size_t size);
   void clear();
   void *malloc(size_t size);
   void *calloc(size_t count, size_t size);
   void *realloc(void *data, size_t newsize);
   void free(void *data);

 private:
   char *buffer_;
   size_t size_;
   size_t offset_;

};

}; //namespace memory

}; //namespace pf_sys

#endif //PF_SYS_MEMORY_STATIC_ALLOCATOR_H_
