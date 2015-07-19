#include "pf/base/util.h"
#include "pf/engine/thread/db.h"

using namespace pf_engine::thread;

DB::DB() {
  isactive_ = false;
}

DB::~DB() {
  //do nothing
}

bool DB::init(const char *connection_or_dbname,
              const char *username,
              const char *password) {
  __ENTER_FUNCTION
    using namespace pf_db;
    isactive_  = Manager::init(connection_or_dbname, username, password);
    return isactive_;
  __LEAVE_FUNCTION
    return false;
}

void DB::run() {
  __ENTER_FUNCTION
    using namespace pf_db;
    while (isactive()) {
      Manager::check_db_connect();
      int32_t waittime = 
        static_cast<uint32_t>(1000 / ENGINE_THREAD_FRAME);
      pf_base::util::sleep(waittime);
    }
  __LEAVE_FUNCTION
}

bool DB::isactive() {
  return isactive_;
}

void DB::stop() {
  isactive_ = false;
}

void DB::quit() {
  //do nothing
}
