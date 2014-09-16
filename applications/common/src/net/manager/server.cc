#include "pf/base/log.h"
#include "pf/net/socket/api.h"
#include "common/define/enum.h"
#include "common/net/packet/serverserver/connect.h"
#include "common/net/manager/server.h"

namespace common {

namespace net {

namespace manager {

Server::Server() {
  current_serverinfo_ = NULL;
  uint16_t i;
  for (i = 0; i < OVER_SERVER_MAX; ++i) {
    connectionids_[i] = ID_INVALID;
  }
}

Server::~Server() {
  //do nothing
}

bool Server::remove(pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    remove_serverconenction(connection);
    if (!Manager::remove(connection)) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

server_data_t *Server::get_current_serverinfo() {
  __ENTER_FUNCTION
    if (NULL == current_serverinfo_) {
      int16_t current_serverid = get_current_serverid();
      int16_t index = 
        SETTING_POINTER->server_info_.hash_server[current_serverid];
      Assert(index >= 0 && index < GROUP_SERVER_MAX);
      current_serverinfo_ = &(SETTING_POINTER->server_info_.data[index]);
    }
    Assert(current_serverinfo_);
    return current_serverinfo_;
  __LEAVE_FUNCTION
    return NULL;
}

server_data_t *Server::get_serverinfo(int16_t serverid) {
  __ENTER_FUNCTION
    server_data_t *serverdata = NULL;
    Assert(serverid < OVER_SERVER_MAX && serverid >= 0);
    int16_t index = SETTING_POINTER->server_info_.hash_server[serverid];
    Assert(index >= 0 && index < GROUP_SERVER_MAX);
    serverdata = &(SETTING_POINTER->server_info_.data[index]);
    return serverdata;
  __LEAVE_FUNCTION
    return NULL;
}

common::net::connection::Server *Server::get_serverconnection(
    int16_t serverid) {
  __ENTER_FUNCTION
    if (ID_INVALID == serverid) return NULL;
    Assert(serverid >= 0 && serverid < OVER_SERVER_MAX);
    int16_t connectionid = connectionids_[serverid];
    if (ID_INVALID == connectionid) return NULL;
    common::net::connection::Server *serverconnection = 
      dynamic_cast<common::net::connection::Server*>(Manager::get(connectionid));
    Assert(serverconnection);
    return serverconnection;
  __LEAVE_FUNCTION
    return NULL;
}

void Server::add_serverconnection(int16_t serverid, int16_t connectionid) {
  __ENTER_FUNCTION
    Assert(serverid >= 0 && serverid < OVER_SERVER_MAX);
    SLOW_LOG(NET_MODULENAME,
             "[common.net.manager] (Server::add_serverconnection)"
             " connectionids_[%d] = %d",
             serverid,
             connectionid); 
    connectionids_[serverid] = connectionid;
  __LEAVE_FUNCTION
}

void Server::remove_serverconenction(int16_t serverid) {
  __ENTER_FUNCTION
    Assert(serverid >= 0 && serverid < OVER_SERVER_MAX);
    SLOW_LOG(NET_MODULENAME,
             "[common.net.manager] (Server::remove_serverconnection)"
             " connectionids_[%d] = %d",
             serverid,
             ID_INVALID); 
    connectionids_[serverid] = ID_INVALID;
  __LEAVE_FUNCTION
}

void Server::remove_serverconenction(pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    common::net::connection::Server *serverconnection =
      dynamic_cast<common::net::connection::Server *>(connection);
    Assert(serverconnection);
    server_data_t *serverdata = serverconnection->get_serverdata();
    if (serverdata) {
      int16_t serverid = serverdata->id;
      remove_serverconenction(serverid);
    }
  __LEAVE_FUNCTION
}

void Server::broadcast(pf_net::packet::Base *packet) {
  __ENTER_FUNCTION
    uint16_t count = getcount();
    uint16_t i;
    for (i = 0; i < count; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      pf_net::connection::Base *connection = get(connection_idset_[i]);
      if (!connection) {
        Assert(false);
        continue;
      }
      connection->sendpacket(packet);
    }
  __LEAVE_FUNCTION
}

void Server::broadcast_center(pf_net::packet::Base *packet) {
  __ENTER_FUNCTION
    uint16_t count = getcount();
    uint16_t i;
    for (i = 0; i < count; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      common::net::connection::Server *serverconnection = 
        dynamic_cast<common::net::connection::Server *>(get(connection_idset_[i]));
      if (!serverconnection) {
        Assert(false);
        continue;
      }
      if (serverconnection->get_serverdata() && 
          kServerTypeCenter == serverconnection->get_serverdata()->type) {
        serverconnection->sendpacket(packet);
      }
    }
  __LEAVE_FUNCTION
}

void Server::broadcast_server(pf_net::packet::Base *packet) {
  __ENTER_FUNCTION
    uint16_t count = getcount();
    uint16_t i;
    for (i = 0; i < count; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      common::net::connection::Server *serverconnection = 
        dynamic_cast<common::net::connection::Server *>(get(connection_idset_[i]));
      if (!serverconnection) {
        Assert(false);
        continue;
      }
      if (serverconnection->get_serverdata() && 
          kServerTypeServer == serverconnection->get_serverdata()->type) {
        serverconnection->sendpacket(packet);
      }
    }
  __LEAVE_FUNCTION
}

void Server::broadcast_login(pf_net::packet::Base *packet) {
  __ENTER_FUNCTION
    uint16_t count = getcount();
    uint16_t i;
    for (i = 0; i < count; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      common::net::connection::Server *serverconnection = 
        dynamic_cast<common::net::connection::Server *>(get(connection_idset_[i]));
      if (!serverconnection) {
        Assert(false);
        continue;
      }
      if (serverconnection->get_serverdata() && 
          kServerTypeLogin == serverconnection->get_serverdata()->type) {
        serverconnection->sendpacket(packet);
      }
    }
  __LEAVE_FUNCTION
}
   
bool Server::connect_toserver(const char *ip, 
                              uint16_t port, 
                              int16_t serverid, 
                              int16_t &connectionid, 
                              bool create, 
                              bool sendconnect) {
  __ENTER_FUNCTION
    using namespace common::net::packet::serverserver;
    uint8_t step = 0;
    bool _remove = false;
    bool result = false;
    pf_net::connection::Base *connection = pool_->create(create);
    if (NULL == connection) return false;
    if (!connection->init()) return false; //记得初始化连接
    pf_net::socket::Base *socket = connection->getsocket();
    try {
      if (create) {
        result = socket->create();
        if (!result) {
          step = 1;
          goto EXCEPTION;
        }
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
    if (sendconnect) {
      Connect packet;
      packet.set_serverid(serverid);
      result = connection->sendpacket(&packet);
      if (!result) {
        step = 7;
        goto EXCEPTION;
      }
    }
    SLOW_LOG(NET_MODULENAME,
             "[common.net.manager] (Server::connect_toserver) success!"
             " ip: %s, port: %d, serverid: %d",
             ip,
             port,
             serverid);
    connectionid = connection->getid();
    return true;
EXCEPTION:
    SLOW_WARNINGLOG(NET_MODULENAME,
                    "[common.net.manager] (Server::connect_toserver) failed!"
                    " ip: %s, port: %d, serverid: %d, step: %d",
                    ip,
                    port,
                    serverid,
                    step);
    if (_remove) {
      remove(connection);
    } else {
      pool_->remove(connection->getid());
    }
    return false;
  __LEAVE_FUNCTION
    return false;
}

pf_net::connection::Base *Server::connect_toserver_forgroup(
    const char *ip, 
    uint16_t port, 
    int16_t serverid, 
    int16_t &connectionid) {
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
    _remove = true;
    SLOW_LOG(NET_MODULENAME,
             "[common.net.manager] (Server::connect_toserver_forgroup) success!"
             " ip: %s, port: %d, serverid: %d",
             ip,
             port,
             serverid);
    connectionid = connection->getid();
    return connection;
EXCEPTION:
    SLOW_WARNINGLOG(
        NET_MODULENAME,
        "[common.net.manager] (Server::connect_toserver_forgroup) failed!"
        " ip: %s, port: %d, serverid: %d, step: %d",
        ip,
        port,
        serverid,
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

} //namespace manager

} //namespace net

} //namespace common
