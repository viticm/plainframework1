#include "pf/base/log.h"
#include "common/net/packet/serverserver/connect.h"

using namespace common::net::packet::serverserver;
uint32_t ConnectHandler::execute(Connect* packet, 
                                 pf_net::connection::Base* connection) {
  __ENTER_FUNCTION
    USE_PARAM(connection);
    FAST_LOG(kApplicationLogFile,
             "[gateway][message][handler][serverver]"
             " (ConnectHandler::execute) success"
             " server id: %d",
             packet->get_serverid());
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
