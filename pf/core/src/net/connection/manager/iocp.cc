#include "pf/net/connection/manager/iocp.h"

#if __WINDOWS__ && defined(_PF_NET_IOCP)
namespace pf_net {

namespace connection {

namespace manager {

Iocp::Iocp() {
  //do nothing
}

Iocp::~Iocp() {
  //do nothing
}

} //namespace manager

} //namespace connection

} //namespace pf_net
#endif
