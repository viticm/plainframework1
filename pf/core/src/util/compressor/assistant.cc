#include "pf/util/compressor/minimanager.h"
#include "pf/util/compressor/assistant.h"

using namespace pf_util::compressor;

Assistant::Assistant() {
  __ENTER_FUNCTION
    workmemory_ = NULL;
    isenable_ = false;
    log_isenable_ = false;
    compressframe_ = 0;
    compressframe_success_ = 0;
  __LEAVE_FUNCTION
}

Assistant::~Assistant() {
  //do nothing
}

void Assistant::set_workmemory(void *memory) {
  workmemory_ = memory;
}

void *Assistant::get_workmemory() {
  return workmemory_;
}

bool Assistant::isenable() const {
  return isenable_;
}

void Assistant::enable(bool enable, uint64_t threadid) {
  __ENTER_FUNCTION
    if (true == enable) {
      if (!UTIL_COMPRESSOR_MINIMANAGER_POINTER ||
          NULL == UTIL_COMPRESSOR_MINIMANAGER_POINTER->alloc(threadid)) {
        isenable_ = false;
      }
    }
    isenable_ = enable;
  __LEAVE_FUNCTION
}

bool Assistant::log_isenable() const {
  return log_isenable_;
}

void Assistant::log_enable(bool enable) {
  log_isenable_ = enable;
}

void Assistant::compressframe_inc() {
  ++compressframe_;
}

uint32_t Assistant::get_compressframe() const {
  return compressframe_;
}

void Assistant::compressframe_successinc() {
  ++compressframe_success_;
}

uint32_t Assistant::get_success_compressframe() const {
  return compressframe_success_;
}
