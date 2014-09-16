/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id system.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/11 10:36
 * @uses engine system class
 */
#ifndef ENGINE_SYSTEM_H_
#define ENGINE_SYSTEM_H_

#include "engine/config.h"
#include "pf/base/singleton.h"
#include "pf/engine/kernel.h"

namespace engine {

class System : public pf_engine::Kernel, 
  public pf_base::Singleton<System> {

 public:
   System();
   ~System();

 public:
   static System *getsingleton_pointer();
   static System &getsingleton(); 

 public:
   bool init();

 public:
   pf_db::Manager *get_dbmanager();

 private:
   bool init_setting();

};

}; //namespace engine

#define ENGINE_SYSTEM_POINTER engine::System::getsingleton_pointer()

extern engine::System *g_engine_system;

#endif //ENGINE_SYSTEM_H_
