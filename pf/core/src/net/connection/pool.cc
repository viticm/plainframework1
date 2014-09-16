#include "pf/net/connection/pool.h"

namespace pf_net {

namespace connection {

Pool::Pool() {
  connections_ = NULL;
  position_ = 0;
  count_ = 0;
  maxcount_ = NET_CONNECTION_POOL_SIZE_DEFAULT;
}

Pool::~Pool() {
  __ENTER_FUNCTION
    uint16_t i = 0;
    for (i = 0; i < maxcount_; ++i) {
      SAFE_DELETE(connections_[i]);
    }
    SAFE_DELETE_ARRAY(connections_);
  __LEAVE_FUNCTION
}

bool Pool::init(uint32_t maxcount) {
  __ENTER_FUNCTION
    maxcount_ = maxcount;
    connections_ = new Base * [maxcount_];
    Assert(connections_);
    uint16_t i;
    for(i = 0; i < maxcount_; ++i) {
      connections_[i] = NULL;
    }
    position_ = 0;
    count_ = maxcount_;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Pool::init_data(uint16_t index, Base *connection) {
  __ENTER_FUNCTION
    Assert(connection);
    Assert(index >= 0 && index < maxcount_);
    connections_[index] = connection;
    connections_[index]->setid(index);
    connections_[index]->setempty(true);
  __LEAVE_FUNCTION
    return false;
}

Base *Pool::get(int16_t id) {
  __ENTER_FUNCTION
    Base *connection = NULL;
    if (static_cast<uint32_t>(id) > maxcount_) return NULL;
    connection = connections_[id];
    if (NULL == connection) ERRORPRINTF("Pool::get is NULL");
    return connection;
  __LEAVE_FUNCTION
    return NULL;
}

Base *Pool::create(bool clear) {
  __ENTER_FUNCTION
    Base *connection = NULL;
    lock();
    uint16_t result = 0, i;
    for (i = 0; i < maxcount_; i++) {
      if (connections_[position_]->isempty()) { //找出空闲位置
        result = static_cast<uint16_t>(position_);
        connection = connections_[result];
        if (clear && connection) connection->cleanup(); //清除连接信息
        if (connection) connection->setempty(false);
        ++position_;
        if (position_ >= maxcount_) position_ = 0;
        --count_;
        break;
      }
      ++position_;
      if (position_ >= maxcount_) position_ = 0;
    }
    unlock();
    return connection;
  __LEAVE_FUNCTION
    unlock();
    return NULL;
}

void Pool::remove(int16_t id) {
  __ENTER_FUNCTION
    lock();
    if (static_cast<uint32_t>(id) > maxcount_) {
      Assert(false);
      unlock();
      return;
    }
    connections_[id]->cleanup(); //清除连接信息
    ++count_;
    unlock();
  __LEAVE_FUNCTION
    unlock();
}

void Pool::lock() {
  lock_.lock();
}

void Pool::unlock() {
  lock_.unlock();
}

} //namespace connection

} //namespace pf_net
