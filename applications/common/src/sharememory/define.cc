#include "pf/sys/memory/share.h"
#include "common/sharememory/define.h"

bool g_commond_exit = false;

namespace common {

namespace sharememory {

head_struct::head_struct() {
  __ENTER_FUNCTION
    cleanup();
  __LEAVE_FUNCTION
}

head_struct::~head_struct() {
  //do nothing
}

void head_struct::cleanup() {
  __ENTER_FUNCTION
    using namespace pf_sys::memory::share;
    poolid = 0;
    id = -1;
    playerid = -1;
    usestatus = kUseStatusFree;
    initword(flag, kFlagFree);
    savetime = 0;
  __LEAVE_FUNCTION
}

} //namespace sharememory

} //namespace common
