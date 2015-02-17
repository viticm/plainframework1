#include "pf/util/compressor/minimanager.h"
#include "pf/net/socket/compressor.h"

using namespace pf_net::socket;

Compressor::Compressor() {
  __ENTER_FUNCTION
    clear();
  __LEAVE_FUNCTION
}

Compressor::~Compressor() {
  __ENTER_FUNCTION
    SAFE_DELETE_ARRAY(buffer_);
    clear();
  __LEAVE_FUNCTION
}

void Compressor::clear() {
  __ENTER_FUNCTION
    head_ = NET_SOCKET_COMPRESSOR_HEADER_SIZE;
    tail_ = NET_SOCKET_COMPRESSOR_HEADER_SIZE;
    maxsize_ = 0;
    buffer_ = NULL;
    encryptor_ = NULL;
  __LEAVE_FUNCTION
}

bool Compressor::alloc(uint32_t size) {
  __ENTER_FUNCTION
    SAFE_DELETE_ARRAY(buffer_);
    buffer_ = new char[size];
    if (NULL == buffer_) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

char *Compressor::getbuffer() {
  return buffer_;
}

char *Compressor::getheader() {
  return buffer_ + head_;
}

uint32_t Compressor::getsize() const {
  return tail_ - head_;
}

uint32_t Compressor::get_maxsize() const {
  return maxsize_;
}

void Compressor::sethead(uint32_t head) {
  head_ = head;
}

void Compressor::settail(uint32_t tail) {
	tail_ = tail;
}

bool Compressor::pushback(uint32_t size) {
  __ENTER_FUNCTION
    tail_ += size;
    if (tail_ > maxsize_) {
      tail_ = maxsize_ - 1;
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Compressor::pophead(uint32_t size) {
  __ENTER_FUNCTION
    head_ += size;
    if (head_ > tail_) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Compressor::add_packetheader() {
  __ENTER_FUNCTION
    uint16_t size = static_cast<uint16_t>(getsize());
    size |= 0x8000;
    *reinterpret_cast<uint16_t *>(buffer_) = size;
    head_ = 0;
  __LEAVE_FUNCTION
}

void Compressor::encrypt() {
  __ENTER_FUNCTION
    encryptor_->encrypt(buffer_, buffer_, getsize());
  __LEAVE_FUNCTION
}

void Compressor::resetposition() {
  head_ = tail_ = NET_SOCKET_COMPRESSOR_HEADER_SIZE;
}

void Compressor::setencryptor(Encryptor *encryptor) {
  encryptor_ = encryptor;
}

bool Compressor::compress(const char *in, 
                          uint32_t insize, 
                          char *out, 
                          uint32_t &outsize) {
  __ENTER_FUNCTION
    assistant_.compressframe_inc();
    if (insize < NET_SOCKET_COMPRESSOR_IN_SIZE) return false;
    const unsigned char *_in = reinterpret_cast<const unsigned char *>(in);
    unsigned char *_out = reinterpret_cast<unsigned char *>(out);
    bool result = UTIL_COMPRESSOR_MINIMANAGER_POINTER->compress(
        _in, insize, _out, outsize, assistant_.get_workmemory());
    if (true == result) {
      assistant_.compressframe_successinc();
      pushback(outsize);
      add_packetheader();
      //logging
      if (UTIL_COMPRESSOR_MINIMANAGER_POINTER->log_isenable()) {
        UTIL_COMPRESSOR_MINIMANAGER_POINTER->add_compress_datasize(
            outsize + NET_SOCKET_COMPRESSOR_HEADER_SIZE);
        UTIL_COMPRESSOR_MINIMANAGER_POINTER->add_uncompress_datasize(insize);
      }
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Compressor::decompress(const char *in,
                            uint32_t insize,
                            char *out,
                            uint32_t &outsize) {
  __ENTER_FUNCTION
    const unsigned char *_in = reinterpret_cast<const unsigned char *>(in);
    unsigned char *_out = reinterpret_cast<unsigned char *>(out);
    int32_t result = UTIL_COMPRESSOR_MINIMANAGER_POINTER->decompress(
        _in + NET_SOCKET_COMPRESSOR_HEADER_SIZE, insize, _out, outsize);
    bool _result = LZO_E_OK == result;
    return _result;
  __LEAVE_FUNCTION
    return false;
}

pf_util::compressor::Assistant *Compressor::getassistant() {
  __ENTER_FUNCTION
    pf_util::compressor::Assistant *assistant = &assistant_;
    return assistant;
  __LEAVE_FUNCTION
    return NULL;
}
