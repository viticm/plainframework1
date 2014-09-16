/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id net.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/03 16:05
 * @uses your description
 */
#ifndef PF_ENGINE_THREAD_NET_H_
#define PF_ENGINE_THREAD_NET_H_

#include "pf/engine/thread/config.h"
#include "pf/sys/thread.h"
#include "pf/net/manager.h"

namespace pf_engine {

namespace thread {

class PF_API Net : public pf_net::Manager, public pf_sys::Thread {

 public:
   Net();
   ~Net();

 public:
   bool init(uint16_t connectionmax = NET_CONNECTION_MAX,
             uint16_t listenport = 0,
             const char *listenip = NULL);
   virtual void run();
   virtual void stop();
   void quit();
   bool isactive() const;

 private:
   bool isactive_;

};

}; //namespace thread

}; //namespace pf_engine

#endif //PF_ENGINE_THREAD_NET_H_
