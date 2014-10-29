#include "pf/base/string.h"
#include "pf/net/socket/server.h"

namespace pf_net {

namespace socket {

Server::Server(uint16_t port, const char *ip, uint32_t backlog) {
  __ENTER_FUNCTION
    bool result = false;
    socket_ = new pf_net::socket::Base();
    if (NULL == socket_) { //memory not enough
      ERRORPRINTF("[net.socket] (Server::Server)"
                  " new pap_common_net::socket::Base() failed,"
                  " errorcode: %d",
                  socket_->getlast_errorcode());
      throw 1;
    }
    result = socket_->create();
    if (false == result) {
      ERRORPRINTF("[net.socket] (Server::Server)"
                  " socket_->create() failed, errorcode: %d",
                  socket_->getlast_errorcode()); 
      throw 1;
    }
    result = socket_->set_reuseaddr();
    if (false == result) {
      ERRORPRINTF("[net.socket] (Server::Server)"
                  " socket_->set_reuseaddr() failed, errorcode: %d",
                  socket_->getlast_errorcode());
      throw 1;
    }
    result = socket_->bind(port, ip);
    if (false == result) {
      ERRORPRINTF("[net.socket] (Server::Server)"
                  " socket_->bind(%d, %s) failed, errorcode: %d", 
                  port,
                  ip,
                  socket_->getlast_errorcode());
      throw 1;
    }
    result = socket_->listen(backlog);
    if (false == result) {
      ERRORPRINTF("[net.socket] (Server::Server)"
                  " socket_->listen(%d) failed, errorcode: %d",
                  backlog,
                  socket_->getlast_errorcode());
      throw 1;
    }
  __LEAVE_FUNCTION
}

Server::~Server() {
  __ENTER_FUNCTION
    if (socket_ != NULL) {
      socket_->close();
      SAFE_DELETE(socket_);
    }
  __LEAVE_FUNCTION
}

void Server::close() {
  if (socket_ != NULL) socket_->close();
}

bool Server::accept(pf_net::socket::Base *socket) {
  __ENTER_FUNCTION
    using namespace pf_base;
    if (NULL == socket) return false;
    struct sockaddr_in accept_sockaddr_in;
    socket->close();
    socket->socketid_ = socket_->accept(&accept_sockaddr_in);
    if (SOCKET_INVALID == socket->socketid_) return false;
    socket->port_ = ntohs(accept_sockaddr_in.sin_port);
    string::safecopy(socket->host_, 
                     inet_ntoa(accept_sockaddr_in.sin_addr), 
                     sizeof(socket->host_));
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint32_t Server::getlinger() const {
  __ENTER_FUNCTION
    uint32_t linger;
    linger = socket_->getlinger();
    return linger;
  __LEAVE_FUNCTION
    return 0;
}

bool Server::setlinger(uint32_t lingertime) {
  __ENTER_FUNCTION
    bool result = false;
    result = socket_->setlinger(lingertime);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Server::is_nonblocking() const {
  __ENTER_FUNCTION
    bool result = false;
    result = socket_->is_nonblocking();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Server::set_nonblocking(bool on) {
  __ENTER_FUNCTION
    return socket_->set_nonblocking(on);
  __LEAVE_FUNCTION
    return false;
}

uint32_t Server::getreceive_buffersize() const {
  __ENTER_FUNCTION
    uint32_t result = 0;
    result = socket_->getreceive_buffersize();
    return result;
  __LEAVE_FUNCTION
    return 0;
}

bool Server::setreceive_buffersize(uint32_t size) {
  __ENTER_FUNCTION
    bool result = false;
    result = socket_->setreceive_buffersize(size);
    return result;
  __LEAVE_FUNCTION
    return false;
}

int32_t Server::getid() const {
  __ENTER_FUNCTION
    int32_t result = SOCKET_INVALID;
    result = socket_->getid();
    return result;
  __LEAVE_FUNCTION
    return SOCKET_INVALID;
}

uint16_t Server::getport() const {
  __ENTER_FUNCTION
    uint16_t port = socket_->getport();
    return port;
  __LEAVE_FUNCTION
    return 0;
}

} //namespace socket

} //namespace pf_net
