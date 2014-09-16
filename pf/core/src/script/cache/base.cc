#include "pf/base/string.h"
#include "pf/script/cache/base.h"

namespace pf_script {

namespace cache {

Base::Base() {
  __ENTER_FUNCTION
    clear();
  __LEAVE_FUNCTION
}

Base::~Base() {
  //do nothing
}

void Base::clear() {
  __ENTER_FUNCTION
    id_ = ID_INVALID;
    memset(filename_, 0, sizeof(filename_));    
  __LEAVE_FUNCTION
}

bool Base::init(int32_t id, const char *filename, FILE *fp) {
  __ENTER_FUNCTION
    USE_PARAM(fp); //¿©’π∫Û»•µÙ
    using namespace pf_base;
    Assert(filename != NULL);
    id_ = id;
    string::safecopy(filename_, filename, sizeof(filename_));
    return true;
  __LEAVE_FUNCTION
    return false;
}

int32_t Base::getid() const {
  return id_;
}

const char *Base::get_filename() const {
  return filename_;
}

} //namespace cache

} //namespace pf_script
