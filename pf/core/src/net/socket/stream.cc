#include "pf/net/socket/stream.h"

namespace pf_net {

namespace socket {

Stream::Stream(Base *socket, 
               uint32_t bufferlength, 
               uint32_t bufferlength_max) {
  __ENTER_FUNCTION
    socket_ = socket;
    memset(&streamdata_, 0, sizeof(streamdata_));
    memset(&encodeparam_, 0, sizeof(encodeparam_));
    streamdata_.bufferlength = bufferlength;
    streamdata_.bufferlength_max = bufferlength_max;
    streamdata_.headlength = 0;
    streamdata_.taillength = 0;
    streamdata_.buffer = new char[sizeof(char) * bufferlength];
    memset(streamdata_.buffer, 0, streamdata_.bufferlength);
    send_bytes_ = 0;
    receive_bytes_ = 0;
  __LEAVE_FUNCTION
}

Stream::~Stream() {
  __ENTER_FUNCTION
    SAFE_DELETE_ARRAY(streamdata_.buffer);
  __LEAVE_FUNCTION
}

void Stream::init() {
  __ENTER_FUNCTION
    streamdata_.headlength = 0;
    streamdata_.taillength = 0;
    SAFE_DELETE_ARRAY(streamdata_.buffer);
    //streamdata_.bufferlength = SOCKETINPUT_BUFFERSIZE_DEFAULT;
    //streamdata_.bufferlength_max = SOCKETINPUT_DISCONNECT_MAXSIZE;
    streamdata_.buffer = new char[sizeof(char) * streamdata_.bufferlength];
    memset(streamdata_.buffer, 0, sizeof(char) * streamdata_.bufferlength);
  __LEAVE_FUNCTION
}

bool Stream::resize(int32_t size) {
  __ENTER_FUNCTION
    bool result = true;
    uint32_t bufferlength = streamdata_.bufferlength;
    uint32_t headlength = streamdata_.headlength;
    uint32_t taillength = streamdata_.taillength;
    uint32_t _reallength = reallength();
    size = max(size, static_cast<int32_t>(bufferlength >> 1));
    uint32_t newbuffer_length = bufferlength + size;
    if (size < 0 && (newbuffer_length < 0 || newbuffer_length < _reallength))
      return false;
    char *oldbuffer = streamdata_.buffer;
    char *newbuffer = new char[sizeof(char) * newbuffer_length];
    if (!newbuffer) return false;
    memset(newbuffer, 0, sizeof(char) * newbuffer_length);
    if (headlength < taillength) {
      memcpy(newbuffer, &oldbuffer[headlength], taillength - headlength);
    } else {
      memcpy(newbuffer, &oldbuffer[headlength], bufferlength - headlength);
      memcpy(&newbuffer[bufferlength - headlength], oldbuffer, taillength);
    }
    SAFE_DELETE_ARRAY(oldbuffer);
    streamdata_.buffer = newbuffer;
    streamdata_.bufferlength = newbuffer_length;
    streamdata_.headlength = 0;
    streamdata_.taillength = _reallength;
    return result;
  __LEAVE_FUNCTION
    return false;
}

uint32_t Stream::reallength() {
  __ENTER_FUNCTION
    uint32_t length = 0;
    uint32_t bufferlength = streamdata_.bufferlength;
    uint32_t headlength = streamdata_.headlength;
    uint32_t taillength = streamdata_.taillength;
    if (headlength <= taillength) {
      length = taillength - headlength;
    } else {
      length = bufferlength - headlength + taillength;
    }
    return length;
  __LEAVE_FUNCTION
    return 0;
}

void Stream::cleanup() {
  __ENTER_FUNCTION
    init();
    memset(&encodeparam_, 0, sizeof(encodeparam_));
  __LEAVE_FUNCTION
}

Base* Stream::getsocket() {
  return socket_;
}

}; //namespace socket

}; //namespace pf_net
