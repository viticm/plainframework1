/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id config.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2013-2013 viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2013-12-31 17:34:43
 * @uses server and client net model base config file
 */
#ifndef PF_NET_CONFIG_H_
#define PF_NET_CONFIG_H_

#include "pf/base/config.h"

#define NET_OVER_SERVER_MAX 256
#define NET_ONESTEP_ACCEPT_DEFAULT 50 //每帧接受新连接的默认值
#define NET_MANAGER_FRAME 100 //网络帧率
#define NET_MANAGER_CACHE_SIZE 1024 //网络管理器默认缓存大小
#define NET_PACKET_FACTORYMANAGER_ALLOCMAX (1024 * 100)

extern bool g_net_stream_usepacket; //true 使用包对象处理 false不使用

namespace pf_net {


namespace packet {

class Base;
class Dynamic;
class Factory;
class FactoryManager;

typedef PF_API struct queue_struct queue_t;
struct queue_struct {
  Base *packet;
  uint16_t connectionid;
  uint32_t flag;
  queue_struct();
  ~queue_struct();
}; //包缓存队列


}; //namespace packet

namespace connection {
class Base;
class Server;
class Pool;

namespace manager {

class Base;
#if __LINUX__ && defined(_PF_NET_EPOLL) /* { */
class Epoll;
#elif __WINDOWS__ && defined(_PF_NET_IOCP) /* }{ */
class Iocp;
#else /* }{ */
class Select;
#endif /* } */

typedef PF_API struct cache_struct cache_t;
struct cache_struct {
  packet::queue_t *queue;
  uint32_t head;
  uint32_t tail;
  uint32_t size;
  cache_struct();
  ~cache_struct();
}; //管理器缓存结构

}; //namespace manager

}; //namespace connection

namespace socket {

class Base;
class Server;
class Stream;
class InputStream;
class OutputStream;
class Encryptor;
class Compressor;

typedef struct streamdata_struct {
  char *buffer;
  uint32_t bufferlength;
  uint32_t bufferlength_max;
  uint32_t head;
  uint32_t tail;
  streamdata_struct() {
    buffer = NULL;
    bufferlength = 0;
    bufferlength_max = 0;
    head = 0;
    tail = 0;
  }
} streamdata_t;

}; //namespace socket

}; //namespace pf_net

#endif //PF_NET_CONFIG_H_
