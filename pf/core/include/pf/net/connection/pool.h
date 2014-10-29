/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id pool.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/23 14:10
 * @uses net connection pool class
 */
#ifndef PF_NET_CONNECTION_POOL_H_
#define PF_NET_CONNECTION_POOL_H_

#include "pf/net/connection/config.h"
#include "pf/sys/thread.h"
#include "pf/net/connection/base.h"

namespace pf_net {

namespace connection {

class PF_API Pool {

 public:
   Pool();
   ~Pool();

 public:
   bool init(uint32_t maxcount = NET_CONNECTION_POOL_SIZE_DEFAULT);
   Base *get(int16_t id);
   Base *create(bool clear = true); //new
   bool init_data(uint16_t index, Base *connection);
   void remove(int16_t id); //delete
   void lock();
   void unlock();
   uint32_t get_maxcount() const { return maxcount_; }   

 private:
   Base **connections_; //注意，这是一个指向Base对象的数组指针
   uint32_t position_;
   pf_sys::ThreadLock lock_;
   uint32_t count_;
   uint32_t maxcount_;

};

}; //namespace connection

}; //namespace pf_net

//extern pf_net::connection::Pool* g_connectionpool;

#endif //PF_NET_CONNECTION_POOL_H_
