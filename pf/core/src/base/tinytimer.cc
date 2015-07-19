#include "pf/base/tinytimer.h"

namespace pf_base {

TinyTimer::TinyTimer() {
  cleanup();
}

TinyTimer::~TinyTimer() {
  //do nothing
}

bool TinyTimer::isstart() const {
  return isstart_;
}

void TinyTimer::set_termtime(uint32_t time) {
  tick_termtime_ = time;
}

uint32_t TinyTimer::get_termtime() const {
  return tick_termtime_;
}

uint32_t TinyTimer::get_last_ticktime() const {
  return last_ticktime_;
}

void TinyTimer::cleanup() {
  tick_termtime_ = 0;
  isstart_ = false;
  last_ticktime_ = 0;
}

void TinyTimer::start(uint32_t term, uint32_t now) {
  isstart_ = true;
  tick_termtime_ = term;
  last_ticktime_ = now;
}

bool TinyTimer::counting(uint32_t now) {
  __ENTER_FUNCTION
    if (!isstart_) return false;
    uint32_t new_ticktime = now;
    uint32_t delta = 0;
    if (new_ticktime > last_ticktime_) {
      delta = new_ticktime - last_ticktime_;
    } else {
      if (new_ticktime + 10000 < last_ticktime_) {
        delta = 
          (static_cast<uint32_t>(0xFFFFFFFF) - last_ticktime_) + new_ticktime;
      } else {
        return false;
      }
    }
    if (delta < tick_termtime_) return false;
    last_ticktime_ = new_ticktime;
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace pf_base
