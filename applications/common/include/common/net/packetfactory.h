/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id packetfactory.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/28 18:05
 * @uses packet factory common functions
 */
#ifndef COMMON_NET_PACKETFACTORY_H_
#define COMMON_NET_PACKETFACTORY_H_

#include "common/config.h"

namespace common {

namespace net {

bool __stdcall registerfactories();
bool __stdcall isvalid_packetid(uint16_t id);
uint16_t get_facctorysize();

}; //namespace net

}; //namespace common

#endif //COMMON_NET_PACKETFACTORY_H_
