/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id extend.inl
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/19 15:23
 * @uses for socket extend inline functions(epoll or icop)
 */
#ifndef PF_NET_SOCKET_EXTEND_INL_
#define PF_NET_SOCKET_EXTEND_INL_

#include "pf/file/api.h"
#include "pf/base/util.h"
#include "pf/base/log.h"
#if __LINUX__
#include <sys/epoll.h>
#include <poll.h>
#endif

#if __LINUX__ /* { */
typedef struct {
  enum {
    kEventIn = EPOLLIN,
    kEventOut = EPOLLOUT,
    kEventError = EPOLLERR
  };
  int32_t fd;
  int32_t maxcount;
  int32_t result_eventcount;
  int32_t event_index;
  struct epoll_event *events;
} polldata_t;
#endif /* } */

#if __LINUX__ /* { */

inline int32_t poll_create(polldata_t& polldata, int32_t maxcount) {
  int32_t fd = epoll_create(maxcount);
  if (fd > 0) {
    polldata.fd = fd;
    polldata.maxcount = maxcount;
    polldata.events = new epoll_event[maxcount];
    Assert(polldata.events);
    signal(SIGPIPE, SIG_IGN);
  }
  return fd;
}

inline int32_t poll_add(polldata_t& polldata, 
                        int32_t fd, 
                        int32_t mask, 
                        int16_t connectionid) {
  struct epoll_event _epoll_event;
  memset(&_epoll_event, 0, sizeof(_epoll_event));
  _epoll_event.events = mask;
  _epoll_event.data.u64 = pf_base::util::touint64(
      static_cast<uint32_t>(fd), static_cast<uint32_t>(connectionid));
  int32_t result = epoll_ctl(polldata.fd, EPOLL_CTL_ADD, fd, &_epoll_event);
  return result;
}

inline int32_t poll_mod(polldata_t& polldata, int32_t fd, int32_t mask) {
  struct epoll_event _epoll_event;
  memset(&_epoll_event, 0, sizeof(_epoll_event));
  _epoll_event.events = mask;
  _epoll_event.data.fd = fd;
  int32_t result = epoll_ctl(polldata.fd, EPOLL_CTL_MOD, fd, &_epoll_event);
  return result;
}

inline int32_t poll_delete(polldata_t& polldata, int32_t fd) {
  struct epoll_event _epoll_event;
  memset(&_epoll_event, 0, sizeof(_epoll_event));
  _epoll_event.events = EPOLLIN | EPOLLET;
  _epoll_event.data.fd = fd;
  int32_t result = epoll_ctl(polldata.fd, EPOLL_CTL_DEL, fd, &_epoll_event);
  return result;
}

inline int32_t poll_wait(polldata_t& polldata, int32_t timeout) {
  Assert(polldata.events);
  Assert(polldata.maxcount > 0);
  polldata.result_eventcount = epoll_wait(polldata.fd, 
                                          polldata.events, 
                                          polldata.maxcount, 
                                          timeout);
  polldata.event_index = 0;
  return polldata.result_eventcount;
}

inline int32_t poll_destory(polldata_t& polldata) {
  pf_file::api::closeex(polldata.fd);
  SAFE_DELETE_ARRAY(polldata.events);
  return 0;
}

inline int32_t poll_event(polldata_t& polldata, 
                          int32_t* fd, 
                          int32_t * events) {
  if (polldata.event_index < polldata.result_eventcount) {
    struct epoll_event& _epoll_event = 
      polldata.events[polldata.event_index++];
    *events = _epoll_event.events;
    *fd = _epoll_event.data.fd;
    return 0;
  }
  return -1;
}

#endif /* } */

#endif //PF_NET_SOCKET_EXTEND_INL_
