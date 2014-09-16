#include "pf/base/util.h"
#include "pf/base/log.h"

pf_base::Log* g_log = NULL;

namespace pf_base {

bool g_command_logprint = true;
bool g_command_logactive = true;
uint8_t g_logmodule = 0;
const char *kBaseLogSaveDir = "./log";
pf_sys::ThreadLock g_log_lock;

const char *g_log_filename[] = {
  "debug", //kDebugLogFile
  "error", //kErrorLogFile
  "net", //kNetLogFile
  "script", //kScriptLogFile
  "engine", //kEngineLogFile
  "script", //kScriptLogFile
  '\0',
};

bool g_log_in_one_file = false;
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
    int32_t i;
    for (i = 0; i < kLogFileCount; ++i) {
      log_cache_[i] = NULL;
      log_position_[i] = 0;
    }
    cache_size_ = 0;
  __LEAVE_FUNCTION
}

Log::~Log() {
  __ENTER_FUNCTION
    int32_t i;
    for (i = 0; i < kLogFileCount; ++i) {
      SAFE_DELETE_ARRAY(log_cache_[i]);
    }
    cache_size_ = 0;
  __LEAVE_FUNCTION
}

void Log::get_log_timestr(char *time_str, int32_t length) {
  __ENTER_FUNCTION
    if (g_time_manager) {
        g_time_manager->reset_time();
        snprintf(time_str, 
                 length, 
                 "%.2d:%.2d:%.2d (%"PRIu64" %.4f)",
                 g_time_manager->get_hour(),
                 g_time_manager->get_minute(),
                 g_time_manager->get_second(),
                 pf_sys::get_current_thread_id(), 
                 static_cast<float>(g_time_manager->get_run_time())/1000.0);
    } else {
      snprintf(time_str,
               length, 
               "00:00:00 (%"PRIu64" 0.0000)",
               pf_sys::get_current_thread_id());
    }
  __LEAVE_FUNCTION
}

void Log::disk_log(const char *file_nameprefix, const char *format, ...) {
  __ENTER_FUNCTION
    if (g_command_logactive != true) return;
    if (NULL == file_nameprefix || 0 == file_nameprefix[0]) return;
    char buffer[kLogBufferTemp];
    memset(buffer, '\0', sizeof(buffer));
    va_list argptr;
    try {
      va_start(argptr, format);
      vsnprintf(buffer, 
                sizeof(kLogBufferTemp) - kLogNameTemp - 1, 
                format, 
                argptr);
      va_end(argptr);
      if (g_time_manager) {
        char time_str[kLogNameTemp] ;
        memset(time_str, '\0', sizeof(time_str));
        get_log_timestr(time_str, sizeof(time_str) - 1);        
        strncat(buffer, time_str, strlen(time_str));
      }
      strncat(buffer, LF, sizeof(LF)); //add wrap
    } catch(...) {
      if (g_command_logprint) 
        ERRORPRINTF("[base] (Log::disk_log) unknown error!"); 
      return;
    }

    if (true == g_command_logprint) {
      printf("%s", buffer);
    }

    if (!g_command_logactive) return;

    char log_file_name[FILENAME_MAX] = {0};
    try {
      memset(log_file_name, '\0', sizeof(log_file_name));
      snprintf(log_file_name, 
               sizeof(log_file_name) - 1, 
               "%s_%.4d-%.2d-%.2d.%u.log", 
               file_nameprefix, 
               g_file_name_fix / 10000,
               (g_file_name_fix % 10000) / 100,
               g_file_name_fix % 100,
               g_file_name_fix_last);
    } catch(...) {
    }
    
    g_log_lock.lock();

    try {
      FILE* fp = fopen(log_file_name, "a+");
      if (fp) {
        try {
          fwrite(buffer, 1, strlen(buffer), fp );
        } catch(...) {
        
        }
        fclose(fp);
      }
    } catch(...) {
    
    }
    g_log_lock.unlock(); 
  __LEAVE_FUNCTION
}


bool Log::init(int32_t cache_size) {
  __ENTER_FUNCTION
    if (1 == APPLICATION_TYPE) {
      //初始化时将所有日志目录下的日志清除，防止客户端日志堆积过大
      char command[128] = {0};
#if __LINUX__
      snprintf(command, sizeof(command) - 1, "rm -rf %s/*.log", kBaseLogSaveDir);
#elif __WINDOWS__
      snprintf(command, sizeof(command) - 1, "del %s/*.log", kBaseLogSaveDir);
      util::path_towindows(command, static_cast<uint16_t>(strlen(command)));
#endif
      system(command);
      return true; //客户端不进行快速日志
    }
    cache_size_ = cache_size;
    int32_t i;
    for (i = 0; i < kLogFileCount; ++i) {
      if (NULL == log_cache_[i]) log_cache_[i] = new char[cache_size_];
      if (NULL == log_cache_[i]) { //local memory is failed
        return false;
      }
      log_position_[i] = 0;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Log::get_log_filename(uint8_t logid, char *save) {
  __ENTER_FUNCTION
    const char *filename_prefix = 
      logid != kApplicationLogFile ? g_log_filename[logid] : APPLICATION_NAME;
    char prefixfinal[128] = {0};
    snprintf(prefixfinal, 
             sizeof(prefixfinal) - 1, 
             "%s%s%s", 
             filename_prefix, 
             logid != kApplicationLogFile ? "_" : "",
             logid != kApplicationLogFile ? APPLICATION_NAME : "");
    if (g_time_manager) {
      snprintf(save,
               FILENAME_MAX - 1,
               "%s/%s_%d_%d_%d.log",
               kBaseLogSaveDir,
               prefixfinal,
               g_time_manager->get_year(),
               g_time_manager->get_month(),
               g_time_manager->get_day());
    } else {
      snprintf(save,
               FILENAME_MAX - 1,
               "%s/%s.log",
               kBaseLogSaveDir,
               prefixfinal);
    }
  __LEAVE_FUNCTION
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
    if (g_time_manager) {
      snprintf(save,
               FILENAME_MAX - 1,
               "%s/%s%s%s_%d_%d_%d.log",
               kBaseLogSaveDir,
               filename_prefix,
               strlen(typestr) > 0 ? "_" : "",
               typestr,
               g_time_manager->get_year(),
               g_time_manager->get_month(),
               g_time_manager->get_day());
    } else {
      snprintf(save,
               FILENAME_MAX - 1,
               "%s/%s%s%s.log",
               kBaseLogSaveDir,
               filename_prefix,
               strlen(typestr) > 0 ? "_" : "",
               typestr);
    }
  __LEAVE_FUNCTION
}

void Log::flush_log(uint8_t logid) {
  __ENTER_FUNCTION
    if (logid > kLogFileCount) return;
    char log_file_name[FILENAME_MAX];
    memset(log_file_name, '\0', sizeof(log_file_name));
    get_log_filename(logid, log_file_name); //快速日志不分错误
    log_lock_[logid].lock();
    try {
      FILE* fp;
      fp = fopen(log_file_name, "ab");
      if (fp) {
        fwrite(log_cache_[logid], 1, log_position_[logid], fp);
        fclose(fp);
      }
    } catch(...) {
      //do nothing
    }
    log_lock_[logid].unlock();
  __LEAVE_FUNCTION
}

void Log::flush_alllog() {
  __ENTER_FUNCTION
    int32_t i;
    for (i = 0; i < kLogFileCount; ++i) {
      uint8_t logid = static_cast<uint8_t>(i);
      flush_log(logid);
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
