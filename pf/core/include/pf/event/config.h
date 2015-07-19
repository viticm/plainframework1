/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/29 17:33
 * @uses the config file of event module
 */
#ifndef PF_EVENT_CONFIG_H_
#define PF_EVENT_CONFIG_H_

#include "pf/base/config.h"

namespace pf_event {

typedef struct event_struct event_t;
typedef void (__stdcall *function_eventhandle)
  (const event_t *event, uint32_t ownerdata);

typedef struct eventdefine_struct {
  typedef std::list<std::pair<function_eventhandle, uint32_t> > registerstruct;
  uint16_t id;
  const char *event;
  bool delayprocess;
  registerstruct listenfunction_notify; //监听方法
  eventdefine_struct() {
    id = static_cast<uint16_t>(ID_INVALID);
    event = NULL;
    delayprocess = false;
  };
} eventdefine_t;

typedef struct event_struct {
  eventdefine_t *eventdefine;
  std::vector<std::string> args;
  bool operator == (const event_t &other) {
    if (other.eventdefine != eventdefine) return false;
    if (other.args.size() != args.size()) return false;
    register size_t i;
    for (i = 0; i < args.size(); ++i) {
      if (other.args[i] != args[i]) return false;
    }
    return true;
  }
  event_struct() {
    eventdefine = NULL;
  };
} event_t;

}; //namespace pf_event

#endif //PF_EVENT_CONFIG_H_
