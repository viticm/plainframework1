#include "pf/base/time_manager.h"
#include "pf/base/log.h"
#include "pf/base/util.h"
#include "pf/script/lua/system.h"
#include "pf/performance/eyes.h"
#include "pf/base/string.h"
#include "pf/sys/process.h"
#include "pf/sys/util.h"
#include "pf/sys/thread.h"
#include "pf/engine/kernel.h"

namespace pf_engine {

Kernel::Kernel() {
  __ENTER_FUNCTION
    config_int32_.init(100);
    config_string_.init(100);
    config_bool_.init(100);
    registerconfig(ENGINE_CONFIG_DB_ISACTIVE, false);
    registerconfig(ENGINE_CONFIG_NET_ISACTIVE, false);
    registerconfig(ENGINE_CONFIG_SCRIPT_ISACTIVE, false);
    registerconfig(ENGINE_CONFIG_PERFORMANCE_ISACTIVE, false);
    registerconfig(ENGINE_CONFIG_DB_CONNECTOR_TYPE, kDBConnectorTypeODBC);
    registerconfig(ENGINE_CONFIG_DB_CONNECTION_OR_DBNAME, "");
    registerconfig(ENGINE_CONFIG_DB_USERNAME, "");
    registerconfig(ENGINE_CONFIG_DB_PASSWORD, "");
    registerconfig(ENGINE_CONFIG_NET_LISTEN_PORT, 0);
    registerconfig(ENGINE_CONFIG_NET_LISTEN_IP, "");
    registerconfig(ENGINE_CONFIG_NET_CONNECTION_MAX, NET_CONNECTION_MAX);
    registerconfig(ENGINE_CONFIG_SCRIPT_ROOTPATH, SCRIPT_ROOT_PATH_DEFAULT);
    registerconfig(ENGINE_CONFIG_SCRIPT_WORKPATH, SCRIPT_WORK_PATH_DEFAULT);
    registerconfig(ENGINE_CONFIG_SCRIPT_GLOBALFILE, 
                   SCRIPT_LUA_GLOBAL_VAR_FILE_DEFAULT);
    registerconfig(ENGINE_CONFIG_SETTING_ROOTPATH, "");
    registerconfig(ENGINE_CONFIG_DB_RUN_ASTHREAD, false);
    registerconfig(ENGINE_CONFIG_NET_RUN_ASTHREAD, false);
    registerconfig(ENGINE_CONFIG_PERFORMANCE_RUN_ASTHREAD, true);
    registerconfig(ENGINE_CONFIG_SCRIPT_RUN_ASTHREAD, false);
    registerconfig(ENGINE_CONFIG_BASEMODULE_HAS_INIT, false);
    registerconfig(ENGINE_CONFIG_NET_SERVERMODE, true);
    registerconfig(ENGINE_CONFIG_MAINLOOP, true);
    db_manager_ = NULL;
    net_manager_ = NULL;
    db_thread_ = NULL;
    net_manager_ = NULL;
	  net_thread_ = NULL;
    performance_thread_ = NULL;
    script_thread_ = NULL;
    isactive_ = false;
    GLOBAL_VALUES["thread_count"] = 0;
    GLOBAL_VALUES["app_status"] = kAppStatusSleep;
  __LEAVE_FUNCTION
}

Kernel::~Kernel() {
  __ENTER_FUNCTION
    SAFE_DELETE(performance_thread_);
    SAFE_DELETE(net_thread_);
    SAFE_DELETE(net_manager_);
    SAFE_DELETE(script_thread_);
    SAFE_DELETE(db_thread_);
    SAFE_DELETE(db_manager_);
    SAFE_DELETE(g_log);
    SAFE_DELETE(g_time_manager);
  __LEAVE_FUNCTION
}

bool Kernel::init() {
  __ENTER_FUNCTION
#if __LINUX__ //一些系统设置
    signal(SIGPIPE, SIG_IGN); //屏蔽该信号是因为在socket连接到
                              //一个不存在的IP时该信号会导致程序异常退出
    if (!pf_sys::util::set_core_rlimit()) {
      ERRORPRINTF("[engine] (Kernel::init) change core rlimit failed!");
      return false;
    }
#endif
    //base
    bool hasinit = getconfig_boolvalue(ENGINE_CONFIG_BASEMODULE_HAS_INIT);
    if (!hasinit) 
      DEBUGPRINTF("(###) engine for (%s) start...", APPLICATION_NAME);
    if (!hasinit && !init_base()) {
      SLOW_ERRORLOG(ENGINE_MODULENAME, 
                    "[engine] (Kernel::init) base module failed");
      return false;
    }
    if (!hasinit) 
      SLOW_LOG(ENGINE_MODULENAME, 
               "[engine] (Kernel::init) base module success");
    //DEBUGPRINTF("base"); 
    //SYS_PROCESS_CURRENT_INFO_PRINT();
    //db
    if (getconfig_boolvalue(ENGINE_CONFIG_DB_ISACTIVE)) {
      SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::init) start db module");
      if (!init_db()) { 
        SLOW_ERRORLOG(ENGINE_MODULENAME, 
                      "[engine] (Kernel::init) db module failed");
        return false;
      }
      SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::init) db module success");
    }
    //DEBUGPRINTF("db");
    //SYS_PROCESS_CURRENT_INFO_PRINT();
    //net
    if (getconfig_boolvalue(ENGINE_CONFIG_NET_ISACTIVE)) {
      SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::init) start net module");
      if (!init_net()) {
        SLOW_ERRORLOG(ENGINE_MODULENAME, 
                      "[engine] (Kernel::init) net module failed");
        return false;
      }
      SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::init) net module success");
    }
    //DEBUGPRINTF("net");
    //SYS_PROCESS_CURRENT_INFO_PRINT();
    //script
    if (getconfig_boolvalue(ENGINE_CONFIG_SCRIPT_ISACTIVE)) { 
      SLOW_LOG(ENGINE_MODULENAME, 
               "[engine] (Kernel::init) start script module"); 
      if (!init_script()) {
        SLOW_ERRORLOG(ENGINE_MODULENAME, 
                      "[engine] (Kernel::init) script module failed");
        return false;
      }
      SLOW_LOG(ENGINE_MODULENAME, 
               "[engine] (Kernel::init) script module success");
    }
    //DEBUGPRINTF("script");
    //SYS_PROCESS_CURRENT_INFO_PRINT();
    //performance
    if (getconfig_boolvalue(ENGINE_CONFIG_PERFORMANCE_ISACTIVE)) {
      SLOW_LOG(ENGINE_MODULENAME, 
               "[engine] (Kernel::init) start performance module");
      if (!init_performance()) {
        SLOW_ERRORLOG(ENGINE_MODULENAME,
                      "[engine] (Kernel::init) performance module failed");
        return false;
      }
      SLOW_LOG(ENGINE_MODULENAME, 
               "[engine] (Kernel::init) performance module success"); 
    }
    //DEBUGPRINTF("performance");
    //SYS_PROCESS_CURRENT_INFO_PRINT();
    isactive_ = true;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Kernel::run() {
  __ENTER_FUNCTION
    if (!isactive_ || GLOBAL_VALUES["app_status"] == kAppStatusStop) return;
    GLOBAL_VALUES["app_status"] = kAppStatusRunning;
    //base
    SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::run) base module");
    run_base();
    //db
    if (getconfig_boolvalue(ENGINE_CONFIG_DB_ISACTIVE)) {
      SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::run) db module");
      run_db();
    }
    //script
    if (getconfig_boolvalue(ENGINE_CONFIG_SCRIPT_ISACTIVE)) {
      SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::run) script module");
      run_script();
    }
    //performance
    if (getconfig_boolvalue(ENGINE_CONFIG_PERFORMANCE_ISACTIVE)) {
      SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::run) performance module");
      run_performance();
    }
    //net
    if (getconfig_boolvalue(ENGINE_CONFIG_NET_ISACTIVE)) {
      SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::run) net module");
      run_net();
    }
    bool mainloop = getconfig_boolvalue(ENGINE_CONFIG_MAINLOOP);
    if (mainloop) {
      while (isactive_) {
        uint32_t runtime = TIME_MANAGER_POINTER->get_tickcount();
        loop_handle();
        calculate_FPS();
        int32_t waittime =
          runtime +
          static_cast<uint32_t>(1000 / ENGINE_KERNEL_FRAME) -
          TIME_MANAGER_POINTER->get_tickcount();
        if (waittime > 0) pf_base::util::sleep(waittime);
      }
    }
  __LEAVE_FUNCTION
}

