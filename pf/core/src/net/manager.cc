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
    performance_active_ = false;
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
    uint32_t _time = 0 == time ? g_time_manager->get_tickcount() : time;
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
    uint32_t tickcount = TIME_MANAGER_POINTER->get_tickcount();
    static uint32_t last_tickcount = 0;
    try {
      result = select();
      Assert(result);

      result = processexception();
      Assert(result);

      result = processinput();
      Assert(result);

      result = processoutput();
      Assert(result); 
      if (PERFORMANCE_EYES_POINTER && 
          tickcount - last_tickcount > 1000 &&
          true == performance_active_) { //网络性能监视
        last_tickcount = tickcount;
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
      result = process_cachecommand();
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

void Manager::set_performance_active(bool active) {
  performance_active_ = active;
}

void Manager::broadcast(packet::Base *packet, int32_t status) {
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
      int32_t _status = static_cast<int32_t>(connection->getstatus());
      if (status != -1 && status != _status) continue;
      connection->sendpacket(packet);
    }
  __LEAVE_FUNCTION
}

} //namespace pf_net
