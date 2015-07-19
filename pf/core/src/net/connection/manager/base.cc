#include "pf/base/log.h"
#include "pf/sys/thread.h"
#include "pf/net/packet/factorymanager.h"
#include "pf/net/connection/manager/base.h"

namespace pf_net {

namespace connection {

namespace manager {

cache_struct::cache_struct() {
  __ENTER_FUNCTION
    queue = NULL;
    head = 0;
    tail = 0;
    size = 0;
  __LEAVE_FUNCTION
}
  
cache_struct::~cache_struct() {
  __ENTER_FUNCTION
    SAFE_DELETE_ARRAY(queue);
  __LEAVE_FUNCTION
}

bool Base::set_poll_maxcount(uint16_t) {
  return true;
}

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
    cache_.queue = new packet::queue_t[NET_MANAGER_CACHE_SIZE];
    cache_.size = NET_MANAGER_CACHE_SIZE;
  __LEAVE_FUNCTION
}

Base::~Base() {
  __ENTER_FUNCTION
    SAFE_DELETE(g_packetfactory_manager);
    SAFE_DELETE(pool_);
    SAFE_DELETE(serversocket_);
    SAFE_DELETE_ARRAY(connection_idset_);
  __LEAVE_FUNCTION
}

void Base::set_servermode(bool flag) {
  is_servermode_ = flag;
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
    if (!set_poll_maxcount(maxcount)) return false;
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
    if (is_null(connection)) return false;
    //First remove socket.
    removesocket(connection->getsocket()->getid());
    //Second clean in connection manager.
    remove(connection->getid());
    FAST_LOG(NET_MODULENAME, 
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
    if (!is_servermode_) return NULL;
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
    newconnection->setdisconnect(false); //connect is success
    FAST_LOG(NET_MODULENAME,
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

connection::Base *Base::connect(const char *ip, uint16_t port) {
  __ENTER_FUNCTION
    uint8_t step = 0;
    bool _remove = false;
    bool result = false;
    pf_net::connection::Base *connection = pool_->create();
    if (is_null(connection)) return NULL;
    if (!connection->init()) return NULL; //记得初始化连接
    pf_net::socket::Base *socket = connection->getsocket();
    try {
      result = socket->isvalid() ? true : socket->create();
      if (!result) {
        step = 1;
        goto EXCEPTION;
      }
      result = socket->connect(ip, port);
      if (!result) {
        step = 2;
        goto EXCEPTION;
      }
      result = socket->set_nonblocking();
      if (!result) {
        step = 3;
        goto EXCEPTION;
      }
      result = socket->setlinger(0);
      if (!result) {
        step = 4;
        goto EXCEPTION;
      }
    } catch (...) {
      step = 5;
      goto EXCEPTION;
    }
    result = add(connection);
    if (!result) {
      step = 6;
      goto EXCEPTION;
    }
    _remove = true;
    connection->setdisconnect(false); //Success.
    SLOW_LOG(NET_MODULENAME,
             "[net.connection.manager] (Base::connect) success!"
             " ip: %s, port: %d",
             ip,
             port);
    return connection;
EXCEPTION:
    SLOW_WARNINGLOG(NET_MODULENAME,
                    "[net.connection.manager] (Base::connect) failed!"
                    " ip: %s, port: %d, step: %d",
                    ip,
                    port,
                    step);
    if (_remove) {
      remove(connection);
    } else {
      pool_->remove(connection->getid());
    }
    return NULL;
  __LEAVE_FUNCTION
    return NULL;
}

connection::Base *Base::connectgroup(const char *ip, uint16_t port) {
  __ENTER_FUNCTION
    uint8_t step = 0;
    bool result = false;
    bool _remove = false;
    bool closesocket = false;
    pf_net::connection::Base *connection = pool_->create(true);
    if (NULL == connection) return NULL;
    if (!connection->init()) return NULL;
    pf_net::socket::Base *socket = connection->getsocket();
    try {
      result = socket->create();
      if (!result) {
        step = 1;
        goto EXCEPTION;
      }
      result = socket->set_nonblocking();
      if (!result) {
        step = 2;
        goto EXCEPTION;
      }
      result = socket->connect(ip, port);
      if (!result) {
        step = 3;
        struct timeval tm;
        fd_set readset, writeset;
        tm.tv_sec = 10;
        tm.tv_usec = 0;
        FD_ZERO(&readset);
        FD_SET(socket->getid(), &readset);
        writeset = readset;
        int32_t _result = 0;
        _result = 
          socket->select(socket->getid() + 1, &readset, &writeset, NULL, &tm);
        closesocket = true;
        if (0 == _result) { //连接超时
          step = 4;
          goto EXCEPTION;
        }
        //如果描述符变为可读或可写，就调用getsockopt取得套接字的待处理错误
        if (FD_ISSET(socket->getid(), &readset) || 
            FD_ISSET(socket->getid(), &writeset)) {
          uint32_t length = sizeof(_result);
          if (!pf_net::socket::api::getsockopt_exb(
                socket->getid(), SOL_SOCKET, SO_ERROR, &result, &length)) {
            step = 5;
            goto EXCEPTION;
          }
          if (_result != 0) {
            step = 6;
            goto EXCEPTION;
          }
        }
      }
      result = socket->set_nonblocking();
      if (!result) {
        step = 7;
        goto EXCEPTION;
      }
      result = socket->setlinger(0);
      if (!result) {
        step = 8;
        goto EXCEPTION;
      }
    } catch (...) {
      step = 9;
      goto EXCEPTION;
    }
    result = add(connection); //加入到管理器
    if (!result) {
      step = 10;
      goto EXCEPTION;
    }
    connection->setdisconnect(false); //Success.
    _remove = true;
    SLOW_LOG(NET_MODULENAME,
             "[net.connection.manager] (Base::connectgroup) success!"
             " ip: %s, port: %d",
             ip,
             port);
    return connection;
EXCEPTION:
    SLOW_WARNINGLOG(
        NET_MODULENAME,
        "[net.connection.manager] (Base::connectgroup) failed!"
        " ip: %s, port: %d, step: %d",
        ip,
        port,
        step);
    if (closesocket) socket->close();
    if (_remove) {
      remove(connection);
    } else {
      pool_->remove(connection->getid());
    }
    return NULL;
  __LEAVE_FUNCTION
    return NULL;
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
    connection::Base *connection = NULL;
    if (id >= 0 && id < NET_OVER_SERVER_MAX) {
      connection = pool_->get(id);
      Assert(connection);
    }
    return connection;
  __LEAVE_FUNCTION
    return NULL;
}

bool Base::isinit() const {
  return isinit_;
}

bool Base::sendpacket(packet::Base *packet, 
                      uint16_t connectionid, 
                      uint32_t flag) {
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> auto_lock(lock_);
    if (cache_.queue[cache_.tail].packet) {
      bool result = cacheresize();
      Assert(result);
    }
    cache_.queue[cache_.tail].packet = packet;
    cache_.queue[cache_.tail].connectionid = connectionid;
    cache_.queue[cache_.tail].flag = flag;
    ++cache_.tail;
    if (cache_.tail > cache_.size) cache_.tail = 0;
    return true;
  __LEAVE_FUNCTION
    return false;
}
   
bool Base::process_cachecommand() {
  __ENTER_FUNCTION
    bool result = false;
    if (!NET_PACKET_FACTORYMANAGER_POINTER) return result;
    uint32_t _result = kPacketExecuteStatusContinue;
    for (uint32_t i = 0; i < cache_.size; ++i) {
      packet::Base *packet = NULL;
      uint16_t connectionid = static_cast<uint16_t>(ID_INVALID);
      uint32_t flag = kPacketFlagNone;
      bool needremove = true;
      result = recvpacket(packet, connectionid, flag);
      if (!result) break;
      if (is_null(packet)) {
        SaveErrorLog();
        break;
      }
      
      if (kPacketFlagRemove == flag) {
        NET_PACKET_FACTORYMANAGER_POINTER->removepacket(packet);
        break;
      }
      
      int32_t _connectionid = static_cast<int32_t>(connectionid);
      if (ID_INVALID == _connectionid || ID_INVALID_EX == _connectionid) {
        try {
          packet->execute(NULL);
        } catch (...) {
          SaveErrorLog();
          _result = kPacketExecuteStatusError;
        }
        switch (_result) {
          case kPacketExecuteStatusError:
            break;
          case kPacketExecuteStatusBreak:
            break;
          case kPacketExecuteStatusContinue:
            break;
          case kPacketExecuteStatusNotRemove:
            needremove = false;
            break;
          case kPacketExecuteStatusNotRemoveError:
            needremove = false;
            break;
          default:
            break;
        }
      } else {
        connection::Base *connection = get(connectionid);
        if (connection) {
          try {
            packet->execute(connection);
          } catch (...) {
            SaveErrorLog();
            _result = kPacketExecuteStatusError;
          }
          switch (_result) {
            case kPacketExecuteStatusError:
              break;
            case kPacketExecuteStatusBreak:
              break;
            case kPacketExecuteStatusContinue:
              break;
            case kPacketExecuteStatusNotRemove:
              needremove = false;
              break;
            case kPacketExecuteStatusNotRemoveError:
              needremove = false;
              remove(connection);
              break;
            default:
              break;
          }
        } else {
          SLOW_ERRORLOG(NET_MODULENAME,
                        "[net.connection.manager] (Base::process_cachecommand)"
                        " the connection is NULL id: %d, packet id: %d",
                        connectionid,
                        packet->getid());
          Assert(false);
        }
      }
      if (needremove) NET_PACKET_FACTORYMANAGER_POINTER->removepacket(packet);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Base::recvpacket(packet::Base *&packet, 
                      uint16_t &connectionid, 
                      uint32_t &flag) {
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> auto_lock(lock_);
    if (is_null(cache_.queue[cache_.head].packet)) return false;
    packet = cache_.queue[cache_.head].packet;
    connectionid = cache_.queue[cache_.head].connectionid;
    flag = cache_.queue[cache_.head].flag;
    cache_.queue[cache_.head].packet = NULL;
    cache_.queue[cache_.head].connectionid = static_cast<uint16_t>(ID_INVALID);
    cache_.queue[cache_.head].flag = kPacketFlagNone;
    ++cache_.head;
    if (cache_.head > cache_.size) cache_.head = 0;
    return true;
  __LEAVE_FUNCTION
    return false;
}
   
bool Base::cacheresize() {
  __ENTER_FUNCTION
    packet::queue_t *queuenew = 
      new packet::queue_t[cache_.size + NET_MANAGER_CACHE_SIZE];
    if (is_null(queuenew)) return false;
    if (cache_.head < cache_.tail) {
      memcpy(queuenew, 
             &(cache_.queue[cache_.head]), 
             sizeof(packet::queue_t) * (cache_.tail - cache_.head));
    } else {
      memcpy(queuenew, 
             &(cache_.queue[cache_.head]), 
             sizeof(packet::queue_t) * (cache_.size - cache_.head));
      memcpy(&queuenew[cache_.size - cache_.head], 
             cache_.queue, 
             sizeof(packet::queue_t) * cache_.tail);
    }
    memset(cache_.queue, 0, sizeof(packet::queue_t) * cache_.size);
    SAFE_DELETE_ARRAY(cache_.queue);
    cache_.queue = queuenew;
    cache_.head = 0;
    cache_.tail = cache_.size;
    cache_.size = cache_.size + NET_MANAGER_CACHE_SIZE;
    FAST_DEBUGLOG(NET_MODULENAME,
                  "[net.connection.manager] (Base::cacheresize)"
                  " from: %d, to: %d",
                  cache_.size - NET_MANAGER_CACHE_SIZE,
                  cache_.size);
    return true;
  __LEAVE_FUNCTION
    return false; 
}

} //namespace manager

} //namespace connection

} //namespace pf_net
