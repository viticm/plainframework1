#include "pf/net/socket/stream.h"

namespace pf_net {

namespace socket {

Stream::Stream(Base *socket, 
               uint32_t bufferlength, 
               uint32_t bufferlength_max) {
  __ENTER_FUNCTION
    socket_ = socket;
    memset(&streamdata_, 0, sizeof(streamdata_));
    streamdata_.bufferlength = bufferlength;
    streamdata_.bufferlength_max = bufferlength_max;
    streamdata_.head = 0;
    streamdata_.tail = 0;
    streamdata_.buffer = new char[sizeof(char) * bufferlength];
    memset(streamdata_.buffer, 0, streamdata_.bufferlength);
    send_bytes_ = 0;
    receive_bytes_ = 0;
    compressor_.sethead(NET_SOCKET_COMPRESSOR_HEADER_SIZE);
    compressor_.settail(NET_SOCKET_COMPRESSOR_HEADER_SIZE);
    compressor_.setencryptor(&encryptor_);
  __LEAVE_FUNCTION
}

Stream::~Stream() {
  __ENTER_FUNCTION
    SAFE_DELETE_ARRAY(streamdata_.buffer);
  __LEAVE_FUNCTION
}

void Stream::init() {
  __ENTER_FUNCTION
    streamdata_.head = 0;
    streamdata_.tail = 0;
    SAFE_DELETE_ARRAY(streamdata_.buffer);
    streamdata_.buffer = new char[sizeof(char) * streamdata_.bufferlength];
    memset(streamdata_.buffer, 0, sizeof(char) * streamdata_.bufferlength);
    receive_bytes_ = send_bytes_ = 0;
    compressor_.sethead(NET_SOCKET_COMPRESSOR_HEADER_SIZE);
    compressor_.settail(NET_SOCKET_COMPRESSOR_HEADER_SIZE);
  __LEAVE_FUNCTION
}

bool Stream::resize(int32_t size) {
  __ENTER_FUNCTION
    bool result = true;
    uint32_t bufferlength = streamdata_.bufferlength;
    uint32_t head = streamdata_.head;
    uint32_t tail = streamdata_.tail;
    uint32_t _reallength = reallength();
    size = max(size, static_cast<int32_t>(bufferlength >> 1));
    uint32_t newbuffer_length = bufferlength + size;
    if (size < 0 && (newbuffer_length < 0 || newbuffer_length < _reallength))
      return false;
    char *oldbuffer = streamdata_.buffer;
    char *newbuffer = new char[sizeof(char) * newbuffer_length];
    if (!newbuffer) return false;
    memset(newbuffer, 0, sizeof(char) * newbuffer_length);
    if (head < tail) {
      memcpy(newbuffer, &oldbuffer[head], tail - head);
    } else {
      memcpy(newbuffer, &oldbuffer[head], bufferlength - head);
      memcpy(&newbuffer[bufferlength - head], oldbuffer, tail);
    }
    SAFE_DELETE_ARRAY(oldbuffer);
    streamdata_.buffer = newbuffer;
    streamdata_.bufferlength = newbuffer_length;
    streamdata_.head = 0;
    streamdata_.tail = _reallength;
    return result;
  __LEAVE_FUNCTION
    return false;
}

uint32_t Stream::reallength() const {
  __ENTER_FUNCTION
    uint32_t length = 0;
    uint32_t bufferlength = streamdata_.bufferlength;
    uint32_t head = streamdata_.head;
    uint32_t tail = streamdata_.tail;
    if (head <= tail) {
      length = tail - head;
    } else {
      length = bufferlength - head + tail;
    }
    return length;
  __LEAVE_FUNCTION
    return 0;
}

void Stream::cleanup() {
  __ENTER_FUNCTION
    init();
  __LEAVE_FUNCTION
}

Base* Stream::getsocket() {
  return socket_;
}

bool Stream::encrypt_isenable() const {
  return encrypt_isenable_;
}

void Stream::encryptenable(bool enable) {
  __ENTER_FUNCTION
    encrypt_isenable_ = enable;
  __LEAVE_FUNCTION
}

void Stream::compressenable(bool enable) {
  __ENTER_FUNCTION
    compressor_.getassistant()->enable(enable);
  __LEAVE_FUNCTION
}

Compressor *Stream::getcompressor() {
  return &compressor_;
}

Encryptor *Stream::getencryptor() {
  return &encryptor_;
}

}; //namespace socket

}; //namespace pf_net
