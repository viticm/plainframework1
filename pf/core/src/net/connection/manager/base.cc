#include "pf/base/log.h"
#include "pf/sys/thread.h"
#include "pf/net/packet/factorymanager.h"
#include "pf/net/connection/manager/base.h"

namespace pf_net {

namespace connection {

namespace manager {

Base::Base() {
  __ENTER_FUNCTION
    threadid_ = 0;
    listenport_ = 0;
    connectionmax_ = NET_CONNECTION_MAX;
    is_servermode_ = true;
    serversocket_ = NULL;
    socketid_ = ID_INVALID;
    fdsize_ = 0;
    connection_idset_ = NULL;
    maxcount_ = 0;
    count_ = 0;
    send_bytes_ = 0;
    receive_bytes_ = 0;
    onestep_accept_ = NET_ONESTEP_ACCEPT_DEFAULT;
    isinit_ = false;
    pool_ = NULL;
  __LEAVE_FUNCTION
}

Base::~Base() {
  __ENTER_FUNCTION
    SAFE_DELETE(pool_);
    SAFE_DELETE(serversocket_);
    SAFE_DELETE_ARRAY(connection_idset_);
  __LEAVE_FUNCTION
}

bool Base::init(uint16_t maxcount, uint16_t listenport, const char *listenip) {
  __ENTER_FUNCTION
    if (isinit()) return true; //有内存分配的请参考此方式避免再次分配内存
    count_ = 0;
    maxcount_ = maxcount;
    connection_idset_ = new int16_t[maxcount_];
    Assert(connection_idset_);
    memset(connection_idset_, ID_INVALID, sizeof(int16_t) * maxcount_);
    if (is_servermode_) {
      listenport_ = listenport;
      serversocket_ = new socket::Server(listenport_, listenip);
      Assert(serversocket_);
      listenport_ = 0 == listenport_ ? serversocket_->getport() : listenport_;
      serversocket_->set_nonblocking();
      socketid_ = serversocket_->getid();
      Assert(socketid_ != SOCKET_INVALID);
      if (g_net_stream_usepacket) {
        if (!NET_PACKET_FACTORYMANAGER_POINTER)
          g_packetfactory_manager = new packet::FactoryManager();
        if (!NET_PACKET_FACTORYMANAGER_POINTER) return false;
        if (!NET_PACKET_FACTORYMANAGER_POINTER->init()) return false;
      }
    }
    threadid_ = pf_sys::get_current_thread_id();
    isinit_ = true;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Base::init_pool(uint16_t connectionmax) {
  __ENTER_FUNCTION
    if (!pool_) return false;
    connectionmax_ = connectionmax;
    if (!pool_->init(connectionmax_)) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Base::init_pool(connection::Pool *pool) {
  if (pool_) {
    SLOW_WARNINGLOG(NET_MODULENAME,
                    "[connection.manager] (Base::init_pool)"
                    " the class pool is init, now will delete it first");
    SAFE_DELETE(pool_);
  }
  pool_ = pool;
}

bool Base::heartbeat(uint32_t time) {
  __ENTER_FUNCTION
    USE_PARAM(time);
    bool result = true;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Base::add(connection::Base *connection) {
  __ENTER_FUNCTION
    Assert(connection);
    if (count_ >= maxcount_) return false;
    //首先处理socket
    if (!addsocket(connection->getsocket()->getid(), connection->getid()))
      return false;
    //再处理管理器ID
    if (ID_INVALID == connection_idset_[count_]) {
      connection_idset_[count_] = connection->getid();
      connection->set_managerid(count_);
      ++count_;
      Assert(count_ <= maxcount_);
    } else {
      Assert(false);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Base::add(int16_t id) {
  __ENTER_FUNCTION
    USE_PARAM(id);
    //not used
    Assert(false);
  __LEAVE_FUNCTION
    return false;
}

bool Base::remove(int16_t id) {
  __ENTER_FUNCTION
    Assert(count_ > 0);
    connection::Base *connection = NULL;
    connection = pool_->get(id);
    if (NULL == connection) {
      Assert(false);
      return false;
    }
    int16_t managerid = connection->get_managerid();
    if (managerid >= static_cast<int16_t>(sizeof(connection_idset_))) {
      Assert(false);
      return false;
    }
    connection = pool_->get(connection_idset_[count_ - 1]);
    if (NULL == connection) {
      Assert(false);
      return false;
    }
    connection_idset_[managerid] = connection_idset_[count_ - 1];
    connection_idset_[count_ - 1] = ID_INVALID;
    connection->set_managerid(managerid);
    --count_;
    Assert(count_ >= 0);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Base::erase(connection::Base* connection) {
  __ENTER_FUNCTION
    //first clean in connection manager
    remove(connection->getid());
    //second remove socket
    removesocket(connection->getsocket()->getid());
    Assert(connection != NULL);
    FAST_LOG(kNetLogFile, 
             "[net.connection.manager] (Base::erase) id: %d", 
             connection->getid());
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Base::remove(connection::Base *connection) {
  __ENTER_FUNCTION
    if (!erase(connection)) return false; 
    pool_->remove(connection->getid());
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Base::destroy() {
  __ENTER_FUNCTION
    uint16_t count = getcount();
    uint16_t i = 0;
    for (i = 0; i < count; ++i) {
      if (ID_INVALID == connection_idset_[i]) {
        SLOW_ERRORLOG(NET_MODULENAME, 
                      "[net.connection.manager] (Base::destroy) error!"
                      " ID_INVALID == connection_idset_[%d]",
                      i);
        continue;
      }
      connection::Base *connection = get(connection_idset_[i]);
      if (NULL == connection) {
        SLOW_ERRORLOG(NET_MODULENAME,
                      "[net.connection.manager]( Base::destroy) error!"
                      " connection is NULL, id: %d",
                      connection_idset_[i]);
        continue;
      }
      remove(connection);
    }
  __LEAVE_FUNCTION
    return false;
}

connection::Base *Base::accept() {
  __ENTER_FUNCTION
    uint32_t step = 0;
    bool result = false;
    connection::Base *newconnection = NULL;
    newconnection = pool_->create();
    if (NULL == newconnection) return NULL;
    step = 5;
    newconnection->init();
    newconnection->cleanup();
    int32_t socketid = SOCKET_INVALID;
    step = 10;
    try {
      //accept client socket
      result = serversocket_->accept(newconnection->getsocket());
      if (!result) {
        step = 15;
        goto EXCEPTION;
      }
    } catch(...) {
      step += 1000;
      goto EXCEPTION;
    }
    try {
      step = 30;
      socketid = newconnection->getsocket()->getid();
      if (SOCKET_INVALID == socketid) {
        Assert(false);
        goto EXCEPTION;
      }
      step = 40;
      result = newconnection->getsocket()->set_nonblocking();
      if (!result) {
        Assert(false);
        goto EXCEPTION;
      }
      step = 50;
      if (newconnection->getsocket()->iserror()) {
        Assert(false);
        goto EXCEPTION;
      }
      step = 60;
      result = newconnection->getsocket()->setlinger(0);
      if (!result) {
        Assert(false);
        goto EXCEPTION;
      }
      step = 70;
      try {
        result = add(newconnection);
        if (!result) {
          Assert(false);
          goto EXCEPTION;
        }
      } catch(...) {
        step += 10000;
        goto EXCEPTION;
      }
    } catch(...) {
      step += 100000;
    }
    FAST_LOG(kNetLogFile,
             "[net.connection.manager] (Base::accept)"
             " host: %s id: %d socketid: %d",
             newconnection->getsocket()->host_,
             newconnection->getid(),
             newconnection->getsocket()->getid());
    return newconnection;
EXCEPTION:
    newconnection->cleanup();
    pool_->remove(newconnection->getid());
    return NULL;
  __LEAVE_FUNCTION
    return NULL;;
}

connection::Base *Base::get(int16_t id) {
  __ENTER_FUNCTION 
    if (id < 0 || id > maxcount_) return NULL;
    connection::Base *connection = NULL;
    connection = pool_->get(id);
    Assert(connection);
    return connection;
  __LEAVE_FUNCTION
    return NULL;
}

int16_t* Base::get_idset() {
  return connection_idset_;
}

uint16_t Base::getcount() const {
  return count_;
}

bool Base::hash() {
  bool result = connection_idset_[0] != ID_INVALID;
  return result;
}

int32_t Base::get_onestep_accept() const {
  return onestep_accept_;
}

void Base::set_onestep_accept(int32_t count) {
  onestep_accept_ = count;
}

uint64_t Base::get_send_bytes() {
  uint64_t result = send_bytes_;
  send_bytes_ = 0;
  return result;
}
   
uint64_t Base::get_receive_bytes() {
  uint64_t result = receive_bytes_;
  receive_bytes_ = 0;
  return result;
}

connection::Pool *Base::getpool() {
  if (!pool_) pool_ = new connection::Pool();
  Assert(pool_);
  return pool_;
}

uint16_t Base::get_listenport() const {
  return listenport_;
}

connection::Base *Base::get(uint16_t id) {
  __ENTER_FUNCTION 
    Assert(id >= 0 && id < NET_OVER_SERVER_MAX);
    connection::Base *connection = NULL;
    connection = pool_->get(id);
    Assert(connection);
    return connection;
  __LEAVE_FUNCTION
    return NULL;
}

bool Base::isinit() const {
  return isinit_;
}

} //namespace manager

} //namespace connection

} //namespace pf_net
