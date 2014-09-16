/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id eyes.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/07 10:15
 * @uses the perfoemance eyes base class
 *       性能监视器只作输出和获取系统性能参数的作用，
 *       如需释放内存和优化结构则需要相应的系统管理器去负责
 */
#ifndef PF_PERFORMANCE_EYES_H_
#define PF_PERFORMANCE_EYES_H_

#include "pf/performance/config.h"
#include "pf/base/singleton.h"

namespace pf_performance {

class PF_API Eyes : public pf_base::Singleton<Eyes> {

 public:
   Eyes();
   ~Eyes();

 public:
   static Eyes *getsingleton_pointer();
   static Eyes &getsingleton();

 public:
   void printinfo();
   void activate();
   void tick_forFPS();

 public:
   uint32_t get_onlinecount() const;
   uint32_t get_connectioncount() const;
   uint64_t get_uptraffic() const;
   uint64_t get_downtraffic() const;

 public:
   void set_sendbytes(uint64_t bytes);
   void set_receivebytes(uint64_t bytes);
   void set_printinfo_interval(uint32_t interval);
   void set_onlinecount(uint32_t count);
   void set_connectioncount(uint32_t count);
   void set_fps(float value);

 protected:
   uint64_t sendbytes_[2];
   uint64_t receivebytes_[2];
   uint32_t last_counttime_;
   uint32_t last_printtime_;
   uint32_t printinfo_interval_;
   uint32_t onlinecount_;
   uint32_t connectioncount_;
   float FPS_;

};

}; //namespace pf_performance

#define PERFORMANCE_EYES_POINTER \
  pf_performance::Eyes::getsingleton_pointer()

PF_API extern pf_performance::Eyes *g_performance_eyes;

#endif //PF_PERFORMANCE_EYES_H_
