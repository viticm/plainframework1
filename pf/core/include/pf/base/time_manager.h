/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id time_manager.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/06/18 15:53
 * @uses the base time manager class
 */
#ifndef PF_BASE_TIME_MANAGER_H_
#define PF_BASE_TIME_MANAGER_H_

#include "pf/base/config.h"
#if __LINUX__
#include <sys/time.h>
#include <sys/utsname.h>
#endif
#include "pf/base/singleton.h"

namespace pf_base {

class PF_API TimeManager : public Singleton<TimeManager> {

 public:
   TimeManager();
   ~TimeManager();
   uint32_t start_time_;
   uint32_t current_time_;
   time_t set_time_;
   tm tm_;
#if __LINUX__
   struct timeval start_, end_;
   struct timezone time_zone_;
#endif

 public:
   bool init();
   uint32_t get_tickcount(); //获取从程序启动到现在经历的时间(ms)
   uint32_t get_saved_time() const;
   uint32_t get_start_time() const;
   static TimeManager &getsingleton();
   static TimeManager *getsingleton_pointer();
   void reset_time();
   time_t get_ansi_time(); //standard
   uint32_t get_ctime(); //获得以1970年1月1号为起始到现在已经过去的秒数
   tm get_tm();

 public:
   void get_full_format_time(char *format_time, uint32_t length); //2013-11-29 15:38:09
   uint16_t get_year();
   uint8_t get_month();
   uint8_t get_day();
   uint8_t get_hour();
   uint8_t get_minute();
   uint8_t get_second();
   uint8_t get_week();
   uint32_t tm_todword();
   void dword_totm(uint32_t time, tm *_tm);
   uint32_t diff_dword_time(uint32_t time1, uint32_t time2);
   int32_t diff_day_count(time_t ansi_time1, time_t ansi_time2);
   uint32_t get_day_time(); //20131129
   uint32_t get_run_time();
   uint32_t get_current_time();
   uint32_t diff_time(uint32_t time1, uint32_t time2); //两个时间的差值，毫秒
   void time_totm(uint32_t time, tm *_tm);
   void tm_totime(tm *_tm, uint32_t &time);
   uint32_t get_days(); //取得以天为单位的时间值, 千位数代表年份，其他三位代表时间（天数）
   uint32_t get_hours(); //12723表示本年度第127天的5(23/4)点的第3(23%4)刻钟时间
   uint32_t get_weeks(); //取得以周为单位的时间值, 千位数代表年份，其他三位代表时间（周数）

};

}; //namespace pf_base

PF_API extern pf_base::TimeManager *g_time_manager;

#define TIME_MANAGER_POINTER \
  pf_base::TimeManager::getsingleton_pointer()

#endif //PF__COMMON_BASE_TIME_MANAGER_H_