void Kernel::stop() {
  __ENTER_FUNCTION
    if (!isactive_) return;
    GLOBAL_VALUES["app_status"] = kAppStatusStop;
    //performance
    if (getconfig_boolvalue(ENGINE_CONFIG_PERFORMANCE_ISACTIVE)) {
      SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::stop) performance module");
      stop_performance();
    }
    
    //script
    if (getconfig_boolvalue(ENGINE_CONFIG_SCRIPT_ISACTIVE)) {
      SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::stop) script module");
      stop_script();
    }
    
    //net
    if (getconfig_boolvalue(ENGINE_CONFIG_NET_ISACTIVE)) {
      SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::stop) net module");
      stop_net();
    }

    //db
    if (getconfig_boolvalue(ENGINE_CONFIG_DB_ISACTIVE)) {
      SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::stop) db module");
      stop_db();
    }
    
    //Check threads.
    SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::stop) threads module");
    check_quitthreads();

    //base
    SLOW_LOG(ENGINE_MODULENAME, "[engine] (Kernel::stop) base module");
    stop_base();
    isactive_ = false;
  __LEAVE_FUNCTION
}

void Kernel::registerconfig(int32_t key, int32_t value) {
  __ENTER_FUNCTION
    if (config_int32_.isfind(key)) {
      SLOW_WARNINGLOG(ENGINE_MODULENAME,
                      "[engine] (Kernel::registerconfig) repeat the key: %d",
                      key);
      return;
    }
    config_int32_.add(key, value);
  __LEAVE_FUNCTION
}

