#include "pf/cache/dbstore.h"
#include "pf/cache/repository.h"
#include "pf/cache/manager.h"

namespace pf_cache {

Manager::Manager() {
  __ENTER_FUNCTION
    dbdirver_ = NULL;
  __LEAVE_FUNCTION
}
   
Manager::~Manager() {
  __ENTER_FUNCTION
    SAFE_DELETE(dbdirver_);
  __LEAVE_FUNCTION
}

Repository *Manager::create_dbdirver() {
  __ENTER_FUNCTION
    Repository *result = 
      is_null(dbdirver_) ? new Repository(new DBStore) : dbdirver_;
    dbdirver_ = is_null(dbdirver_) ? result : dbdirver_;
    return result;
  __LEAVE_FUNCTION
    return NULL;
}
   
Repository *Manager::get_dbdirver() {
  return dbdirver_;
}

} //namespace pf_cache
