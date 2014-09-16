#include "pf/net/socket/encode.h"
#include "pf/net/socket/outputstream.h"

namespace pf_net {

namespace socket {

uint32_t OutputStream::write(const char *buffer, uint32_t length) {
  __ENTER_FUNCTION
    /**
     * tail head       head tail --length 10
     * 0123456789      0123456789
     * abcd...efg      ...abcd...
     */
    uint32_t headlength = streamdata_.headlength;
    uint32_t taillength = streamdata_.taillength;
    uint32_t bufferlength = streamdata_.bufferlength;
    uint32_t freecount = headlength <= taillength ? 
                         bufferlength - taillength + headlength - 1 : 
                         headlength - taillength - 1;
    if (length >= freecount && !resize(length - freecount + 1)) return 0;
    unsigned char *tempbuffer = new unsigned char[length];
    if (NULL == tempbuffer) return 0;
    memset(tempbuffer, 0, length);
    if (encodeparam_.keysize > 0) {
      bool encode_result = true;
      encodeparam_.in = 
        reinterpret_cast<unsigned char *>(const_cast<char *>(buffer));
      encodeparam_.insize = length;
      encodeparam_.out = tempbuffer;
      encodeparam_.outsize = length;
      encode_result = encode::make(encodeparam_);
      if (!encode_result) {
        SAFE_DELETE_ARRAY(tempbuffer);
        return 0;
      }
    } else {
      memcpy(tempbuffer, buffer, length);
    }
    if (headlength <= taillength && 
        taillength != 0 && 
        (freecount = bufferlength - taillength) <= length) {
      memcpy(&(streamdata_.buffer[taillength]), tempbuffer, freecount);
      memcpy(streamdata_.buffer, &tempbuffer[freecount], length - freecount);
    } else {
      memcpy(&(streamdata_.buffer[taillength]), tempbuffer, length);
    }
    streamdata_.taillength = (taillength + length) % bufferlength;
    SAFE_DELETE_ARRAY(tempbuffer);
    return length;
  __LEAVE_FUNCTION
    return 0;
}

bool OutputStream::writepacket(const packet::Base* packet) {
  __ENTER_FUNCTION
    bool result = false;
    uint16_t packetid = packet->getid();
    uint32_t writecount = 0;
    uint32_t packetcheck; //index and size(if diffrent then have error)
    //write packetid
    writecount = 
      write(reinterpret_cast<const char *>(&packetid), sizeof(packetid));
    uint32_t packetsize = packet->getsize();
    uint32_t packetindex = packet->getindex();
    SET_PACKETINDEX(packetcheck, packetindex);
    SET_PACKETLENGTH(packetcheck, packetsize);
    writecount = write(reinterpret_cast<const char *>(&packetcheck), 
                       sizeof(packetcheck));
    result = packet->write(*this);
    return result;
  __LEAVE_FUNCTION
    return false;
}

int32_t OutputStream::flush() {
  __ENTER_FUNCTION
    if (!socket_->isvalid()) return 0;
    uint32_t flushcount = 0;
    int32_t sendcount = 0;
    uint32_t leftcount = 0;
    uint32_t flag = 0;
    uint32_t bufferlength = streamdata_.bufferlength;
    uint32_t bufferlength_max = streamdata_.bufferlength_max;
    uint32_t headlength = streamdata_.headlength;
    uint32_t taillength = streamdata_.taillength;
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
    leftcount = headlength < taillength ? 
                taillength - headlength : 
                bufferlength - headlength;
    while (leftcount > 0) {
      sendcount = socket_->send(&stream_buffer[headlength], leftcount, flag);
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
      streamdata_.headlength += sendcount;
    }
    if (headlength > taillength) {
      streamdata_.headlength = 0;
      leftcount = taillength;
      while (leftcount > 0) {
        sendcount = socket_->send(&stream_buffer[headlength], leftcount, flag);
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
        streamdata_.headlength += sendcount;
      }
    }
    streamdata_.headlength = streamdata_.taillength = 0;
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

} //namespace socket

} //namespace pf_net
