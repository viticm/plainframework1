#include "pf/base/log.h"
#include "pf/base/util.h"
#include "pf/net/connection/manager/select.h"

#if !(__LINUX__ && defined(_PF_NET_EPOLL)) && \
  !(__WINDOWS__ && defined(_PF_NET_IOCP))

namespace pf_net {

namespace connection {

namespace manager {

Select::Select() {
  __ENTER_FUNCTION
    FD_ZERO(&readfds_[kSelectFull]);
    FD_ZERO(&writefds_[kSelectFull]);
    FD_ZERO(&exceptfds_[kSelectFull]);
    FD_ZERO(&readfds_[kSelectUse]);
    FD_ZERO(&writefds_[kSelectUse]);
    FD_ZERO(&exceptfds_[kSelectUse]);
    timeout_[kSelectFull].tv_sec = 0;
    timeout_[kSelectFull].tv_usec = 0;
    maxfd_ = minfd_ = SOCKET_INVALID;
  __LEAVE_FUNCTION
}

Select::~Select() {
  //do nothing
}

bool Select::init(uint16_t connectionmax,
                  uint16_t listenport,
                  const char *listenip) {
  __ENTER_FUNCTION
    if (!Base::init(connectionmax, listenport, listenip)) return false;
    if (is_servermode_) {
      FD_SET(socketid_, &readfds_[kSelectFull]);
      FD_SET(socketid_, &exceptfds_[kSelectFull]);
      minfd_ = maxfd_ = socketid_;
      timeout_[kSelectFull].tv_sec = 0;
      timeout_[kSelectFull].tv_usec = 0;
    }
    return true;    
  __LEAVE_FUNCTION
    return false;
}

bool Select::select() {
  __ENTER_FUNCTION
	if (SOCKET_INVALID == minfd_ && SOCKET_INVALID == maxfd_)
      return true; //no connection
    timeout_[kSelectUse].tv_sec = timeout_[kSelectFull].tv_sec;
    timeout_[kSelectUse].tv_usec = timeout_[kSelectFull].tv_usec;
    readfds_[kSelectUse] = readfds_[kSelectFull];
    writefds_[kSelectUse] = writefds_[kSelectFull];
    exceptfds_[kSelectUse] = exceptfds_[kSelectFull];
    int32_t result = SOCKET_ERROR;
    try {
      result = socket::Base::select(
          maxfd_ + 1,
          &readfds_[kSelectUse],
          &writefds_[kSelectUse],
          &exceptfds_[kSelectUse],
          &timeout_[kSelectUse]);
      Assert(result != SOCKET_ERROR);
    } catch(...) {
      FAST_ERRORLOG(NET_MODULENAME, 
                    "[net.connection.manager] (Select::select)"
                    " have error, result: %d", 
                    result);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Select::processinput() {
  __ENTER_FUNCTION
    if (SOCKET_INVALID == minfd_ && SOCKET_INVALID == maxfd_)
      return true; //no connection
    uint16_t i;
    //接受新连接的时候至少尝试两次，所以连接池里会多创建一个
    if (FD_ISSET(socketid_, &readfds_[kSelectUse])) {
      for (i = 0; i < onestep_accept_; ++i) {
        if (!accept()) break;
      }
    }
    uint16_t connectioncount = getcount();
    for (i = 0; i < connectioncount; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      connection::Base *connection = NULL;
      connection = pool_->get(connection_idset_[i]);
      Assert(connection);
      int32_t socketid = connection->getsocket()->getid();
      if (socketid_ == socketid) continue;
      if (FD_ISSET(socketid, &readfds_[kSelectUse])) { //read information
        if (connection->getsocket()->iserror()) {
          remove(connection);
        } else {
          try {
            if (!connection->processinput()) { 
              remove(connection);
            } else {
              receive_bytes_ += connection->get_receive_bytes();
            }
          } catch(...) {
            remove(connection);
          }
        }//connection->getsocket()->iserror()
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Select::processoutput() {
  __ENTER_FUNCTION
    if (SOCKET_INVALID == maxfd_&& SOCKET_INVALID == minfd_)
      return true;
    uint16_t i;
    uint16_t connectioncount = getcount();
    for (i = 0; i < connectioncount; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      connection::Base* connection = NULL;
      connection = pool_->get(connection_idset_[i]);
      Assert(connection);
      int32_t socketid = connection->getsocket()->getid();
      if (socketid_ == socketid) continue;
      if (FD_ISSET(socketid, &writefds_[kSelectUse])) {
        if (connection->getsocket()->iserror()) {
          remove(connection);
        } else {
          try {
            if (!connection->processoutput()) { 
              remove(connection);
            } else {
              send_bytes_ += connection->get_send_bytes();
            }
          } catch(...) {
            remove(connection);
          }
        } //connection->getsocket()->iserror()
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Select::processexception() {
  __ENTER_FUNCTION
    if (SOCKET_INVALID == minfd_ && SOCKET_INVALID == maxfd_)
      return true;
    uint16_t connectioncount = getcount();
    connection::Base* connection = NULL;
    uint16_t i;
    for (i = 0; i < connectioncount; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      connection = pool_->get(connection_idset_[i]);
      Assert(connection);
      int32_t socketid = connection->getsocket()->getid();
      if (socketid_ == socketid) {
        Assert(false);
        continue;
      }
      if (FD_ISSET(socketid, &exceptfds_[kSelectUse])) {
        remove(connection);
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Select::processcommand() {
  __ENTER_FUNCTION
    if (SOCKET_INVALID == maxfd_&& SOCKET_INVALID == minfd_)
      return true;
    uint16_t i;
    uint16_t connectioncount = getcount();
    for (i = 0; i < connectioncount; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      connection::Base* connection = NULL;
      connection = pool_->get(connection_idset_[i]);
      Assert(connection);
      int32_t socketid = connection->getsocket()->getid();
      if (socketid_ == socketid) continue;
      if (connection->getsocket()->iserror()) {
        remove(connection);
      } else { //connection is ok
        try {
          if (!connection->processcommand(false)) 
            remove(connection);
        } catch(...) {
          remove(connection);
        }
      } //connection->getsocket()->iserror()
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Select::addsocket(int32_t socketid, int16_t connectionid) {
  __ENTER_FUNCTION
    USE_PARAM(connectionid);
    if (fdsize_ > FD_SETSIZE) {
      Assert(false);
      return false;
    }
    Assert(SOCKET_INVALID != socketid);
    minfd_ = SOCKET_INVALID == minfd_ ? socketid : min(socketid, minfd_);
    maxfd_ = SOCKET_INVALID == maxfd_ ? socketid : max(socketid, maxfd_);
    FD_SET(socketid, &readfds_[kSelectFull]);
    FD_SET(socketid, &writefds_[kSelectFull]);
    FD_SET(socketid, &exceptfds_[kSelectFull]);
    ++fdsize_;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Select::removesocket(int32_t socketid) {
  __ENTER_FUNCTION
    connection::Base *connection = NULL;
    uint16_t i;
    Assert(minfd_ != SOCKET_INVALID || maxfd_ != SOCKET_INVALID);
    Assert(fdsize_ > 0);
    if (socketid == minfd_) { //the first connection
      int32_t socketid_max = maxfd_;
      uint16_t connectioncount = getcount();
      for (i = 0; i < connectioncount; ++i) {
        if (ID_INVALID == connection_idset_[i]) continue;
        connection = pool_->get(connection_idset_[i]);
        Assert(connection);
        if (NULL == connection) continue;
        int32_t _socketid = connection->getsocket()->getid();
        if (socketid == _socketid || SOCKET_INVALID == _socketid) continue;
        if (socketid_max < _socketid) socketid_max = _socketid;
      }
      if (minfd_ == maxfd_) {
        minfd_ = maxfd_ = SOCKET_INVALID;
      } else {
        minfd_ = socketid_max > socketid_ ? minfd_ : socketid_max;
      }
    } else if (socketid == maxfd_) { //
      int32_t socketid_min = minfd_;
      uint16_t connectioncount = getcount();
      for (i = 0; i < connectioncount; ++i) {
        if (ID_INVALID == connection_idset_[i]) continue;
        connection = pool_->get(connection_idset_[i]);
        Assert(connection);
        if (NULL == connection) continue;
        int32_t _socketid = connection->getsocket()->getid();
        if (socketid == _socketid || SOCKET_INVALID == _socketid) continue;
        if (socketid_min > _socketid) socketid_min = _socketid;
      }
      if (minfd_ == maxfd_) {
        minfd_ = maxfd_ = SOCKET_INVALID;
      } else {
        maxfd_ = socketid_min < socketid_ ? socketid_ : socketid_min;
      }
    }
    FD_CLR(static_cast<uint32_t>(socketid), &readfds_[kSelectFull]);
    FD_CLR(static_cast<uint32_t>(socketid), &readfds_[kSelectUse]);
    FD_CLR(static_cast<uint32_t>(socketid), &writefds_[kSelectFull]);
    FD_CLR(static_cast<uint32_t>(socketid), &writefds_[kSelectUse]);
    FD_CLR(static_cast<uint32_t>(socketid), &exceptfds_[kSelectFull]);
    FD_CLR(static_cast<uint32_t>(socketid), &exceptfds_[kSelectUse]);
    --fdsize_;
    Assert(fdsize_ >= 0);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Select::heartbeat(uint32_t time) {
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
