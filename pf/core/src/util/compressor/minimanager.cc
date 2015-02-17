#include "pf/util/compressor/minimanager.h"

using namespace pf_util::compressor;

pf_util::compressor::MiniManager *g_util_compressor_minimanager = NULL;

template <>
pf_util::compressor::MiniManager
  *pf_base::Singleton<pf_util::compressor::MiniManager>::singleton_ = NULL;

MiniManager *MiniManager::getsingleton_pointer() {
  return singleton_;
}

MiniManager &MiniManager::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

MiniManager::MiniManager() {
  __ENTER_FUNCTION
    log_isenable_ = false;
    workmemory_size_ = 0;
    uncompress_size_ = 0;
    compress_size_ = 0;
    for (uint16_t i = 0; i < UTIL_COMPRESSOR_MINI_MANAGER_THREAD_SIZEMAX; ++i) {
      workmemory_[i].pointer = NULL;
      workmemory_[i].threadid = 0;
    }
  __LEAVE_FUNCTION
}

MiniManager::~MiniManager() {
  destory();
}

bool MiniManager::init() {
  __ENTER_FUNCTION
    bool result = LZO_E_OK == lzo_init();
    return result;
  __LEAVE_FUNCTION
    return false;
}

void MiniManager::destory() {
  __ENTER_FUNCTION
    workmemory_size_ = 0;
    uncompress_size_ = 0;
    compress_size_ = 0;
    for (uint16_t i = 0; i < UTIL_COMPRESSOR_MINI_MANAGER_THREAD_SIZEMAX; ++i) {
      char *pointer = reinterpret_cast<char *>(workmemory_[i].pointer);
      SAFE_DELETE_ARRAY(pointer);
      workmemory_[i].threadid = 0;
    }
  __LEAVE_FUNCTION
}

void *MiniManager::alloc(uint64_t threadid) {
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
    for (uint16_t i = 0; i < UTIL_COMPRESSOR_MINI_MANAGER_THREAD_SIZEMAX; ++i) {
      if (workmemory_[i].threadid == threadid) return workmemory_[i].pointer;
    }
    if (workmemory_size_ > UTIL_COMPRESSOR_MINI_MANAGER_THREAD_SIZEMAX) 
      return NULL;
    lzo_align_t* workmemory = 
      new lzo_align_t[UTIL_COMPRESSOR_MINI_MANAGER_WORK_MEMORY_SIZE];
    workmemory_[workmemory_size_].pointer = workmemory;
    workmemory_[workmemory_size_].threadid = threadid;
    ++workmemory_size_;
    return workmemory;
  __LEAVE_FUNCTION
    return NULL;
}

bool MiniManager::compress(const unsigned char *in,
                           uint32_t insize,
                           unsigned char *out,
                           uint32_t &outsize,
                           void *workmemory) {
  __ENTER_FUNCTION
    int32_t result = 
      lzo1x_1_compress(in, insize, out, (lzo_uint *)&outsize, workmemory);
    if (result != LZO_E_OK || outsize + 2 >= insize) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

int32_t MiniManager::decompress(const unsigned char *in,
                                uint32_t insize,
                                unsigned char *out,
                                uint32_t &outsize) {
  __ENTER_FUNCTION
    int32_t result = 
      lzo1x_decompress(in, insize, out, (lzo_uint *)&outsize, NULL);
    return result;
  __LEAVE_FUNCTION
    return -1;
}

void MiniManager::add_uncompress_datasize(uint64_t size) {
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
    uncompress_size_ += size;
  __LEAVE_FUNCTION
}

void MiniManager::add_compress_datasize(uint64_t size) {
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
    compress_size_ += size;
  __LEAVE_FUNCTION
}

uint64_t MiniManager::get_uncompress_datasize() {
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
    return uncompress_size_;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t MiniManager::get_compress_datasize() {
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
    return compress_size_;
  __LEAVE_FUNCTION
    return 0;
}

void MiniManager::log_enable(bool enable) {
  log_isenable_ = enable;
}

bool MiniManager::log_isenable() const {
  return log_isenable_;
}
