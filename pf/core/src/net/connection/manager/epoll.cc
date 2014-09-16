#include "pf/base/log.h"
#include "pf/base/util.h"
#include "pf/net/connection/manager/epoll.h"

#if __LINUX__ && defined(_PF_NET_EPOLL)

namespace pf_net {

namespace connection {

namespace manager {

Epoll::Epoll() {
  __ENTER_FUNCTION
    polldata_.fd = ID_INVALID;
    polldata_.maxcount = 0;
    polldata_.result_eventcount = 0;
    polldata_.event_index = 0;
  __LEAVE_FUNCTION
}

Epoll::~Epoll() {
  __ENTER_FUNCTION
    poll_destory(polldata_);
  __LEAVE_FUNCTION
}

bool Epoll::init(uint16_t connectionmax,
                 uint16_t listenport,
                 const char *listenip) {
  __ENTER_FUNCTION
    if (!Base::init(connectionmax, listenport, listenip)) return false;
    if (is_servermode_) {
      poll_add(polldata_, socketid_, EPOLLIN, ID_INVALID);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Epoll::select() {
  __ENTER_FUNCTION
    int32_t result = SOCKET_ERROR;
    try {
      poll_wait(polldata_, 0);
      if (polldata_.result_eventcount > polldata_.maxcount || 
          polldata_.result_eventcount < 0) {
        char message[128] = {0};
        snprintf(message, 
                 sizeof(message) - 1, 
                 "Epoll::select error, result event count: %d",
                 polldata_.result_eventcount);
        AssertEx(false, message);
      }
    } catch(...) {
      FAST_ERRORLOG(kNetLogFile, 
                    "[net.connection.manager] (Epoll::select)"
                    " have error, result: %d", 
                    result);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Epoll::set_poll_maxcount(uint16_t maxcount) {
  __ENTER_FUNCTION
    if (polldata_.fd > 0) return true;
    bool result = 0 < poll_create(polldata_, maxcount) ? true : false;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Epoll::addsocket(int32_t socketid, int16_t connectionid) {
  __ENTER_FUNCTION
    if (fdsize_ > polldata_.maxcount) {
      Assert(false);
      return false;
    }
    Assert(SOCKET_INVALID != socketid);
    if (poll_add(polldata_, socketid, EPOLLIN | EPOLLET, connectionid) != 0) {
      SLOW_ERRORLOG(NET_MODULENAME, 
                    "[net.connection.manager] (Epoll::addsocket)"
                    " error, message: %s", 
                    strerror(errno));
      return false;
    }
    ++fdsize_;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Epoll::removesocket(int32_t socketid) {
  __ENTER_FUNCTION
    if (SOCKET_INVALID == socketid) {
      SLOW_ERRORLOG(NET_MODULENAME,
                    "[net.connection.manager] (Epoll::removesocket) error!"
                    " SOCKET_INVALID == socketid");
      return false;
    }
    if (socketid <= 0) {
      SLOW_ERRORLOG(NET_MODULENAME,
                    "[net.connection.manager] (Epoll::removesocket) error!"
                    "socketid(%d) <= 0", socketid);
      return false;
    }
    if (poll_delete(polldata_, socketid) != 0) {
      SLOW_ERRORLOG(NET_MODULENAME,
                    "[net.connection.manager] (Epoll::removesocket) error!"
                    " message: %s",
                    strerror(errno));
      return false;
    }
    --fdsize_;
    Assert(fdsize_ >= 0);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Epoll::processinput() {
  __ENTER_FUNCTION
    using namespace pf_base;
    uint16_t i;
    for (i = 0; i < polldata_.result_eventcount; ++i) {
      int32_t socketid = static_cast<int32_t>(
          util::get_highsection(polldata_.events[i].data.u64));
      int16_t connectionid = static_cast<int16_t>(
          util::get_lowsection(polldata_.events[i].data.u64));
      //接受新连接的时候至少尝试两次，所以连接池里会多创建一个
      if (socketid == socketid_) {
        for (i = 0; i < onestep_accept_; ++i) {
          if (!accept()) break;
        }
      } else if (polldata_.events[i].events & EPOLLIN) {
        connection::Base *connection = NULL;
        if (ID_INVALID == connectionid) {
          SLOW_WARNINGLOG(NET_MODULENAME, 
                          "[net.connection.manager] (Epoll::processinput)"
                          " ID_INVALID == connectionid");
          continue;
        }
        connection = Base::get(connectionid);
        if (NULL == connection) {
          SLOW_WARNINGLOG(NET_MODULENAME, 
                          "[net.connection.manager] (Epoll::processinput)"
                          " NULL == connection, id: %d", connectionid);
          continue;
        }
        if (connection->isdisconnect()) continue;
        int32_t _socketid = connection->getsocket()->getid();
        if (SOCKET_INVALID == _socketid) {
          SLOW_ERRORLOG(NET_MODULENAME,
                        "[net.connection.manager] (Epoll::processinput)"
                        " error! _socketid == SOCKET_INVALID, connectionid: %d",
                        connectionid);
          return false;
        }
        if (connection->getsocket()->iserror()) {
          Base::remove(connection);
        } else {
          try {
            if (!connection->processinput()) { 
              Base::remove(connection);
            } else {
              receive_bytes_ += connection->get_receive_bytes();
            }
          } catch(...) {
            Base::remove(connection);
          }
        }
      } //handle the epoll input event
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Epoll::processoutput() {
  __ENTER_FUNCTION
    uint16_t i;
    uint16_t connectioncount = Base::getcount();
    for (i = 0; i < connectioncount; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      connection::Base* connection = NULL;
      connection = pool_->get(connection_idset_[i]);
      Assert(connection);
      int32_t socketid = connection->getsocket()->getid();
      if (socketid_ == socketid) continue;
      if (connection->getsocket()->iserror()) {
        Base::remove(connection);
      } else {
        try {
          if (!connection->processoutput()) { 
            Base::remove(connection);
          } else {
            send_bytes_ += connection->get_send_bytes();
          }
        } catch(...) {
          Base::remove(connection);
        }
      } //connection->getsocket()->iserror()
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Epoll::processexception() {
  __ENTER_FUNCTION
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Epoll::processcommand() {
  __ENTER_FUNCTION
    uint16_t i;
    uint16_t connectioncount = Base::getcount();
    for (i = 0; i < connectioncount; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      connection::Base* connection = NULL;
      connection = pool_->get(connection_idset_[i]);
      Assert(connection);
      if (connection->isdisconnect()) continue;
      int32_t socketid = connection->getsocket()->getid();
      if (socketid_ == socketid) continue;
      if (connection->getsocket()->iserror()) {
        Base::remove(connection);
      } else { //connection is ok
        try {
          if (!connection->processcommand(false)) 
            Base::remove(connection);
        } catch(...) {
          Base::remove(connection);
        }
      } //connection->getsocket()->iserror()
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Epoll::heartbeat(uint32_t time) {
  __ENTER_FUNCTION
    bool result = Base::heartbeat(time);
    return result;
  __LEAVE_FUNCTION
    return false;
}

} //namespace manager

} //namespace connection

} //namespace pf_net

#endif
