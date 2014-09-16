/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id performance.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/03 16:23
 * @uses the engine performance thread
 */
#ifndef PF_ENGINE_THREAD_PERFORMANCE_H_
#define PF_ENGINE_THREAD_PERFORMANCE_H_

#include "pf/engine/thread/config.h"
#include "pf/sys/thread.h"

namespace pf_engine {

namespace thread {

class PF_API Performance : public pf_sys::Thread {

 public:
   Performance();
   ~Performance();

 public:
   bool init();
   virtual void run();
   virtual void stop();
   void quit();
   bool isactive();

 private:
   bool isactive_;

};

}; //namespace thread

}; //namespace pf_engine

#endif //PF_ENGINE_THREAD_PERFORMANCE_H_
