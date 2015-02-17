/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id iocp.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/17 17:38
 * @uses the connection iocp manager
 */
#ifndef PF_NET_CONNECTION_MANAGER_IOCP_H_
#define PF_NET_CONNECTION_MANAGER_IOCP_H_

#if __WINDOWS__ && defined(_PF_NET_IOCP)
#include "pf/net/connection/manager/config.h"
#include "pf/net/connection/manager/base.h"
#error iocp connection manager not completed!

namespace pf_net {

namespace connection {

namespace manager {

class PF_API Iocp : public Base {

 public:
   Iocp();
   virtual ~Iocp();

};

}; //namespace manager

}; //namespace connection

}; //namespace pf_net
#endif

#endif //PF_NET_CONNECTION_MANAGER_IOCP_H_
