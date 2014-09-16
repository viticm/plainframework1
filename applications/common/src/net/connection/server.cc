#include "common/define/enum.h"
#include "common/net/connection/server.h"

namespace common {

namespace net {

namespace connection {

Server::Server() { 
  status_ = 0;
  serverdata_ = NULL;
}

Server::~Server() {
  //do nothing
}

bool Server::init() {
  __ENTER_FUNCTION
    Base::setdisconnect(false);
    Base::resetkick();
    bool result = Base::init();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Server::processinput() {
  __ENTER_FUNCTION
    bool result = false;
    result = Base::processinput();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Server::processoutput() {
  __ENTER_FUNCTION
    bool result = false;
    result = Base::processoutput();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Server::processcommand(bool option) {
  __ENTER_FUNCTION
    bool result = false;
    result = Base::processcommand(option);
    return result;
  __LEAVE_FUNCTION
    return false;
}

void Server::cleanup() {
  __ENTER_FUNCTION
    Base::cleanup();
  __LEAVE_FUNCTION
}

bool Server::heartbeat(uint32_t time) {
  __ENTER_FUNCTION
    bool reslut = false;
    reslut = Base::heartbeat(time);
    return reslut;
  __LEAVE_FUNCTION
    return false;
}

bool Server::isserver() const {
  return true;
}

bool Server::isplayer() const {
  return false;
}

bool Server::sendpacket(pf_net::packet::Base *packet) {
  __ENTER_FUNCTION
    bool result = false;
    result = Base::sendpacket(packet);
    return result;
  __LEAVE_FUNCTION
    return false;
}

server_data_t *Server::get_serverdata() {
  return serverdata_;
}

void Server::set_serverdata(server_data_t *serverdata) {
  serverdata_ = serverdata;
}

int16_t Server::get_serverid() const {
  __ENTER_FUNCTION
    int16_t id = ID_INVALID;
    if (serverdata_) {
      id = serverdata_->id;
    }
    return id;
  __LEAVE_FUNCTION
    return ID_INVALID;
}

} //namespace connection

} //namespace net

} //namespace common
