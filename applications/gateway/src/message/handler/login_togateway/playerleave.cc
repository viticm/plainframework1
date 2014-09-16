#include "common/net/packet/login_togateway/playerleave.h"

using namespace common::net::packet::login_togateway;

uint32_t PlayerLeaveHandler::execute(
    PlayerLeave *packet,
    pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    USE_PARAM(connection);
    USE_PARAM(packet);
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
