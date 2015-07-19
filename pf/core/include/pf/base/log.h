/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id log.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/18 15:48
 * @uses server log class
 */
#ifndef PF_BASE_LOG_H_
#define PF_BASE_LOG_H_

#include "pf/base/config.h"
#include "pf/sys/thread.h"
#include "pf/base/singleton.h"
#include "pf/base/hashmap/template.h"
#include "pf/base/time_manager.h"

namespace pf_base {

PF_API extern pf_sys::ThreadLock g_log_lock;

const uint32_t kLogBufferTemp = 4096;
const uint32_t kLogNameTemp = 128;
const uint32_t kDefaultLogCacheSize = 1024 * 1024 * 4;

class PF_API Log : public Singleton<Log> {

 public:
   Log();
   ~Log();
   static Log *getsingleton_pointer();
   static Log &getsingleton();

 public:
   typedef pf_base::hashmap::Template<std::string, int32_t> logids_t;
   typedef pf_base::hashmap::Template<int32_t, int32_t> log_position_t;
   typedef pf_base::hashmap::Template<int32_t, char *> logcache_t;
   typedef pf_base::hashmap::Template<int32_t, pf_sys::ThreadLock*> loglock_t;

 public:
   bool init(int32_t cache_size = kDefaultLogCacheSize);
   void flush_log(const char *logname);
   int32_t get_logsize(uint8_t logid);
   static void get_log_filename(const char *filename_prefix, 
                                char *filename, 
                                uint8_t type = 0);
   void flush_alllog();
   static void get_serial(char *serial, int16_t worldid, int16_t serverid);
   static void remove_log(const char *filename);
   static void get_log_timestr(char *time_str, int32_t length);
   static void check_log_directory();

 public:
   bool register_fastlog(const char *logname);

 public:
   //模板函数 type 0 普通日志 1 警告日志 2 错误日志 3 调试日志 9 只写日志
   template <uint8_t type>
   void fast_savelog(const char *logname, const char *format, ...);

   //模板函数 type 0 普通日志 1 警告日志 2 错误日志 3 调试日志 9 只写日志
   template <uint8_t type>
   static void slow_savelog(const char *logname, const char *format, ...);

 private:
   logids_t logids_;
   log_position_t log_position_;
   logcache_t logcache_;
   loglock_t loglock_;
   int32_t cache_size_;

};

}; //namespace pf_base


//log sytem macros
#define LOGSYSTEM_POINTER pf_base::Log::getsingleton_pointer()
#define FAST_LOG LOGSYSTEM_POINTER->fast_savelog<0>
#define FAST_WARNINGLOG LOGSYSTEM_POINTER->fast_savelog<1>
#define FAST_ERRORLOG LOGSYSTEM_POINTER->fast_savelog<2>
#define FAST_DEBUGLOG LOGSYSTEM_POINTER->fast_savelog<3>
#define FAST_WRITELOG LOGSYSTEM_POINTER->fast_savelog<9>
#define SLOW_LOG pf_base::Log::slow_savelog<0>
#define SLOW_WARNINGLOG pf_base::Log::slow_savelog<1>
#define SLOW_ERRORLOG pf_base::Log::slow_savelog<2>
#define SLOW_DEBUGLOG pf_base::Log::slow_savelog<3> 
#define SLOW_WRITELOG pf_base::Log::slow_savelog<9>

#if __LINUX__
#define SaveErrorLog() (SLOW_ERRORLOG( \
  "error", \
  "%s %d %s", \
  __FILE__, \
  __LINE__, \
  __PRETTY_FUNCTION__))
#elif __WINDOWS__
#define SaveErrorLog() (SLOW_ERRORLOG( \
  "error", \
  "%s %d %s", \
  __FILE__, \
  __LINE__, \
  __FUNCTION__))
#endif

extern pf_base::Log* g_log;

#include "pf/base/log.tpp"

#endif //PF_BASE_LOG_H_