void Kernel::registerconfig(int32_t key, bool value) {
  __ENTER_FUNCTION
    if (config_bool_.isfind(key)) {
      SLOW_WARNINGLOG(ENGINE_MODULENAME,
                      "[engine] (Kernel::registerconfig) repeat the key: %d",
                      key);
      return;
    }
    config_bool_.add(key, value);
  __LEAVE_FUNCTION
}

void Kernel::registerconfig(int32_t key, const char *value) {
  __ENTER_FUNCTION
    if (config_string_.isfind(key)) {
      SLOW_WARNINGLOG(ENGINE_MODULENAME,
                      "[engine] (Kernel::registerconfig) repeat the key: %d",
                      key);
      return;
    }
    config_string_.add(key, value);
  __LEAVE_FUNCTION
}

bool Kernel::setconfig(int32_t key, int32_t value) {
  __ENTER_FUNCTION
    bool result = config_int32_.set(key, value);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Kernel::setconfig(int32_t key, bool value) {
  __ENTER_FUNCTION
    bool result = config_bool_.set(key, value);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Kernel::setconfig(int32_t key, const char *value) {
  __ENTER_FUNCTION
    bool result = config_string_.set(key, value);
    return result;
  __LEAVE_FUNCTION
    return false;
}

int32_t Kernel::getconfig_int32value(int32_t key) {
  __ENTER_FUNCTION
    int32_t result = config_int32_.get(key);
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
bool Kernel::getconfig_boolvalue(int32_t key) {
  __ENTER_FUNCTION
    bool result = config_bool_.get(key);
    return result;
  __LEAVE_FUNCTION
    return false;
}
   
const char *Kernel::getconfig_stringvalue(int32_t key) {
  __ENTER_FUNCTION
    const char *result = NULL;
    result = config_string_.get(key);
    return 0 == strlen(result) ? NULL : result;
  __LEAVE_FUNCTION
    return NULL;
}

void Kernel::set_net_stream_usepacket(bool flag) {
  g_net_stream_usepacket = flag;
}

bool Kernel::init_base() {
  __ENTER_FUNCTION
    using namespace pf_base;
    if (!TIME_MANAGER_POINTER) g_time_manager = new TimeManager();
    if (!TIME_MANAGER_POINTER) return false;
    TIME_MANAGER_POINTER->init();
    if (!LOGSYSTEM_POINTER) g_log = new Log();
    if (!LOGSYSTEM_POINTER) return false;
    LOGSYSTEM_POINTER->init(10 * 1024 * 1024); //10mb cache size for fast log
    setconfig(ENGINE_CONFIG_BASEMODULE_HAS_INIT, true);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Kernel::init_db() {
  __ENTER_FUNCTION
    using namespace pf_db;
    bool isactive = getconfig_boolvalue(ENGINE_CONFIG_DB_ISACTIVE);
    bool is_usethread = getconfig_boolvalue(ENGINE_CONFIG_DB_RUN_ASTHREAD); 
    if (isactive) {
      int8_t connector_type = static_cast<int8_t>(
          getconfig_int32value(ENGINE_CONFIG_DB_CONNECTOR_TYPE));
      const char *connection_or_dbname = 
        getconfig_stringvalue(ENGINE_CONFIG_DB_CONNECTION_OR_DBNAME);
      if (NULL == connection_or_dbname) {
        SLOW_ERRORLOG(ENGINE_MODULENAME,
                      "[engine] (Kernel::init_db) the connection or db name"
                      " is empty!");
        return false;
      }
      const char *username = getconfig_stringvalue(ENGINE_CONFIG_DB_USERNAME);
      const char *password = getconfig_stringvalue(ENGINE_CONFIG_DB_PASSWORD);
      if (is_usethread) {
        db_thread_ = new thread::DB;
        if (NULL == db_thread_) return false;
        db_thread_->set_connector_type(connector_type);
        bool result = 
          db_thread_->init(connection_or_dbname, username, password);
        return result;
      } else {
        db_manager_ = new Manager;
        if (NULL == db_manager_) return false;
        db_manager_->set_connector_type(connector_type);
        bool result = 
          db_manager_->init(connection_or_dbname, username, password);
        return result;
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Kernel::init_net() {
  __ENTER_FUNCTION
    using namespace pf_net;
    bool isactive = getconfig_boolvalue(ENGINE_CONFIG_NET_ISACTIVE);
    bool is_usethread = getconfig_boolvalue(ENGINE_CONFIG_NET_RUN_ASTHREAD);
    if (isactive) {
      uint16_t listenport = static_cast<uint16_t>(
          getconfig_int32value(ENGINE_CONFIG_NET_LISTEN_PORT));
      int32_t _connectionmax = 
        getconfig_int32value(ENGINE_CONFIG_NET_CONNECTION_MAX);
      const char *listenip = getconfig_stringvalue(ENGINE_CONFIG_NET_LISTEN_IP);
      if (_connectionmax <= 0) {
        SLOW_ERRORLOG(ENGINE_MODULENAME,
                      "[engine] (Kernel::init_net)"
                      " the connection maxcount <= 0");
        return false;
      }
      uint16_t connectionmax = static_cast<uint16_t>(_connectionmax);
      bool servermode = getconfig_boolvalue(ENGINE_CONFIG_NET_SERVERMODE);
      bool result = true;
      if (is_usethread) {
        if (!net_thread_) net_thread_ = new thread::Net();
        if (NULL == net_thread_) {
          SLOW_ERRORLOG(ENGINE_MODULENAME, 
                        "[engine] (Kernel::init_net)"
                        " NULL == net_thread_");
          return false;
        }
        net_thread_->set_servermode(servermode);
        result = net_thread_->init(connectionmax, listenport, listenip);
        if (!result) {
          SLOW_ERRORLOG(ENGINE_MODULENAME, 
                        "[engine] (Kernel::init_net)"
                        " net_thread_->init(%d, %d, %s) failed",
                        connectionmax,
                        listenport,
                        listenport);
          return false;
        }
      } else {
        if (!net_manager_) net_manager_ = new Manager();
        if (NULL == net_manager_) {
          SLOW_ERRORLOG(ENGINE_MODULENAME, 
                        "[engine] (Kernel::init_net)"
                        " NULL == net_manager_");
          return false;
        }
        net_manager_->set_servermode(servermode);
        result = net_manager_->init(connectionmax, listenport, listenip);
        if (!result) {
          SLOW_ERRORLOG(ENGINE_MODULENAME, 
                        "[engine] (Kernel::init_net)"
                        " net_manager_->init(%d, %d, %s) failed",
                        connectionmax,
                        listenport,
                        listenport);
          return false;
        }
      }
      if (result) {
        result = init_net_connectionpool();
        if (!result) {
          SLOW_ERRORLOG(ENGINE_MODULENAME, 
                        "[engine] (Kernel::init_net)"
                        " init_net_connectionpool() failed");
          return false;
        }
      }
      if (result) {
        listenport = is_usethread ? 
                     net_thread_->get_listenport() : 
                     net_manager_->get_listenport();
        SLOW_LOG(ENGINE_MODULENAME,
                 "[engine] (Kernel::init_net) success!"
                 " connection maxcount: %d, listenport: %d, listenip: %s",
                 connectionmax,
                 listenport,
                 NULL == listenip ? "any" : listenip);
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Kernel::init_script() {
  __ENTER_FUNCTION
    using namespace pf_script;
    bool isactive = getconfig_boolvalue(ENGINE_CONFIG_SCRIPT_ISACTIVE);
    if (isactive) {
      bool is_usethread = 
        getconfig_boolvalue(ENGINE_CONFIG_SCRIPT_RUN_ASTHREAD);
      const char *globalfile = 
        getconfig_stringvalue(ENGINE_CONFIG_SCRIPT_GLOBALFILE);
      const char *rootpath = 
        getconfig_stringvalue(ENGINE_CONFIG_SCRIPT_ROOTPATH);
      const char *workpath =
        getconfig_stringvalue(ENGINE_CONFIG_SCRIPT_WORKPATH);
      if (is_usethread) {
        script_thread_ = new thread::Script;
        if (is_null(script_thread_)) return false;
        if (!script_thread_->init(rootpath, workpath, globalfile))
          return false;
      } else {

        if (!SCRIPT_LUASYSTEM_POINTER)
          g_script_luasystem = new lua::System();
        if (NULL == g_script_luasystem) return false;
        SCRIPT_LUASYSTEM_POINTER->set_globalfile(globalfile);
        SCRIPT_LUASYSTEM_POINTER->set_rootpath(rootpath);
        SCRIPT_LUASYSTEM_POINTER->set_workpath(workpath);
        if (!SCRIPT_LUASYSTEM_POINTER->init()) return false;
        SCRIPT_LUASYSTEM_POINTER->registerfunctions();
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Kernel::init_performance() {
  __ENTER_FUNCTION
    using namespace pf_performance;
    bool isactive = getconfig_boolvalue(ENGINE_CONFIG_PERFORMANCE_ISACTIVE);
    bool is_usethread = 
      getconfig_boolvalue(ENGINE_CONFIG_PERFORMANCE_RUN_ASTHREAD);
    if (isactive) {
      if (is_usethread) {
        performance_thread_ = new thread::Performance();
        if (NULL == performance_thread_) return false;
        if (!performance_thread_->init()) return false;
      }
    }
    else {
      if (!PERFORMANCE_EYES_POINTER)
        g_performance_eyes = new Eyes();
      if (!PERFORMANCE_EYES_POINTER) return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Kernel::run_base() {
  //do nothing
}

void Kernel::run_db() {
  __ENTER_FUNCTION
    bool is_usethread = getconfig_boolvalue(ENGINE_CONFIG_DB_RUN_ASTHREAD);
    if (is_usethread) {
      db_thread_->start();
    } else {
      db_manager_->check_db_connect();
    }
  __LEAVE_FUNCTION
}

void Kernel::run_net() {
  __ENTER_FUNCTION
    bool is_usethread = getconfig_boolvalue(ENGINE_CONFIG_NET_RUN_ASTHREAD);
    if (is_usethread) {
      net_thread_->start();
      net_thread_->threadid_ = net_thread_->get_id(); //如果自己设置了线程，则需要手动归置ID
    }
  __LEAVE_FUNCTION
}

void Kernel::run_script() {
  __ENTER_FUNCTION
    bool is_usethread = getconfig_boolvalue(ENGINE_CONFIG_SCRIPT_RUN_ASTHREAD);
    if (is_usethread) {
      script_thread_->start();
    }
  __LEAVE_FUNCTION
}

void Kernel::run_performance() {
  __ENTER_FUNCTION
    bool is_usethread = 
      getconfig_boolvalue(ENGINE_CONFIG_PERFORMANCE_RUN_ASTHREAD);
    if (is_usethread) {
      performance_thread_->start();
    } else {
      PERFORMANCE_EYES_POINTER->activate();
    }
  __LEAVE_FUNCTION
}

void Kernel::stop_base() {
  __ENTER_FUNCTION
    //SAFE_DELETE(g_log);
    //SAFE_DELETE(g_time_manager);
  __LEAVE_FUNCTION
}

void Kernel::stop_db() {
  __ENTER_FUNCTION
    bool is_usethread = getconfig_boolvalue(ENGINE_CONFIG_DB_RUN_ASTHREAD);
    if (is_usethread) {
      db_thread_->stop();
    }
    //SAFE_DELETE(db_manager_);    
  __LEAVE_FUNCTION
}

void Kernel::stop_net() {
  __ENTER_FUNCTION
    bool is_usethread = getconfig_boolvalue(ENGINE_CONFIG_NET_RUN_ASTHREAD);
    if (is_usethread) {
      net_thread_->stop();
    }
    //pf_base::util::sleep(5000);
    //SAFE_DELETE(net_manager_);
  __LEAVE_FUNCTION
}

void Kernel::stop_script() {
  __ENTER_FUNCTION
    //SAFE_DELETE(g_script_luasystem);
    bool is_usethread = getconfig_boolvalue(ENGINE_CONFIG_SCRIPT_RUN_ASTHREAD);
    if (is_usethread) {
      script_thread_->stop();
    }
  __LEAVE_FUNCTION
}

void Kernel::stop_performance() {
  __ENTER_FUNCTION
    bool is_usethread = 
      getconfig_boolvalue(ENGINE_CONFIG_PERFORMANCE_RUN_ASTHREAD);
    if (is_usethread) {
      performance_thread_->stop();
    }
  __LEAVE_FUNCTION
}

bool Kernel::init_net_connectionpool() {
  __ENTER_FUNCTION
    using namespace pf_net;
    bool is_usethread = getconfig_boolvalue(ENGINE_CONFIG_NET_RUN_ASTHREAD);
    uint16_t connectionmax = static_cast<uint16_t>(
        getconfig_int32value(ENGINE_CONFIG_NET_CONNECTION_MAX));
    if (is_usethread) {
      net_thread_->getpool()->init(connectionmax);
    } else {
      net_manager_->getpool()->init(connectionmax);
    }
    uint16_t i = 0;
    for (i = 0; i < connectionmax; ++i) {
      connection::Base *connection = new connection::Base();
      Assert(connection);
      if (is_usethread) {
        net_thread_->getpool()->init_data(i, connection);
      } else {
        net_manager_->getpool()->init_data(i, connection);
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Kernel::loop_handle() {
  __ENTER_FUNCTION
    if (getconfig_boolvalue(ENGINE_CONFIG_NET_ISACTIVE) &&
      !getconfig_boolvalue(ENGINE_CONFIG_NET_RUN_ASTHREAD)) {
      net_manager_->tick();
    }
    if (getconfig_boolvalue(ENGINE_CONFIG_SCRIPT_ISACTIVE) &&
      !getconfig_boolvalue(ENGINE_CONFIG_SCRIPT_RUN_ASTHREAD)) {
      SCRIPT_LUASYSTEM_POINTER->tick(10); //For gc.
    }
    bool db_is_usethread = getconfig_boolvalue(ENGINE_CONFIG_DB_RUN_ASTHREAD);
    bool db_isactive = getconfig_boolvalue(ENGINE_CONFIG_DB_ISACTIVE);
    if (db_isactive && !db_is_usethread)
      db_manager_->check_db_connect();
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Kernel::calculate_FPS() {
  __ENTER_FUNCTION
    if (!getconfig_boolvalue(ENGINE_CONFIG_PERFORMANCE_ISACTIVE))
      return;
    static uint32_t last_ticktime = 0;
    static uint32_t looptime = 0; //时间累计数
    static uint32_t loopcount = 0;
    uint32_t currenttime = TIME_MANAGER_POINTER->get_tickcount();
    uint32_t difftime = currenttime - last_ticktime;
    //计算帧率
    const uint32_t kCalculateFPS = 1000; //每一秒计算一次
    if (difftime != currenttime) looptime += difftime;
    last_ticktime = currenttime;
    if (looptime > kCalculateFPS) {
      FPS_ = static_cast<float>((loopcount * 1000) / looptime);
      looptime = loopcount = 0;
      if (getconfig_boolvalue(ENGINE_CONFIG_PERFORMANCE_ISACTIVE))
        PERFORMANCE_EYES_POINTER->set_fps(FPS_);
    }
    ++loopcount;
  __LEAVE_FUNCTION
}

void Kernel::check_quitthreads() {
  __ENTER_FUNCTION
    using namespace pf_sys;
    uint32_t start_tickcount = TIME_MANAGER_POINTER->get_tickcount();
    uint32_t checktime = 0;
    enum { kCheckTimeMax = 3000 };
    while (GLOBAL_VALUES["thread_count"] > 0) {
      if (GLOBAL_VALUES["thread_count"] > 0 && checktime > kCheckTimeMax) {
        SLOW_ERRORLOG(ENGINE_MODULENAME,
                      "[engine] (Kernel::check_quitthreads) quit thread more"
                      " than time max, still need quit thread: %d,"
                      " check time max: %d",
                      GLOBAL_VALUES["thread_count"].int32(),
                      kCheckTimeMax);
        break;
      } else {
        SLOW_DEBUGLOG(ENGINE_MODULENAME,
                      "[engine] (Kernel::check_quitthreads) checking ..."
                      " waiting quit thread: %d",
                      GLOBAL_VALUES["thread_count"].int32());
      }
      pf_base::util::sleep(10);
      checktime = TIME_MANAGER_POINTER->get_tickcount() - start_tickcount;
    }
  __LEAVE_FUNCTION
}

} //namespace pf_engine
