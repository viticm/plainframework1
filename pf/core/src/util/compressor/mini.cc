#include "pf/base/string.h"
#include "pf/util/compressor/mini.h"

using namespace pf_base;
using namespace pf_util::compressor;

Mini::Mini() {
  __ENTER_FUNCTION
    memset(work_memory_, 0, sizeof(work_memory_));
    memset(decompress_buffer_, 0, sizeof(decompress_buffer_));
    decompress_buffersize_ = 0;
  __LEAVE_FUNCTION
}

Mini::~Mini() {
  //do nothing
}

bool Mini::init() {
  __ENTER_FUNCTION
    if (lzo_init() != LZO_E_OK) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Mini::compress(const unsigned char *in,
                    uint32_t insize,
                    unsigned char *out,
                    uint64_t& outsize) {
  __ENTER_FUNCTION
    if (outsize < UTIL_COMPRESSOR_MINI_GET_OUTLENGTH(insize)) return false;
    int32_t result = lzo1x_1_compress(in,
                                      static_cast<lzo_uint>(insize),
                                      out,
                                      reinterpret_cast<lzo_uintp>(&outsize),
                                      work_memory_);
    if (result != LZO_E_OK) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Mini::compress_andescape(const unsigned char *in,
                              uint32_t insize,
                              unsigned char *out,
                              uint32_t outsize) {
  __ENTER_FUNCTION
    uint64_t compresssize = UTIL_COMPRESSOR_MINI_GET_OUTLENGTH(insize);
    unsigned char *compressbuffer = 
      new unsigned char[static_cast<size_t>(compresssize)];
    if (NULL == compressbuffer) return false;
    if (!compress(in, insize, compressbuffer, compresssize)) {
      SAFE_DELETE_ARRAY(compressbuffer);
      Assert(false);
      return false;
    }
    if (outsize < 2 * compresssize) {
      SAFE_DELETE_ARRAY(compressbuffer);
      Assert(false);
      return false;
    }
    string::getescape(reinterpret_cast<const char *>(compressbuffer),
                      static_cast<size_t>(compresssize),
                      reinterpret_cast<char *>(out),
                      outsize);
    SAFE_DELETE_ARRAY(compressbuffer);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Mini::decompress(const unsigned char *in, uint32_t insize) {
  __ENTER_FUNCTION
    if (0 == insize) {
      decompress_buffersize_ = 0;
      return true;
    }
    int32_t result = lzo1x_decompress(
        in,
        static_cast<lzo_uint>(insize),
        decompress_buffer_,
        reinterpret_cast<lzo_uintp>(&decompress_buffersize_),
        NULL);
    if (result != LZO_E_OK) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

const unsigned char *Mini::get_decompress_buffer() {
  return decompress_buffer_;
}

uint32_t Mini::get_decompress_buffersize() const {
  uint32_t result = static_cast<uint32_t>(decompress_buffersize_);
  return result;
}
