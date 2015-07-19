#include "pf/base/util.h"
#include "pf/base/log.h"

pf_base::Log* g_log = NULL;

namespace pf_base {

pf_sys::ThreadLock g_log_lock;

template<> Log *Singleton<Log>::singleton_ = NULL;

Log *Log::getsingleton_pointer() {
  return singleton_;
}

Log &Log::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

Log::Log() {
  __ENTER_FUNCTION
    GLOBAL_VALUES["logactive"] = 1;
    logids_.init(LOGTYPE_MAX);
    log_position_.init(LOGTYPE_MAX);
    logcache_.init(LOGTYPE_MAX);
    loglock_.init(LOGTYPE_MAX);
    cache_size_ = 0;
  __LEAVE_FUNCTION
}

Log::~Log() {
  __ENTER_FUNCTION
    logcache_t::iterator_t iterator;
    for (iterator = logcache_.begin(); 
         iterator != logcache_.end(); 
         ++iterator) {
      SAFE_DELETE_ARRAY(iterator->second);
    }
    loglock_t::iterator_t iterator1;
    for (iterator1 = loglock_.begin(); 
         iterator1 != loglock_.end(); 
         ++iterator1) {
      SAFE_DELETE(iterator1->second);
    }
    cache_size_ = 0;
  __LEAVE_FUNCTION
}   
   
bool Log::register_fastlog(const char *logname) {
  __ENTER_FUNCTION
    uint8_t count = static_cast<uint8_t>(logids_.getcount());
    if (count > logids_.get_maxcount()) return false;
    if (logids_.isfind(logname)) return false;
    pf_sys::ThreadLock *lock = new pf_sys::ThreadLock;
    if (is_null(lock)) return false;
    char *buffer = new char[kDefaultLogCacheSize];
    if (is_null(buffer)) return false;
    uint8_t logid = count + 1;
    logids_.add(logname, logid);
    log_position_.add(logid, 0);
    logcache_.add(logid, buffer);
    loglock_.add(logid, lock);
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Log::check_log_directory() {
  __ENTER_FUNCTION
    using namespace pf_base::global;
    if (GLOBAL_VALUES["log_directory"] == "") {
      GLOBAL_VALUES["log_directory"] = app_basepath();
      GLOBAL_VALUES["log_directory"] += "log";
    }
  __LEAVE_FUNCTION
}

void Log::get_log_timestr(char *time_str, int32_t length) {
  __ENTER_FUNCTION
    if (TIME_MANAGER_POINTER) {
        TIME_MANAGER_POINTER->reset_time();
        snprintf(
            time_str, 
            length, 
            "%.2d:%.2d:%.2d (%"PRIu64" %.4f)",
            TIME_MANAGER_POINTER->get_hour(),
            TIME_MANAGER_POINTER->get_minute(),
            TIME_MANAGER_POINTER->get_second(),
            pf_sys::get_current_thread_id(), 
            static_cast<float>(TIME_MANAGER_POINTER->get_run_time())/1000.0);
    } else {
      snprintf(time_str,
               length, 
               "00:00:00 (%"PRIu64" 0.0000)",
               pf_sys::get_current_thread_id());
    }
  __LEAVE_FUNCTION
}

bool Log::init(int32_t cache_size) {
  __ENTER_FUNCTION
    check_log_directory();
    if (1 == APPLICATION_TYPE) {
      //初始化时将所有日志目录下的日志清除，防止客户端日志堆积过大
      char command[128] = {0};
#if __LINUX__
      snprintf(command, 
               sizeof(command) - 1, 
               "rm -rf %s/*.log", 
               GLOBAL_VALUES["log_directory"].string());
#elif __WINDOWS__
      snprintf(command, 
               sizeof(command) - 1, 
               "del %s/*.log", 
               GLOBAL_VALUES["log_directory"].string());
      util::path_towindows(command, static_cast<uint16_t>(strlen(command)));
#endif
      system(command);
      return true; //客户端不进行快速日志
    }
    cache_size_ = cache_size;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Log::get_log_filename(const char *filename_prefix, 
                           char *save, 
                           uint8_t type) { 
  __ENTER_FUNCTION
    const char *typestr = NULL;
    switch (type) {
      case 1:
        typestr = "warning";
        break;
      case 2:
        typestr = "error";
        break;
      case 3:
        typestr = "debug";
        break;
      default:
        typestr = "";
        break;
    }
    char prefixfinal[128] = {0};
    snprintf(prefixfinal, 
             sizeof(prefixfinal) - 1, 
             "%s%s%s",
             filename_prefix,
             strlen(typestr) > 0 ? "_" : "",
             typestr);
    if (TIME_MANAGER_POINTER) {
      char savedir[128] = {0};
      snprintf(savedir, 
               sizeof(savedir) - 1, 
               "%s/%.2d_%.2d_%.2d/%s", 
               GLOBAL_VALUES["log_directory"].string(),
               TIME_MANAGER_POINTER->get_year(), 
               TIME_MANAGER_POINTER->get_month(),
               TIME_MANAGER_POINTER->get_day(),
               APPLICATION_NAME);
      if (!pf_base::util::makedir(savedir, 0755))
        ERRORPRINTF("save dir: %s make failed", savedir);
      snprintf(save,
               FILENAME_MAX - 1,
               "%s/%s_%.2d.log",
               savedir,
               prefixfinal,
               TIME_MANAGER_POINTER->get_hour());
    } else {
      snprintf(save,
               FILENAME_MAX - 1,
               "%s/%s%s%s.log",
               GLOBAL_VALUES["log_directory"].string(),
               filename_prefix,
               strlen(typestr) > 0 ? "_" : "",
               typestr);
    }
  __LEAVE_FUNCTION
}

void Log::flush_log(const char *logname) {
  __ENTER_FUNCTION
    uint8_t logid = static_cast<uint8_t>(logids_.get(logname));
    char *buffer = logcache_.get(logid);
    uint32_t position = log_position_.get(logid);
    if (!loglock_.isfind(logid) || is_null(buffer)) return;
    char log_filename[FILENAME_MAX];
    memset(log_filename, '\0', sizeof(log_filename));
    get_log_filename(logname, log_filename); //快速日志不分错误
    pf_sys::ThreadLock *lock = loglock_.get(logid);
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(*lock);
    try {
      FILE* fp;
      fp = fopen(log_filename, "ab");
      if (fp) {
        fwrite(buffer, 1, position, fp);
        fclose(fp);
      }
    } catch(...) {
      //do nothing
    }
  __LEAVE_FUNCTION
}

void Log::flush_alllog() {
  __ENTER_FUNCTION
    logids_t::iterator_t iterator;
    for (iterator = logids_.begin(); iterator != logids_.end(); ++iterator) {
      flush_log(iterator->first.c_str());
    }
  __LEAVE_FUNCTION
}

void Log::remove_log(const char *file_name) {
  __ENTER_FUNCTION
    g_log_lock.lock();
    FILE* fp;
    fp = fopen(file_name, "w");
    if (fp) fclose(fp);
    g_log_lock.unlock();
  __LEAVE_FUNCTION
}

void Log::get_serial(char *serial, int16_t world_id, int16_t server_id) {
  __ENTER_FUNCTION
    USE_PARAM(world_id);
    USE_PARAM(server_id);
    USE_PARAM(serial);
  __LEAVE_FUNCTION
}

} //namespace pf_base
