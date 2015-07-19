#include "pf/base/util.h"
#include "pf/base/log.h"
#include "pf/net/socket/inputstream.h"

namespace pf_net {

namespace socket {

uint32_t InputStream::read(char *buffer, uint32_t length) {
  __ENTER_FUNCTION
    uint32_t result = length; //normal state
    if (0 == length || length > reallength()) {
      return 0;
    }
    if (streamdata_.head < streamdata_.tail) {
      if (encrypt_isenable()) {
        encryptor_.decrypt(buffer, 
                           &(streamdata_.buffer[streamdata_.head]), 
                           length);
      } else {
        memcpy(buffer, &(streamdata_.buffer[streamdata_.head]), length);
      }
    } else {
      uint32_t rightlength = streamdata_.bufferlength - streamdata_.head;
      if (length < rightlength) {
        if (encrypt_isenable()) {
          encryptor_.decrypt(&buffer[0], 
                             &(streamdata_.buffer[streamdata_.head]), 
                             length);
        } else {
          memcpy(&buffer[0], &(streamdata_.buffer[streamdata_.head]), length);
        }
      } else {
        if (encrypt_isenable()) {
          encryptor_.decrypt(&buffer[0], 
                             &(streamdata_.buffer[streamdata_.head]), 
                             rightlength);
          encryptor_.decrypt(&buffer[rightlength], 
                             &(streamdata_.buffer[0]), 
                             length - rightlength);
        } else {
          memcpy(&buffer[0], 
                 &(streamdata_.buffer[streamdata_.head]), 
                 rightlength);
          memcpy(&buffer[rightlength], 
                 &(streamdata_.buffer[0]), 
                 length - rightlength);
        }
      }
    }
    streamdata_.head = (streamdata_.head + length) % streamdata_.bufferlength;
    return result;
  __LEAVE_FUNCTION
    return 0;
}

bool InputStream::readpacket(packet::Base *packet) {
  __ENTER_FUNCTION
    bool result = false;
    result = skip(NET_PACKET_HEADERSIZE);
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
    if (streamdata_.head < streamdata_.tail) {
      if (encrypt_isenable()) {
        encryptor_.decrypt(buffer, 
                           &(streamdata_.buffer[streamdata_.head]), length);
      } else {
        memcpy(buffer, &(streamdata_.buffer[streamdata_.head]), length);
      }
    } else {
      uint32_t rightlength = streamdata_.bufferlength - streamdata_.head;
      if (length < rightlength) {
        if (encrypt_isenable()) {
          encryptor_.decrypt(&buffer[0], 
                             &(streamdata_.buffer[streamdata_.head]), 
                             length);
        } else {
          memcpy(&buffer[0], &(streamdata_.buffer[streamdata_.head]), length);
        }
      } else {
        if (encrypt_isenable()) {
          encryptor_.decrypt(&buffer[0], 
                             &(streamdata_.buffer[streamdata_.head]), 
                             rightlength);
          encryptor_.decrypt(&buffer[rightlength], 
                             &(streamdata_.buffer[0]), 
                             length - rightlength);
        } else {
          memcpy(&buffer[0], 
                 &(streamdata_.buffer[streamdata_.head]), 
                 rightlength);
          memcpy(&buffer[rightlength], 
                 &(streamdata_.buffer[0]), 
                 length - rightlength);
        }
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
    uint32_t head = streamdata_.head;
    uint32_t bufferlength = streamdata_.bufferlength;
    streamdata_.head = (head + length) % bufferlength;
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
    if (NULL == streamdata_.buffer) return -1;
    // head tail length=10
    // 0123456789
    // abcd......

    if (streamdata_.head <= streamdata_.tail) {
      if (0 == streamdata_.head) {
        receivecount = 0;
        freecount = streamdata_.bufferlength - streamdata_.tail - 1;
        if (freecount != 0) {
          receivecount = socket_->receive(
              &streamdata_.buffer[streamdata_.tail], freecount);
          if (SOCKET_ERROR_WOULD_BLOCK == receivecount) return 0;
          if (SOCKET_ERROR == receivecount) return SOCKET_ERROR - 1;
          if (0 == receivecount) return SOCKET_ERROR - 2;
          streamdata_.tail += receivecount;
          fillcount += receivecount;
          if (static_cast<uint32_t>(receivecount) == freecount) {
            uint32_t available = socket_->available();
            if (available <= 0) return fillcount;
            if ((streamdata_.bufferlength + available + 1) > 
                streamdata_.bufferlength_max) {
              init();
              return SOCKET_ERROR - 3;
            }
            if (!resize(available + 1)) return 0;
            receivecount = socket_->receive(
                &streamdata_.buffer[streamdata_.tail], available);
            if (SOCKET_ERROR_WOULD_BLOCK == receivecount) return 0;
            if (SOCKET_ERROR == receivecount) return SOCKET_ERROR - 4;
            if (0 == receivecount) return SOCKET_ERROR - 5;
            streamdata_.tail += receivecount;
            fillcount += receivecount; 
          }
        }
      } else { // 0 == head
        freecount = streamdata_.bufferlength - streamdata_.tail;
        receivecount = socket_->receive(
            &streamdata_.buffer[streamdata_.tail], freecount);
        if (SOCKET_ERROR_WOULD_BLOCK == receivecount) return 0;
        if (SOCKET_ERROR == receivecount) return SOCKET_ERROR - 6;
        if (0 == receivecount) return SOCKET_ERROR - 7;
        streamdata_.tail = 
          (streamdata_.tail + receivecount) % streamdata_.bufferlength;
        fillcount += receivecount;
        if (static_cast<uint32_t>(receivecount) == freecount) {
          receivecount = 0;
          freecount = streamdata_.head - 1;
          if (freecount != 0) {
            receivecount = socket_->receive(
                &streamdata_.buffer[0], freecount);
            if (SOCKET_ERROR_WOULD_BLOCK == receivecount) return 0;
            if (SOCKET_ERROR == receivecount) return SOCKET_ERROR - 8;
            if (0 == receivecount) return SOCKET_ERROR - 9;
            streamdata_.tail += receivecount;
            fillcount += receivecount;
          }
          if (static_cast<uint32_t>(receivecount) == freecount) {
            uint32_t available = socket_->available();
            if (available <= 0) return fillcount;
            if ((streamdata_.bufferlength + available + 1) > 
                streamdata_.bufferlength_max) {
              init();
              return SOCKET_ERROR - 10;
            }
            if (!resize(available + 1)) return 0;
            receivecount = socket_->receive(
                &streamdata_.buffer[streamdata_.tail], available);
            if (SOCKET_ERROR_WOULD_BLOCK == receivecount) return 0;
            if (SOCKET_ERROR == receivecount) return SOCKET_ERROR - 11;
            if (0 == receivecount) return SOCKET_ERROR - 12;
            streamdata_.tail += receivecount;
            fillcount += receivecount; 
          }
        }
      }
    } else { //streamdata_.head <= streamdata_.tail
      receivecount = 0;
      freecount = streamdata_.head - streamdata_.tail - 1;
      if (freecount != 0) {
        receivecount = socket_->receive(
            &streamdata_.buffer[streamdata_.tail], freecount);
        if (SOCKET_ERROR_WOULD_BLOCK == receivecount) return 0;
        if (SOCKET_ERROR == receivecount) return SOCKET_ERROR - 13;
        if (0 == receivecount) return SOCKET_ERROR - 14;
        streamdata_.tail += receivecount;
        fillcount += receivecount;
      }
      if (static_cast<uint32_t>(receivecount) == freecount) {
        uint32_t available = socket_->available();
        if (available <= 0) return fillcount;
        if ((streamdata_.bufferlength + available + 1) > 
            streamdata_.bufferlength_max) {
          init();
          return SOCKET_ERROR - 15;
        }
        if (!resize(available + 1)) return 0;
        receivecount = socket_->receive(
            &streamdata_.buffer[streamdata_.tail], available);
        if (SOCKET_ERROR_WOULD_BLOCK == receivecount) return 0;
        if (SOCKET_ERROR == receivecount) return SOCKET_ERROR - 16;
        if (0 == receivecount) return SOCKET_ERROR - 17;
        streamdata_.tail += receivecount;
        fillcount += receivecount; 
      }
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
    if (length <= 0 || size < length) {
      if (size < length) {
        SLOW_ERRORLOG(NET_MODULENAME,
                      "[net.socket] InputStream::read_string size < length"
                      " not read, size: %d, length: %d",
                      size, 
                      length);
      }
      return;
    }
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

void InputStream::compressenable(bool enable) {
  __ENTER_FUNCTION
    compressor_.getassistant()->enable(enable);
  __LEAVE_FUNCTION
}

uint32_t InputStream::write(const char *buffer, uint32_t length) {
  __ENTER_FUNCTION
    //this function diffrent from OutputStream::write is the streamdata_.bufferlength not resize
    uint32_t freecount = 0;
    uint32_t fillcount = 0;
    if (streamdata_.head <= streamdata_.tail) {
      if (0 == streamdata_.head) {
        freecount = streamdata_.bufferlength - streamdata_.tail - 1;
        uint32_t copysize = freecount > length ? length : freecount;
        if (encrypt_isenable()) {
          encryptor_.encrypt(
              &streamdata_.buffer[streamdata_.tail], buffer, copysize);
        } else {
          memcpy(&streamdata_.buffer[streamdata_.tail], buffer, copysize);
        }
      } else {
        freecount = streamdata_.bufferlength - streamdata_.tail;
        uint32_t copysize1 = freecount > length ? length : freecount;
        if (encrypt_isenable()) {
          encryptor_.encrypt(
              &streamdata_.buffer[streamdata_.tail], buffer, copysize1);
        } else {
          memcpy(&streamdata_.buffer[streamdata_.tail], buffer, copysize1);
        }
        fillcount += copysize1;
        streamdata_.tail = 
          (streamdata_.tail + copysize1) % streamdata_.bufferlength;
        buffer += copysize1;
        freecount = streamdata_.head - 1;
        uint32_t copysize2 = freecount > length - copysize1 ? 
                             length - copysize1 : 
                             freecount;
        if (encrypt_isenable()) {
          encryptor_.encrypt(
              &streamdata_.buffer[streamdata_.tail], buffer, copysize2);
        } else {
          memcpy(&streamdata_.buffer[streamdata_.tail], buffer, copysize2);
        }
        fillcount += copysize2;
        streamdata_.tail += copysize2;
      }
    } else {
      freecount = streamdata_.head - streamdata_.tail - 1;
      uint32_t copysize = freecount > length ? length : freecount;
      if (encrypt_isenable()) {
        encryptor_.encrypt(
            &streamdata_.buffer[streamdata_.tail], buffer, copysize);
      } else {
        memcpy(&streamdata_.buffer[streamdata_.tail], buffer, copysize);
      }
      fillcount += copysize;
      streamdata_.tail += copysize;
    }
    return fillcount;
  __LEAVE_FUNCTION
    return 0;
}

} //namespace socket

} //namespace pap_common_net
