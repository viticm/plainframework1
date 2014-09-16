#include "pf/engine/thread/net.h"

using namespace pf_engine::thread;

Net::Net() {
  //do nothing    
}

Net::~Net() {
  //do nothing
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
    using namespace pf_net;
    if (isactive()) Manager::loop();
  __LEAVE_FUNCTION
}

void Net::stop() {
  isactive_ = false;
  Manager::setactive(false);
}

void Net::quit() {
  //do nothing
}

bool Net::isactive() const {
  return isactive_;
}
