/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id account.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/10 16:11
 * @uses the gateway message account delegator
 */
#ifndef MESSAGE_DELEGATOR_ACCOUNT_H_
#define MESSAGE_DELEGATOR_ACCOUNT_H_

#include "message/delegator/config.h"
#include "common/net/packet/login_togateway/askauth.h"

namespace message {

namespace delegator {

namespace account {

void auth(common::net::packet::login_togateway::AskAuth *message,
          pf_net::connection::Base* connection);

}; //namespace account

}; //namespace delegator

}; //namespace message

#endif //MESSAGE_DELEGATOR_ACCOUNT_H_
