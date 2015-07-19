/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id tinytimer.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/11 19:06
 * @uses tiny timer class
 */
#ifndef PF_BASE_TINYTIMER_H_
#define PF_BASE_TINYTIMER_H_

#include "pf/base/config.h"

namespace pf_base {

class PF_API TinyTimer {

 public:
   TinyTimer();
   ~TinyTimer();

 public:
   bool isstart() const;
   void set_termtime(uint32_t time);
   uint32_t get_termtime() const;
   uint32_t get_last_ticktime() const;
   void cleanup();
   void start(uint32_t term, uint32_t now);
   bool counting(uint32_t time);

 private:
   uint32_t tick_termtime_;
   uint32_t last_ticktime_;
   bool isstart_;

};

}; //namespace pf_base

#endif //PF_BASE_TINYTIMER_H_
