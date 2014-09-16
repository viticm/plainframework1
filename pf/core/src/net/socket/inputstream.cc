#include "pf/base/util.h"
#include "pf/net/socket/encode.h"
#include "pf/net/socket/inputstream.h"

namespace pf_net {

namespace socket {

uint32_t InputStream::read(char *buffer, uint32_t length) {
  __ENTER_FUNCTION
    uint32_t result = length; //normal state
    char *stream_buffer = streamdata_.buffer;
    uint32_t bufferlength = streamdata_.bufferlength;
    uint32_t headlength = streamdata_.headlength;
    uint32_t taillength = streamdata_.taillength;
    if (0 == length || length > reallength()) return 0;
    unsigned char *tempbuffer = new unsigned char[length];
    if (0 == tempbuffer) return 0;
    if (headlength < taillength) {
      memcpy(tempbuffer, &stream_buffer[headlength], length);
    } else {
      uint32_t rightlength = bufferlength - headlength;
      if (length < rightlength) {
        memcpy(tempbuffer, &stream_buffer[headlength], length);
      } else {
        memcpy(tempbuffer, &stream_buffer[headlength], rightlength);
        memcpy(&(tempbuffer[rightlength]), stream_buffer, length - rightlength);
      }
    }
    streamdata_.headlength = (headlength + length) % bufferlength;
    if (encodeparam_.keysize > 0) {
      bool encode_result = true;
      encodeparam_.in = tempbuffer;
      encodeparam_.insize = length;
      encodeparam_.out = reinterpret_cast<unsigned char*>(buffer);
      encodeparam_.outsize = length;
      encode_result = encode::make(encodeparam_);
      if (!encode_result) result = 0;
    } else {
      memcpy(buffer, tempbuffer, length);
    }
    SAFE_DELETE_ARRAY(tempbuffer);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

bool InputStream::readpacket(packet::Base *packet) {
  __ENTER_FUNCTION
    bool result = false;
    result = skip(PACKET_HEADERSIZE);
    if (!result) return result;
    result = packet->read(*this);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool InputStream::peek(char *buffer, uint32_t length) {
  __ENTER_FUNCTION
    if (0 == length || length > reallength()) {
      return false;
    }
    uint32_t bufferlength = streamdata_.bufferlength;
    uint32_t headlength = streamdata_.headlength;
    uint32_t taillength = streamdata_.taillength;
    char *stream_buffer = streamdata_.buffer;
    if (headlength < taillength) {
      memcpy(buffer, &(stream_buffer[headlength]), length);
    } else {
      uint32_t rightlength = bufferlength - headlength;
      if (length < rightlength) {
        memcpy(&buffer[0], &(stream_buffer[headlength]), length);
      } else {
        memcpy(&buffer[0], &(stream_buffer[headlength]), rightlength);
        memcpy(&buffer[rightlength], &(stream_buffer[0]), length - rightlength);
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool InputStream::skip(uint32_t length) {
  __ENTER_FUNCTION
    if (0 == length || length > reallength()) return false;
    bool result = true;
    uint32_t headlength = streamdata_.headlength;
    uint32_t bufferlength = streamdata_.bufferlength;
    streamdata_.headlength = (headlength + length) % bufferlength;
    if (encodeparam_.keysize > 0) {
      encodeparam_.in = NULL;
      encodeparam_.insize = 0;
      encodeparam_.out = NULL;
      encodeparam_.outsize = 0;
      result = encode::skip(encodeparam_, length);
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

int32_t InputStream::fill() {
  __ENTER_FUNCTION
    if (!socket_->isvalid()) return 0;
    uint32_t fillcount = 0;
    int32_t receivecount = 0;
    uint32_t freecount = 0;
    uint32_t bufferlength = streamdata_.bufferlength;
    uint32_t bufferlength_max = streamdata_.bufferlength_max;
    uint32_t headlength = streamdata_.headlength;
    uint32_t taillength = streamdata_.taillength;
    char *stream_buffer = streamdata_.buffer;
    // head tail length=10
    // 0123456789
    // abcd......
    if (headlength < taillength) {
      freecount = 0 == headlength ? 
                  bufferlength - taillength - 1 : 
                  bufferlength - headlength;
    } else {
      freecount = headlength - taillength - 1;
    }
    if (freecount != 0) {
      receivecount = socket_->receive(&stream_buffer[taillength], freecount);
      if (SOCKET_ERROR_WOULD_BLOCK == receivecount) return 0;
      if (SOCKET_ERROR == receivecount) return SOCKET_ERROR - 1;
      if (0 == receivecount) return SOCKET_ERROR - 2;
      streamdata_.taillength += receivecount;
      fillcount += receivecount;
    }
    if (static_cast<uint32_t>(receivecount) == freecount) {
      uint32_t available = socket_->available();
      if ((bufferlength + available + 1) > bufferlength_max) {
        init();
        return SOCKET_ERROR - 3;
      }
      if (!resize(available + 1)) return 0;
      receivecount = socket_->receive(&stream_buffer[taillength], available);
      if (SOCKET_ERROR_WOULD_BLOCK == receivecount) return 0;
      if (SOCKET_ERROR == receivecount) return SOCKET_ERROR - 4;
      if (0 == receivecount) return SOCKET_ERROR - 5;
      streamdata_.taillength += receivecount;
      fillcount += receivecount; 
    }
    return fillcount;
  __LEAVE_FUNCTION
    return 0;
}

int8_t InputStream::read_int8() {
  __ENTER_FUNCTION
    int8_t result = 0;
    read((char*)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint8_t InputStream::read_uint8() {
  __ENTER_FUNCTION
    uint8_t result = 0;
    read((char*)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}

int16_t InputStream::read_int16() {
  __ENTER_FUNCTION
    int16_t result = 0;
    read((char*)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
uint16_t InputStream::read_uint16() {
  __ENTER_FUNCTION
    uint16_t result = 0;
    read((char*)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
int32_t InputStream::read_int32() {
  __ENTER_FUNCTION
    int32_t result = 0;
    read((char*)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint32_t InputStream::read_uint32() {
  __ENTER_FUNCTION
    uint32_t result = 0;
    read((char*)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
int64_t InputStream::read_int64() {
  __ENTER_FUNCTION
    int64_t result = 0;
    read((char*)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
uint64_t InputStream::read_uint64() {
  __ENTER_FUNCTION
    uint64_t result = 0;
    read((char*)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
void InputStream::read_string(char *buffer, size_t size) {
  __ENTER_FUNCTION
    uint32_t length = read_uint32();
    if (length <= 0 || size < length) return;
    read(buffer, length);
  __LEAVE_FUNCTION
}

float InputStream::read_float() {
  __ENTER_FUNCTION
    float result = 0;
    read((char*)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0.0f;
}

double InputStream::read_double() {
  __ENTER_FUNCTION
    double result = 0;
    read((char*)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0.0;
}

} //namespace socket

} //namespace pap_common_net
