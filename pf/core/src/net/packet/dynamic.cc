#include "pf/base/log.h"
#include "pf/net/packet/factorymanager.h"
#include "pf/net/packet/dynamic.h"

namespace pf_net {

namespace packet {

Dynamic::Dynamic() {
  __ENTER_FUNCTION
    cleanup();
  __LEAVE_FUNCTION
}


Dynamic::Dynamic(uint16_t id) {
  __ENTER_FUNCTION
    cleanup();
    setid(id);
    set_writeable(true);
  __LEAVE_FUNCTION
}

Dynamic::~Dynamic() {
  //do nothing
}

void Dynamic::cleanup() {
  __ENTER_FUNCTION
    allocator_.malloc(2048);
    offset_ = 0;
    size_ = 0;
    readable_ = false;
    writeable_ = false;
    id_ = 0;
  __LEAVE_FUNCTION
}

void Dynamic::setsize(uint32_t size) {
  size_ = size;
}

uint32_t Dynamic::getsize() const {
  return size_;
}

void Dynamic::write(const char *buffer, uint32_t length) {
  __ENTER_FUNCTION
    if (!writeable_) return;
    //(length - (allocator_.getsize() - offset_)) + allocator_.getsize();
    uint32_t checklength = length + offset_;
    checkmemory(checklength);
    char *_buffer = reinterpret_cast<char *>(allocator_.getpointer()) + offset_;
    memcpy(_buffer, buffer, length);
    offset_ += length;
    size_ += length;
  __LEAVE_FUNCTION
}

void Dynamic::read(char *buffer, uint32_t length) {
  __ENTER_FUNCTION
    if (!readable_) return;
    if (0 == size_ || offset_ >= size_ - 1) return;
    char *_buffer = reinterpret_cast<char *>(allocator_.getpointer()) + offset_;
    memcpy(buffer, _buffer, length);
    offset_ += length;
  __LEAVE_FUNCTION
}

void Dynamic::write_int8(int8_t value) {
  __ENTER_FUNCTION
    write((char *)&value, sizeof(value));
  __LEAVE_FUNCTION
}
   
void Dynamic::write_uint8(uint8_t value) {
  __ENTER_FUNCTION
    write((char *)&value, sizeof(value));
  __LEAVE_FUNCTION
}
   
void Dynamic::write_int16(int16_t value) {
  __ENTER_FUNCTION
    write((char *)&value, sizeof(value));
  __LEAVE_FUNCTION
}
  
void Dynamic::write_uint16(uint16_t value) {
  __ENTER_FUNCTION
    write((char *)&value, sizeof(value));
  __LEAVE_FUNCTION
}
   
void Dynamic::write_int32(int32_t value) {
  __ENTER_FUNCTION
    write((char *)&value, sizeof(value));
  __LEAVE_FUNCTION
}
   
void Dynamic::write_uint32(uint32_t value) {
  __ENTER_FUNCTION
    write((char *)&value, sizeof(value));
  __LEAVE_FUNCTION
}
   
void Dynamic::write_int64(int64_t value) {
  __ENTER_FUNCTION
    write((char *)&value, sizeof(value));
  __LEAVE_FUNCTION
}
   
void Dynamic::write_uint64(uint64_t value) {
  __ENTER_FUNCTION
    write((char *)&value, sizeof(value));
  __LEAVE_FUNCTION
}
   
void Dynamic::write_string(const char *value) {
  __ENTER_FUNCTION
    uint32_t length = static_cast<uint32_t>(strlen(value));
    write_uint32(length);
    write(value, length);
  __LEAVE_FUNCTION
}
   
void Dynamic::write_float(float value) {
  __ENTER_FUNCTION
    write((char *)&value, sizeof(value));
  __LEAVE_FUNCTION
}
   
void Dynamic::write_double(double value) {
  __ENTER_FUNCTION
    write((char *)&value, sizeof(value));
  __LEAVE_FUNCTION
}

int8_t Dynamic::read_int8() {
  __ENTER_FUNCTION
    int8_t result = 0;
    read((char *)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
uint8_t Dynamic::read_uint8() {
  __ENTER_FUNCTION
    uint8_t result = 0;
    read((char *)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}

int16_t Dynamic::read_int16() {
  __ENTER_FUNCTION
    int16_t result = 0;
    read((char *)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
uint16_t Dynamic::read_uint16() {
  __ENTER_FUNCTION
    uint16_t result = 0;
    read((char *)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}

int32_t Dynamic::read_int32() {
  __ENTER_FUNCTION
    int32_t result = 0;
    read((char *)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
uint32_t Dynamic::read_uint32() {
  __ENTER_FUNCTION
    uint32_t result = 0;
    read((char *)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
int64_t Dynamic::read_int64() {
  __ENTER_FUNCTION
    int64_t result = 0;
    read((char *)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
uint64_t Dynamic::read_uint64() {
  __ENTER_FUNCTION
    uint64_t result = 0;
    read((char *)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
void Dynamic::read_string(char *buffer, size_t size) {
  __ENTER_FUNCTION
    uint32_t length = read_uint32();
    if (length <= 0 || size < length) return;
    read(buffer, length);
  __LEAVE_FUNCTION
}

float Dynamic::read_float() {
  __ENTER_FUNCTION
    float result = .0f;
    read((char *)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return .0f;
}
   
double Dynamic::read_double() {
  __ENTER_FUNCTION
    double result = .0;
    read((char *)&result, sizeof(result));
    return result;
  __LEAVE_FUNCTION
    return .0;
}

void Dynamic::checkmemory(uint32_t length) {
  __ENTER_FUNCTION
    if (length > NET_PACKET_DYNAMIC_SIZEMAX) {
      SLOW_ERRORLOG(NET_MODULENAME,
                    "[net.packet] (Dynamic::checkmemory) length out limit"
                    " (%d, %d)",
                    length,
                    NET_PACKET_DYNAMIC_SIZEMAX);
      return;
    }
    while (length >= allocator_.getsize()) {
      //注意这里的length是指要写入完整数据拥有的最小的长度
      allocator_.malloc(allocator_.getsize() + NET_PACKET_DYNAMIC_ONCESIZE);
    }
  __LEAVE_FUNCTION
}

void Dynamic::set_readable(bool flag) {
  readable_ = flag;
}

void Dynamic::set_writeable(bool flag) {
  writeable_ = flag;
}

bool Dynamic::read(socket::InputStream &inputstream) {
  __ENTER_FUNCTION
    checkmemory(size_);
    char *_buffer = reinterpret_cast<char *>(allocator_.getpointer());
    memset(_buffer, 0, allocator_.getsize());
    uint32_t size = inputstream.read(_buffer, size_);
    bool result = size == size_;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Dynamic::write(socket::OutputStream &outputstream) {
  __ENTER_FUNCTION
    //DEBUGPRINTF("Dynamic::write size: %d", size_);
    if (size_ <= 0 || 0 == id_) return false;
    char *_buffer = reinterpret_cast<char *>(allocator_.getpointer());
    uint32_t size = outputstream.write(_buffer, size_);
    bool result = size == size_;
    return result;
  __LEAVE_FUNCTION
    return false;
}

uint16_t Dynamic::getid() const {
  return id_;
}

void Dynamic::setid(uint16_t id) {
  id_ = id;
}

} //namespace packet

} //namespace pf_net
