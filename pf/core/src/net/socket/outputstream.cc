#include "pf/net/socket/outputstream.h"

namespace pf_net {

namespace socket {

void OutputStream::cleanup() {
  __ENTER_FUNCTION
    Stream::cleanup();
    tail_ = 0;
  __LEAVE_FUNCTION
}

uint32_t OutputStream::write(const char *buffer, uint32_t length) {
  __ENTER_FUNCTION
    /**
     * tail head       head tail --length 10
     * 0123456789      0123456789
     * abcd...efg      ...abcd...
     */
    uint32_t head = streamdata_.head;
    uint32_t tail = streamdata_.tail;
    uint32_t bufferlength = streamdata_.bufferlength;
    uint32_t freecount = head <= tail ? 
                         bufferlength - tail + head - 1 : 
                         head - tail - 1;
    if (length >= freecount && !resize(length - freecount + 1)) return 0;
    bufferlength = streamdata_.bufferlength;
    if (head <= tail) {
      uint32_t freecount = bufferlength - tail - 1;
      if (length <= freecount) {
        if (encrypt_isenable()) {
          encryptor_.encrypt(&(streamdata_.buffer[tail]), buffer, length);
        } else {
          memcpy(&(streamdata_.buffer[tail]), buffer, length);
        }
      } else {
        if (encrypt_isenable()) {
          encryptor_.encrypt(&(streamdata_.buffer[tail]), buffer, freecount);
          encryptor_.encrypt(streamdata_.buffer, 
                             &buffer[freecount], 
                             bufferlength - freecount);
        } else {
          memcpy(&(streamdata_.buffer[tail]), buffer, freecount);
          memcpy(streamdata_.buffer, &buffer[freecount], length - freecount);
        }
      }
    } else {
      if (encrypt_isenable()) {
        encryptor_.encrypt(&(streamdata_.buffer[tail]), buffer, length);
      } else {
        memcpy(&(streamdata_.buffer[tail]), buffer, length);
      }
    }
    streamdata_.tail = (tail + length) % bufferlength;
    return length;
  __LEAVE_FUNCTION
    return 0;
}

bool OutputStream::writepacket(const packet::Base* packet) {
  __ENTER_FUNCTION
    bool result = false;
    uint16_t packetid = packet->getid();
    uint32_t packetcheck; //index and size(if diffrent then have error)
    //write packetid
    write(reinterpret_cast<const char *>(&packetid), sizeof(packetid));
    uint32_t packetsize = packet->getsize();
    uint32_t packetindex = packet->getindex();
    NET_PACKET_SETINDEX(packetcheck, packetindex);
    NET_PACKET_SETLENGTH(packetcheck, packetsize);
    write(reinterpret_cast<const char *>(&packetcheck), sizeof(packetcheck));
    result = packet->write(*this);
    return result;
  __LEAVE_FUNCTION
    return false;
}

int32_t OutputStream::flush() {
  __ENTER_FUNCTION
    if (!socket_->isvalid()) return 0;
    if (compressor_.getassistant()->isenable()) { //compress is enable
      uint32_t result = 0;
      uint32_t sendcount = 0;
      uint32_t tail = get_floortail();
      if (static_cast<int32_t>(tail) < -1) return tail;
      if (compress(tail)) return -1;
      result = compressflush();
      sendcount += result;
      if (static_cast<int32_t>(result) <= SOCKET_ERROR) 
        return static_cast<int32_t>(result);
      rawprepare(tail);
      result = rawflush();
      sendcount += result;
      if (static_cast<int32_t>(result) <= SOCKET_ERROR) 
        return static_cast<int32_t>(result);
      return sendcount;
    }
    uint32_t flushcount = 0;
    int32_t sendcount = 0;
    uint32_t leftcount = 0;
    uint32_t flag = 0;
    uint32_t bufferlength = streamdata_.bufferlength;
    uint32_t bufferlength_max = streamdata_.bufferlength_max;
    uint32_t head = streamdata_.head;
    uint32_t tail = streamdata_.tail;
    char *stream_buffer = streamdata_.buffer;
    if (bufferlength > bufferlength_max) {
      init();
      return SOCKET_ERROR - 1;
    }
#if __LINUX__
    flag = MSG_NOSIGNAL;
#elif __WINDOWS__
    flag = MSG_DONTROUTE;
#endif
    leftcount = head < tail ? 
                tail - head : 
                bufferlength - head;
    while (leftcount > 0) {
      sendcount = socket_->send(&stream_buffer[head], leftcount, flag);
      if (SOCKET_ERROR_WOULD_BLOCK == sendcount) {
        return 0;
      }
      if (SOCKET_ERROR == sendcount) {
        return SOCKET_ERROR - 2;
      }
      if (0 == sendcount) {
        return 0;
      }
      flushcount += sendcount;
      leftcount -= sendcount;
      streamdata_.head += sendcount;
    }
    if (head > tail) {
      streamdata_.head = 0;
      leftcount = tail;
      while (leftcount > 0) {
        sendcount = socket_->send(&stream_buffer[head], leftcount, flag);
        if (SOCKET_ERROR_WOULD_BLOCK == sendcount) {
          return 0;
        }
        if (SOCKET_ERROR == sendcount) {
          return SOCKET_ERROR - 3;
        }
        if (0 == sendcount) {
          return 0;
        }
        flushcount += sendcount;
        leftcount -= sendcount;
        streamdata_.head += sendcount;
      }
    }
    if (streamdata_.head == streamdata_.tail)
      streamdata_.head = streamdata_.tail = 0;
    int32_t result = static_cast<int32_t>(flushcount);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

bool OutputStream::write_int8(int8_t value) {
  __ENTER_FUNCTION
    uint32_t count = write((char*)&value, sizeof(value));
    bool result = count == sizeof(value) ? true : false;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool OutputStream::write_uint8(uint8_t value) {
  __ENTER_FUNCTION
    uint32_t count = write((char*)&value, sizeof(value));
    bool result = count == sizeof(value) ? true : false;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool OutputStream::write_int16(int16_t value) {
  __ENTER_FUNCTION
    uint32_t count = write((char*)&value, sizeof(value));
    bool result = count == sizeof(value) ? true : false;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool OutputStream::write_uint16(uint16_t value) {
  __ENTER_FUNCTION
    uint32_t count = write((char*)&value, sizeof(value));
    bool result = count == sizeof(value) ? true : false;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool OutputStream::write_int32(int32_t value) {
  __ENTER_FUNCTION
    uint32_t count = write((char*)&value, sizeof(value));
    bool result = count == sizeof(value) ? true : false;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool OutputStream::write_uint32(uint32_t value) {
  __ENTER_FUNCTION
    uint32_t count = write((char*)&value, sizeof(value));
    bool result = count == sizeof(value) ? true : false;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool OutputStream::write_int64(int64_t value) {
  __ENTER_FUNCTION
    uint32_t count = write((char*)&value, sizeof(value));
    bool result = count == sizeof(value) ? true : false;
    return result;
  __LEAVE_FUNCTION
    return false;
}
   
bool OutputStream::write_uint64(uint64_t value) {
  __ENTER_FUNCTION
    uint32_t count = write((char*)&value, sizeof(value));
    bool result = count == sizeof(value) ? true : false;
    return result;
  __LEAVE_FUNCTION
    return false;
}
   
bool OutputStream::write_string(const char *value) {
  __ENTER_FUNCTION
    uint32_t count = 0;
    uint32_t length = static_cast<uint32_t>(strlen(value));
    bool result = false;
    if (!write_uint32(length)) return false;
    count = write(value, length);
    result = count == sizeof(value) ? true : false;
    return result;
  __LEAVE_FUNCTION
    return false;
}
   
bool OutputStream::write_float(float value) {
  __ENTER_FUNCTION
    uint32_t count = write((char*)&value, sizeof(value));
    bool result = count == sizeof(value) ? true : false;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool OutputStream::write_dobule(double value) {
  __ENTER_FUNCTION
    uint32_t count = write((char*)&value, sizeof(value));
    bool result = count == sizeof(value) ? true : false;
    return result;
  __LEAVE_FUNCTION
    return false;
}

int32_t OutputStream::compressflush() {
  __ENTER_FUNCTION
    if (0 == compressor_.getsize()) return 0;
    uint32_t flushcount = 0;
    uint32_t leftcount = compressor_.getsize();
    uint32_t sendcount = 0;
    uint32_t flag = 0;
#if __LINUX__
    flag = MSG_NOSIGNAL;
#elif __WINDOWS__
    flag = MSG_DONTROUTE;
#endif
    bool totalsend = true;
    if (leftcount > 1024) {
      leftcount = 1024;
      totalsend = false;
    }
    while (leftcount > 0) {
      sendcount = socket_->send(compressor_.getheader(), leftcount, flag);
      if (SOCKET_ERROR_WOULD_BLOCK == static_cast<int32_t>(sendcount)) return 0;
      if (SOCKET_ERROR == static_cast<int32_t>(sendcount)) 
        return SOCKET_ERROR - 12;
      if (0 == sendcount) return 0;
      flushcount += sendcount;
      compressor_.pophead(sendcount);
      leftcount -= sendcount;
    }
    if (!totalsend) return flushcount;
    compressor_.resetposition();
    return flushcount;
  __LEAVE_FUNCTION
    return 0;
}

bool OutputStream::compress(uint32_t tail) {
  __ENTER_FUNCTION
    if (static_cast<uint32_t>(-1) == tail ||
        compressor_.getsize() != 0 ||
        reallength() <= 0) {
      return false;
    }
    uint32_t head = streamdata_.head;
    uint32_t tail = streamdata_.tail;
    uint32_t bufferlength = streamdata_.bufferlength;
    uint32_t bufferlength_max = streamdata_.bufferlength_max;
    if (bufferlength > bufferlength_max) return false;
    compressor_.resetposition();
    char *inbuffer = NULL;
    uint32_t insize = tail - head;
    uint32_t outsize = 0;
    if (insize < NET_SOCKET_COMPRESSOR_SIZE_MIN) return false;
    inbuffer = streamdata_.buffer + head;
    bool compress_result = 
      compressor_.compress(inbuffer, insize, compressor_.getheader(), outsize);
    if (!compress_result) return false;
    if (encrypt_isenable()) compressor_.encrypt();
    streamdata_.head = (head + insize) % bufferlength;
    if (streamdata_.head == streamdata_.tail)
      streamdata_.head = streamdata_.tail = 0;
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint32_t OutputStream::get_floortail() {
  __ENTER_FUNCTION
    uint32_t result = 0;
    uint32_t head = streamdata_.head;
    uint32_t tail = streamdata_.tail;
    uint32_t bufferlength = streamdata_.bufferlength;
    uint32_t bufferlength_max = streamdata_.bufferlength_max;
    if (bufferlength > bufferlength_max) {
      result = static_cast<uint32_t>(SOCKET_ERROR - 21);
      return result;
    }
    if (compressor_.getsize() != 0 || !raw_isempty() || 0 == reallength()) {
      result = static_cast<uint32_t>(SOCKET_ERROR);
      return result;
    }
    uint32_t position = head;
    uint32_t size = 0;
    char *buffer = streamdata_.buffer + position;
    uint32_t sizemax = NET_SOCKET_COMPRESSOR_IN_SIZE;
    if (reallength() < sizemax) return tail;
    //uint16_t last_packetid = static_cast<uint16_t>(-1);
    do {
      uint32_t packetcheck = 0;
      uint32_t packetsize = 0;
      uint16_t packetid = 0;
      if (size + 4 > reallength()) {
        result = static_cast<uint32_t>(SOCKET_ERROR - 17);
        return result;
      }
      if (encrypt_isenable()) {
        encryptor_.decrypt(&packetid, buffer, sizeof(packetid));
        encryptor_.decrypt(&packetcheck, 
                           buffer + sizeof(packetid), 
                           sizeof(packetcheck));
      } else {
        memcpy(&packetid, buffer, sizeof(packetid));
        memcpy(&packetcheck, buffer + sizeof(packetid), sizeof(packetcheck));
      }
      packetsize = NET_PACKET_GETLENGTH(packetcheck);
      if (packetsize > sizemax) {
        result = static_cast<uint32_t>(SOCKET_ERROR - 15);
        return result;
      }
      uint32_t full_packetsize = 
        sizeof(packetid) + sizeof(packetcheck) + packetsize;
      if (size + full_packetsize > bufferlength) {
        result = static_cast<uint32_t>(SOCKET_ERROR - 16);
        return result;
      }
      if (size + full_packetsize > sizemax) {
        break;
      }
      size += full_packetsize;
      position = (position + full_packetsize) % bufferlength;
      buffer = streamdata_.buffer + position;
      //last_packetid = packetid;
    } while(true);
    return position;
  __LEAVE_FUNCTION
    return static_cast<uint32_t>(SOCKET_ERROR);
}

void OutputStream::rawprepare(uint32_t tail) {
  __ENTER_FUNCTION
    if (0 == compressor_.getsize() && 
        raw_isempty() && 
        tail != static_cast<uint32_t>(-1)) {
      tail_ = tail;
    }
  __LEAVE_FUNCTION
}

bool OutputStream::raw_isempty() const {
  return tail_ == streamdata_.head;
}

int32_t OutputStream::rawflush() {
  __ENTER_FUNCTION
    if (compressor_.getsize() != 0 || raw_isempty()) return 0;
    uint32_t flushcount = 0;
    uint32_t sendcount = 0;
    uint32_t leftcount = 0;
    uint32_t result = 0;
    uint32_t flag = 0;
#if __LINUX__
    flag = MSG_NOSIGNAL;
#elif __WINDOWS__
    flag = MSG_DONTROUTE;
#endif
    if (streamdata_.bufferlength > streamdata_.bufferlength_max) {
      result = static_cast<uint32_t>(SOCKET_ERROR - 11);
      return result;
    }
    bool totalsend = true;
    if (streamdata_.head < tail_) {
      leftcount = tail_ - streamdata_.head;
      if (leftcount > 1024) {
        leftcount = 1024;
        totalsend = false;
      }
      while (leftcount > 0) {
        sendcount = 
          socket_->send(&streamdata_.buffer[streamdata_.head], leftcount, flag);
        if (SOCKET_ERROR_WOULD_BLOCK == static_cast<int32_t>(sendcount)) 
          return 0;
        if (SOCKET_ERROR == static_cast<int32_t>(sendcount)) 
          return SOCKET_ERROR - 12;
        if (0 == sendcount) return 0;
        flushcount += sendcount;
        leftcount -= sendcount;
        streamdata_.head += sendcount;
      }
      if (!totalsend) return flushcount;
    } else if (streamdata_.head > tail_) {
      leftcount = streamdata_.bufferlength - streamdata_.head;
      if (leftcount > 1024) {
        leftcount = 1024;
        totalsend = false;
      }
      while (leftcount > 0) {
        sendcount = 
          socket_->send(&streamdata_.buffer[streamdata_.head], leftcount, flag);
        if (SOCKET_ERROR_WOULD_BLOCK == static_cast<int32_t>(sendcount)) 
          return 0;
        if (SOCKET_ERROR == static_cast<int32_t>(sendcount)) 
          return SOCKET_ERROR - 12;
        if (0 == sendcount) return 0;
        flushcount += sendcount;
        leftcount -= sendcount;
        streamdata_.head += sendcount;
      }
      if (!totalsend) {
        return flushcount;
      } else {
        streamdata_.head = 0;
      }
    }
    if (streamdata_.head == streamdata_.tail)
      streamdata_.head = streamdata_.tail = tail_ = 0;
    return flushcount;
  __LEAVE_FUNCTION
    return 0;
}

void OutputStream::compressenable(bool enable) {
  __ENTER_FUNCTION
    Stream::compressenable(enable);
    if (compressor_.getassistant()->isenable())
      compressor_.alloc(NET_SOCKET_COMPRESSOR_OUT_SIZE);
  __LEAVE_FUNCTION
}

} //namespace socket

} //namespace pf_net
