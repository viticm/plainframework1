/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id dynamic.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/08 15:56
 * @uses net dynamic packet class
 *       cn: 这个是动态的网络包，数据不规则，可以使用在任何地方
 *           但强烈建议不要使用在C++中，可以使用在脚本内，动态的包虽然方便，
 *           但是不安全，也同时存在性能上的消耗
 *
 *       test time: 2015-4-9 18:26:41
 */
#ifndef PF_NET_PACKET_DYNAMIC_H_
#define PF_NET_PACKET_DYNAMIC_H_

#include "pf/net/packet/config.h"
#include "pf/net/packet/base.h"
#include "pf/sys/memory/dynamic_allocator.h"

namespace pf_net {

namespace packet {

class PF_API Dynamic : public packet::Base {

 public:
   Dynamic();
   Dynamic(uint16_t id); //如果使用这个构造，默认为可以写的包
   virtual ~Dynamic();

 public:
   void write(const char *buffer, uint32_t length);
   void read(char *buffer, uint32_t length);
   void set_readable(bool flag);
   void set_writeable(bool flag);
   void cleanup();

 public:
   virtual void setid(uint16_t id);
   virtual void setsize(uint32_t size);

 public:
   virtual bool read(socket::InputStream &inputstream);
   virtual bool write(socket::OutputStream &outputstream);
   virtual uint16_t getid() const;
   virtual uint32_t getsize() const;


 public: //write_*常用方法
   void write_int8(int8_t value);
   void write_uint8(uint8_t value);
   void write_int16(int16_t value);
   void write_uint16(uint16_t value);
   void write_int32(int32_t value);
   void write_uint32(uint32_t value);
   void write_int64(int64_t value);
   void write_uint64(uint64_t value);
   void write_string(const char *value);
   void write_float(float value);
   void write_double(double value);

 public:
   int8_t read_int8();
   uint8_t read_uint8();
   int16_t read_int16();
   uint16_t read_uint16();
   int32_t read_int32();
   uint32_t read_uint32();
   int64_t read_int64();
   uint64_t read_uint64();
   void read_string(char *buffer, size_t size);
   float read_float();
   double read_double();

 protected:
   void checkmemory(uint32_t length);

 private:
   pf_sys::memory::DynamicAllocator allocator_; //内存分配
   uint32_t offset_; //动态包写入或读取到的位置
   uint32_t size_; //包的大小
   bool readable_; //是否可读
   bool writeable_; //是否可写
   uint16_t id_; //包ID

};

}; //namespace packet

}; //namespace pf_net

#endif //PF_NET_PACKET_DYNAMIC_H_
