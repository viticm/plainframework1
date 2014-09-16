/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id info.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2013-2013 viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/07/05 11:38
 * @uses system info namespace, 
 *       can get system some info just like cpu or memory(use some JianYi code)
 *       use the code url: http://code.google.com/p/mooon/source/browse/trunk/common_library/include/sys
 *       (thanks eyjian@gmail.com) -- but this functions also need use in windows
 */
#ifndef PF_SYS_INFO_H_
#define PF_SYS_INFO_H_

#include "pf/sys/config.h"

namespace pf_sys {

namespace info {

//这是一个快速释放文件指针的类，暂时放置此处，以后有需要移到base的util下
class PF_API CloseHelper {
 public:
   CloseHelper(FILE*& fp) : fp_(fp) {}; //Initialization can be placed here, 
                                               //but the method to achieve must be placed inside, 
                                               //otherwise use inline functions
   //析构函数
   ~CloseHelper() {
     if (fp_ != NULL) {
       fclose(fp_);
       fp_ = NULL;
     }
   }

 private:
   FILE* fp_;
};

//current system info
typedef struct {
  int64_t uptime_second; //Seconds since boot
  uint64_t average_load[3]; //1, 5, and 15 minute load averages
  uint64_t ram_total; //Total usable main memory size
  uint64_t ram_free; //Available memory size
  uint64_t ram_shared; //Amount of shared memory
  uint64_t ram_buffer; //Memory used by buffers
  uint64_t swap_total; //Total swap space size
  uint64_t swap_free; //swap space still available
  uint16_t process_number; //Number of current processes
} sys_info_t;

//current process time
typedef struct {
  int64_t user_time; //user time
  int64_t system_time; //system time
  int64_t user_time_child; 
  int64_t system_time_child;
} process_time_t;

//current cpu info
typedef struct {
  //单位: jiffies, 1jiffies=0.01秒,下面的时间都是从系统启动开始累计到当前时刻
  uint64_t total; 
  uint32_t user; //处于用户态的运行时间，不包含 nice值为负进程
  uint32_t nice; //nice值为负的进程所占用的CPU时间
  uint32_t system; //处于核心态的运行时间
  uint32_t idle; //除IO等待时间以外的其它等待时间
  uint32_t iowait; //IO等待时间(2.5.41)
  uint32_t irq; //硬中断时间(2.6.0)
  uint32_t softirq;//软中断时间(2.6.0)
  char cpu_name[CPU_NAME_MAX];
} cpu_info_t;

//current memory info
typedef struct {
  uint32_t mem_total; //total memory
  uint32_t mem_free; //free memory
  uint32_t buffers;
  uint32_t cached;
  uint32_t swap_cached;
  uint32_t swap_total;
  uint32_t swap_free;
} memory_info_t;

//current system version
typedef struct {
  char system_name[SYSTEM_NAME_MAX];
  int16_t major; //主版本号
  int16_t minor; //次版本号
  int16_t revision; //修订版本号
} kernel_version_t;

//current process info
typedef struct {
  
} process_info_t;

typedef struct {
  int64_t size;
  int64_t resident;
  int64_t share;
  int64_t text;
  int64_t lib;
  int64_t data;
} process_page_info_t;

//current net info
typedef struct {
  char interface_name[INTERFACE_NAME_MAX]; //网卡名，如eth0
  //receive info
  uint64_t receive_bytes;
  uint64_t receive_packets;
  uint64_t receive_errors;
  uint64_t receive_dropped;
  uint64_t receive_fifo_errors;
  uint64_t receive_frame;
  uint64_t receive_compressed;
  uint64_t receive_multicast;

  //send info
  uint64_t transmit_bytes;
  uint64_t transmit_packets;
  uint64_t transmit_errors;
  uint64_t transmit_dropped;
  uint64_t transmit_fifo_errors;
  uint64_t transmit_collisions;
  uint64_t transmit_carrier;
  uint64_t transmit_compressed;
} net_info_t;

typedef struct {
  float oneminutes;
  float fiveminutes;
  float fifteenminutes;
} loadaverage_t;

PF_API bool get_sys_info(sys_info_t &sys_info);
PF_API bool get_mem_info(memory_info_t &mem_info);
PF_API bool get_cpu_info(cpu_info_t &cpu_info);
PF_API int32_t get_cpu_info_array(std::vector<cpu_info_t> &cpu_info_array);
PF_API bool get_kernel_version(kernel_version_t &kernel_version);
PF_API bool get_process_info(process_info_t &process_info);
PF_API bool get_process_page_info(process_page_info_t &process_page_info);
PF_API bool get_process_times(process_time_t &process_time);
PF_API bool get_net_info(const char *interface_name, net_info_t &net_info);
PF_API bool get_net_info_array(std::vector<net_info_t> &net_info_array);
PF_API bool do_get_net_info_array(const char *interface_name, 
                                  std::vector<net_info_t> &net_info_array);
PF_API bool get_ip(char *&ip, const char *interface_name = NULL);
PF_API bool get_loadaverage(loadaverage_t &loadaverage);

} //namespace info

} //namespace pf_sys

#endif //PF_SYS_INFO_H_
