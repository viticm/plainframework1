#include "pf/base/util.h"
#include "pf/script/lua/system.h"
#include "pf/engine/thread/script.h"

using namespace pf_engine::thread;

Script::Script() {
  __ENTER_FUNCTION
    using namespace pf_script;
    if (!SCRIPT_LUASYSTEM_POINTER)
      g_script_luasystem = new lua::System();
  __LEAVE_FUNCTION
}

Script::~Script() {
  __ENTER_FUNCTION
    SAFE_DELETE(g_script_luasystem);
  __LEAVE_FUNCTION
}

bool Script::init(const char *rootpath, 
                  const char *workpath, 
                  const char *globalfile) {
  __ENTER_FUNCTION
    if (!SCRIPT_LUASYSTEM_POINTER) return false;
    if (globalfile != NULL)
      SCRIPT_LUASYSTEM_POINTER->set_globalfile(globalfile);
    if (rootpath != NULL)
      SCRIPT_LUASYSTEM_POINTER->set_rootpath(rootpath);
    if (workpath != NULL)
      SCRIPT_LUASYSTEM_POINTER->set_workpath(workpath);
    if (!SCRIPT_LUASYSTEM_POINTER->init()) return false;
    SCRIPT_LUASYSTEM_POINTER->registerfunctions();
    isactive_ = true;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Script::run() {
  __ENTER_FUNCTION
    while (isactive()) {
      int32_t waittime = 
        static_cast<uint32_t>(1000 / ENGINE_THREAD_FRAME);
      SCRIPT_LUASYSTEM_POINTER->tick(waittime);
      pf_base::util::sleep(1);
    }
  __LEAVE_FUNCTION
}

void Script::stop() {
  isactive_ = false;
}

void Script::quit() {
  //do nothing
}

bool Script::isactive() {
  return isactive_;
}
