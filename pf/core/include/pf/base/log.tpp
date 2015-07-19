/**
 * PAP Engine ( https://github.com/viticm/plainframework1 )
 * $Id log.tpp
 * @link https://github.com/viticm/plainframework1 for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com/viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com/viticm.ti@gmail.com>
 * @date 2015/04/18 18:43
 * @uses Base log moudle template implement.
 */
#ifndef PF_BASE_LOG_TPP_
#define PF_BASE_LOG_TPP_

#include "pf/base/config.h"
#include "pf/base/log.h"

namespace pf_base {

template <uint8_t type>
void Log::fast_savelog(const char *logname, const char *format, ...) {
  __ENTER_FUNCTION
    if (!logids_.isfind(logname) && !register_fastlog(logname)) {
      return;
    }
    check_log_directory();
    uint8_t logid = static_cast<uint8_t>(logids_.get(logname));
    char *cache = logcache_.get(logid);
    if (is_null(cache)) return;
    pf_sys::ThreadLock *lock = loglock_.get(logid);
    if (is_null(lock)) return;
    uint32_t position = log_position_.get(logid);
    char buffer[4096] = {0};
    char temp[4096] = {0};
    va_list argptr;
    try {
      va_start(argptr, format);
      vsnprintf(temp, sizeof(temp) - 1, format, argptr);
      va_end(argptr);
      if (1 == APPLICATION_TYPE) { //如果客户端使用了快速日志，则转为慢速
        char log_filename[FILENAME_MAX] = {0};
        get_log_filename(logname, log_filename);
        slow_savelog<type>(log_filename, temp);
        return;
      }
      char time_str[256] = {0};
      memset(time_str, '\0', sizeof(time_str));
      get_log_timestr(time_str, sizeof(time_str) - 1);
      snprintf(buffer, sizeof(buffer) - 1,"%s %s", time_str, temp);
    } catch(...) {
      Assert(false);
      return;
    }
    if (GLOBAL_VALUES["logprint"] != -1) {
      switch (type) {
        case 1:
          WARNINGPRINTF(buffer);
          break;
        case 2:
          ERRORPRINTF(buffer);
          break;
        case 3:
          DEBUGPRINTF(buffer);
          break;
        case 9:
          break;
        default:
          printf("%s"LF"", buffer);
          break;
      }
    }
    strncat(buffer, LF, sizeof(LF)); //add wrap
    if (GLOBAL_VALUES["logactive"] == 0) return; //save log condition
    int32_t length = static_cast<int32_t>(strlen(buffer));
    if (length <= 0) return;
    if (GLOBAL_VALUES["logone"] == 1) {
      //do nothing(one log file is not active in pap)
    }
    {
      pf_sys::lock_guard<pf_sys::ThreadLock> autolock(*lock);
      try {
        memcpy(cache + position, buffer, length);
      } catch(...) {
        //do nogthing
      }
      log_position_.set(logid, position + length);
    }
    if (position + length > (kDefaultLogCacheSize * 2) / 3) {
        flush_log(logname);
    }
  __LEAVE_FUNCTION
}

//模板函数 type 0 普通日志 1 警告日志 2 错误日志 3 调试日志 9 只写日志
template <uint8_t type>
void Log::slow_savelog(const char *filename_prefix, 
                       const char *format, ...) {
  __ENTER_FUNCTION
    check_log_directory();
    g_log_lock.lock();
    char buffer[4096] = {0};
    char temp[4096] = {0};
    va_list argptr;
    try {
      va_start(argptr, format);
      vsnprintf(temp, sizeof(temp) - 1, format, argptr);
      va_end(argptr);
      char time_str[256];
      memset(time_str, '\0', sizeof(time_str));
      get_log_timestr(time_str, sizeof(time_str) - 1);
      snprintf(buffer, sizeof(buffer) - 1,"%s %s", time_str, temp);

      if (GLOBAL_VALUES["logprint"] != -1) {
        switch (type) {
          case 1:
            WARNINGPRINTF(buffer);
            break;
          case 2:
            ERRORPRINTF(buffer);
            break;
          case 3:
            DEBUGPRINTF(buffer);
            break;
          case 9:
            break;
          default:
            printf("%s"LF"", buffer);
        }
      }
      strncat(buffer, LF, sizeof(LF)); //add wrap
      if (GLOBAL_VALUES["logactive"] == 0) return;
      char log_filename[FILENAME_MAX];
      memset(log_filename, '\0', sizeof(log_filename));
      get_log_filename(filename_prefix, log_filename, type);
      FILE* fp;
      fp = fopen(log_filename, "ab");
      if (fp) {
        fwrite(buffer, 1, strlen(buffer), fp);
        fclose(fp);
      }
    } catch(...) {
      ERRORPRINTF(
         "pf_base::Log::save_log have some log error in here"LF"");
    }
    g_log_lock.unlock();
  __LEAVE_FUNCTION
}

}; //namespace pf_base;

#endif //PF_BASE_LOG_TPP_
