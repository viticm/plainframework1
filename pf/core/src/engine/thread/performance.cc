#include "pf/performance/eyes.h"
#include "pf/base/util.h"
#include "pf/engine/thread/performance.h"

using namespace pf_engine::thread;

Performance::Performance() {
  isactive_ = false;
}

Performance::~Performance() {
  __ENTER_FUNCTION
    SAFE_DELETE(g_performance_eyes);
  __LEAVE_FUNCTION
}

bool Performance::init() {
  __ENTER_FUNCTION
    using namespace pf_performance;
    if (!PERFORMANCE_EYES_POINTER)
      g_performance_eyes = new Eyes();
    if (!PERFORMANCE_EYES_POINTER) return false;
    isactive_ = true;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Performance::run() {
  __ENTER_FUNCTION
    while (isactive()) {
      if (PERFORMANCE_EYES_POINTER) {
        PERFORMANCE_EYES_POINTER->activate();
      }
    }
  __LEAVE_FUNCTION
}

void Performance::stop() {
  isactive_ = false;
}

bool Performance::isactive() {
  return isactive_;
}
