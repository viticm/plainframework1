/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id db.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/03 14:26
 * @uses the engine db thread module
 */
#ifndef PF_ENGINE_THREAD_DB_H_
#define PF_ENGINE_THREAD_DB_H_

#include "pf/engine/thread/config.h"
#include "pf/sys/thread.h"
#include "pf/db/manager.h"

namespace pf_engine {

namespace thread {

class PF_API DB : public pf_db::Manager, public pf_sys::Thread  {

 public:
   DB();
   ~DB();
 
 public:
   bool init(const char *connection_or_dbname,
             const char *username,
             const char *password);
   virtual void run();
   virtual void stop();
   void quit();
   bool isactive();

 private:
   bool isactive_;

};

}; //namespace thread

}; //namespace pf_engine

#endif //PF_ENGINE_THREAD_DB_H_
