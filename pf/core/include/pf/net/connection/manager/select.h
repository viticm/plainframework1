/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id select.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/17 10:56
 * @uses connection manager with select mode
 */
#ifndef PF_NET_CONNECTION_MANAGER_SELECT_H_
#define PF_NET_CONNECTION_MANAGER_SELECT_H_

#if !(__LINUX__ && defined(_PF_NET_EPOLL)) && \
  !(__WINDOWS__ && defined(_PF_NET_IOCP))
#include "pf/net/connection/manager/config.h"
#include "pf/net/connection/manager/base.h"

namespace pf_net {

namespace connection {

namespace manager {

class PF_API Select : public Base {

 public:
   Select();
   ~Select();

 public:
   virtual bool init(uint16_t connectionmax = NET_CONNECTION_MAX,
                     uint16_t listenport = 0,
                     const char *listenip = NULL);
   virtual bool select(); //网络侦测
   virtual bool processinput(); //数据接收接口
   virtual bool processoutput(); //数据发送接口
   virtual bool processexception(); //异常连接处理
   virtual bool processcommand(); //消息执行
   virtual bool set_poll_maxcount(uint16_t maxcount);
   virtual bool heartbeat(uint32_t time = 0);

 public:
   //增加连接socket
   virtual bool addsocket(int32_t socketid, int16_t connectionid);
   //将拥有fd句柄的玩家(服务器)数据从当前系统中清除
   virtual bool removesocket(int32_t socketid);

 protected:
  //网络相关数据
   enum {
     kSelectFull = 0, //当前系统中拥有的完整句柄数据
     kSelectUse, //用于select调用的句柄数据
     kSelectMax,
   };
   fd_set readfds_[kSelectMax];
   fd_set writefds_[kSelectMax];
   fd_set exceptfds_[kSelectMax];
   timeval timeout_[kSelectMax];
   int32_t maxfd_;
   int32_t minfd_;

};

}; //namespace manager

}; //namespace connection

}; //namespace pf_net
#endif

#endif //PF_NET_CONNECTION_MANAGER_SELECT_H_
