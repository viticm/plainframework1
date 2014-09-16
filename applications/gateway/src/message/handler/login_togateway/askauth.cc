#include "message/delegator/account.h"
#include "common/net/packet/login_togateway/askauth.h"

using namespace common::net::packet::login_togateway;

uint32_t AskAuthHandler::execute(AskAuth *packet, 
                                 pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    message::delegator::account::auth(packet, connection); 
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
