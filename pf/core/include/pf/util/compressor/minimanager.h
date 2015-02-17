/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id minimanager.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/01/25 12:45
 * @uses mini compressor manager class, for mutli thread(other just use mini)
*/
#ifndef PF_UTIL_COMPRESSOR_MINIMANAGER_H_
#define PF_UTIL_COMPRESSOR_MINIMANAGER_H_

#include "pf/util/compressor/config.h"
#include "pf/base/singleton.h"
#include "pf/sys/thread.h"

#if __WINDOWS__ /* { */
#undef __LINUX__
#elif __LINUX__ /* }{ */
#undef __WINDOWS__
#endif /* } */

#include "pf/util/compressor/minilzo.h"

#ifndef __WINDOWS__
#define __WINDOWS__ (defined(_MSC_VER) || defined(__ICL))
#endif
#ifndef __LINUX__
#define __LINUX__ !(__WINDOWS__)
#endif

#define UTIL_COMPRESSOR_MINI_MANAGER_THREAD_SIZEMAX 1024
#define UTIL_COMPRESSOR_MINI_MANAGER_WORK_MEMORY_SIZE \
  ((LZO1X_1_MEM_COMPRESS + sizeof(lzo_align_t) - 1) / sizeof(lzo_align_t))

namespace pf_util {

namespace compressor {

class PF_API MiniManager : public pf_base::Singleton<MiniManager> {

 public:
   MiniManager();
   ~MiniManager();
 public:
   static MiniManager *getsingleton_pointer();
   static MiniManager &getsingleton();

 public:
   typedef struct workmemory_struct {
     void *pointer;
     uint64_t threadid;
   } workmemory_t;

 public:
   bool init();
   void destory();
   void *alloc(uint64_t threadid);
   bool compress(const unsigned char *in,
                 uint32_t insize,
                 unsigned char *out,
                 uint32_t &outsize,
                 void *workmemory);
   int32_t decompress(const unsigned char *in,
                      uint32_t insize,
                      unsigned char *out,
                      uint32_t &outsize);
   void add_uncompress_datasize(uint64_t size);
   void add_compress_datasize(uint64_t size);
   uint64_t get_uncompress_datasize();
   uint64_t get_compress_datasize();
   void log_enable(bool enable);
   bool log_isenable() const;

 private:
   bool log_isenable_;
   workmemory_t workmemory_[UTIL_COMPRESSOR_MINI_MANAGER_THREAD_SIZEMAX];
   uint32_t workmemory_size_;
   uint64_t uncompress_size_;
   uint64_t compress_size_;
   pf_sys::ThreadLock lock_;

};

}; //namespace compressor

}; //namespace pf_util

#define UTIL_COMPRESSOR_MINIMANAGER_POINTER \
pf_util::compressor::MiniManager::getsingleton_pointer()

PF_API extern pf_util::compressor::MiniManager *g_util_compressor_minimanager;

#endif //PF_UTIL_COMPRESSOR_MINIMANAGER_H_
