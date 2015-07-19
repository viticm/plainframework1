#include "pf/base/util.h"
#include "pf/base/time_manager.h"
#include "pf/engine/thread/net.h"

using namespace pf_engine::thread;

Net::Net() {
  //do nothing    
}

Net::~Net() {
}

bool Net::init(uint16_t connectionmax, 
               uint16_t listenport, 
               const char *listenip) {
  __ENTER_FUNCTION
    using namespace pf_net;
    isactive_ = Manager::init(connectionmax, listenport, listenip);
    return isactive_;
  __LEAVE_FUNCTION
    return false;
}

void Net::run() {
  __ENTER_FUNCTION
    while (isactive()) {
      uint32_t runtime = TIME_MANAGER_POINTER->get_tickcount();
      tick(); //Ñ­»·Âß¼­
      int32_t waittime =
        runtime +
        static_cast<uint32_t>(1000 / NET_MANAGER_FRAME) -
        TIME_MANAGER_POINTER->get_tickcount();
      if (waittime > 0) pf_base::util::sleep(waittime);
    }
  __LEAVE_FUNCTION
}

void Net::stop() {
  isactive_ = false;
}

void Net::quit() {
  //do nothing
}

bool Net::isactive() const {
  return isactive_;
}
