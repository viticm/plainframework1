#include "pf/base/log.h"
#include "pf/base/time_manager.h"
#include "pf/base/util.h"
#include "pf/net/packet/factorymanager.h"
#include "pf/performance/eyes.h"
#include "pf/net/manager.h"

bool g_net_stream_usepacket = true;

namespace pf_net {

Manager::Manager() {
  __ENTER_FUNCTION
    setactive(true);
  __LEAVE_FUNCTION
}

Manager::~Manager() {
  //do nothing
}

bool Manager::init(uint16_t connectionmax,
                   uint16_t listenport,
                   const char *listenip) {
  __ENTER_FUNCTION
    bool result = false;
#if __LINUX__ && defined(_PF_NET_EPOLL) /* { */
    result = Epoll::init(connectionmax, listenport, listenip);
#elif __WINDOWS__ && defined(_PF_NET_IOCP) /* }{ */

#else /* }{ */
    result = Select::init(connectionmax, listenport, listenip); 
#endif /* } */
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Manager::heartbeat(uint32_t time) {
  __ENTER_FUNCTION
    uint32_t _time = 0 == time ? g_time_manager->get_current_time() : time;
    uint16_t connectioncount = getcount();
    uint16_t i;
    for (i = 0; i < connectioncount; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      connection::Base* connection = NULL;
      connection = pool_->get(connection_idset_[i]);
      if (NULL == connection) {
        Assert(false);
        return false;
      }
      if (!connection->heartbeat(_time)) {
        remove(connection);
        Assert(false);
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Manager::tick() {
  __ENTER_FUNCTION
      bool result = false;
      try {
        result = select();
        Assert(result);

        result = processexception();
        Assert(result);

        result = processinput();
        Assert(result);

        result = processoutput();
        Assert(result); 
        if (PERFORMANCE_EYES_POINTER) { //网络性能监视
          uint16_t connectioncount = getcount();
          PERFORMANCE_EYES_POINTER->set_onlinecount(connectioncount);
          PERFORMANCE_EYES_POINTER->set_connectioncount(connectioncount);
          uint64_t sendbytes = get_send_bytes();
          uint64_t receivebytes = get_receive_bytes();
          PERFORMANCE_EYES_POINTER->set_sendbytes(sendbytes);
          PERFORMANCE_EYES_POINTER->set_receivebytes(receivebytes);
        }
      } catch(...) {
        
      }
      try {
        result = processcommand();
        Assert(result);
      } catch(...) {
        
      }
      try {
        result = heartbeat();
        Assert(result);
      } catch(...) {

      }
  __LEAVE_FUNCTION
}

void Manager::loop() {
  __ENTER_FUNCTION
    while (isactive()) {
#if __LINUX__ && defined(_PF_NET_EPOLL) /* { */
      //这里有个问题，就是当网络管理器以线程模式运行时会大肆的消耗CPU，
      //如果是不是线程模式则不会出现，所以临时使用了帧率控制的方式解决此问题
      //主要的原因是epoll不是以阻塞的方式来select的
      uint32_t runtime = TIME_MANAGER_POINTER->get_current_time();
      uint32_t waittime = 
        runtime + 
        static_cast<uint32_t>(1000/NET_MANAGER_FRAME) - 
        TIME_MANAGER_POINTER->get_current_time();
#endif /* } */
      tick(); //循环逻辑
#if __LINUX__ && defined(_PF_NET_EPOLL) /* { */
      if (waittime > 0) pf_base::util::sleep(waittime);
#endif /* } */
    }
  __LEAVE_FUNCTION
}

bool Manager::isactive() {
  return active_;
}

void Manager::setactive(bool active) {
  active_ = active;
}

void Manager::broadcast(packet::Base *packet) {
  __ENTER_FUNCTION
    uint16_t connectioncount = getcount();
    uint16_t i;
    for (i = 0; i < connectioncount; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      connection::Base* connection = NULL;
      connection = pool_->get(connection_idset_[i]);
      if (NULL == connection) {
        Assert(false); 
        continue;
      }
      connection->sendpacket(packet);
    }
  __LEAVE_FUNCTION
}

} //namespace pf_net
