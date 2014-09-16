/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id stack.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/26 16:12
 * @uses script lua module about stack classes
 */
#ifndef PF_SCRIPT_LUA_STACK_H_
#define PF_SCRIPT_LUA_STACK_H_

#include "pf/script/lua/config.h"

namespace pf_script {

namespace lua {

class PF_API StackStep {

 public:
   StackStep();
   ~StackStep();

 public:
   void reset();

 public:
   int32_t scriptid_;
   char functionname_[SCRIPT_LUA_STACK_FUNCTION_LENGTH_MAX];

};

class PF_API Stack {

 public:
   Stack();
   ~Stack();

 public:
   void reset();
   bool stepenter(const char *functionname, int32_t scriptid);
   bool stepleave();

 public:
   int32_t currentstep_;
   StackStep step_[SCRIPT_LUA_STACK_STEP_MAX];

 public:
   static Stack run_[SCRIPT_LUA_SCENE_MAX];

};

}; //namespace lua

}; //namespace pf_script

#endif //PF_SCRIPT_LUA_STACK_H_
