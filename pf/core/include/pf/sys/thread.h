/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id thread.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/18 17:39
 * @uses useful thread classes
 */
#ifndef PF_THREAD_H_
#define PF_THREAD_H_

#include "pf/base/config.h"

namespace pf_sys {

class PF_API Thread {
 public:
   typedef enum { //线程的四种状态 (准备、运行中、退出中、已退出)
     kReady,
     kRunning,
     kExiting,
     kExit,
   } status_t;

 public:
   Thread();
   virtual ~Thread();
   void start();
   virtual void stop();
   void exit(void *retval = NULL);
   virtual void run();
#if __WINDOWS__
   DWORD get_id();
#elif __LINUX__
   uint64_t get_id();
#endif
   status_t get_status();
   void set_status(status_t status);

 private:
#if __LINUX__
   uint64_t id_;
#elif __WINDOWS__
   DWORD id_;
#endif
   status_t status_;
#if __WINDOWS__
   HANDLE thread_handle_;
#endif

};

#if __LINUX__
PF_API void *ps_thread_process(void *derived_thread);
#elif __WINDOWS__
PF_API DWORD WINAPI ps_thread_process(void *derived_thread);
#endif

class PF_API ThreadLock {
 public:
#if __LINUX__
   pthread_mutex_t mutex_;
#elif __WINDOWS__
   CRITICAL_SECTION lock_;
#endif
   ThreadLock();
   ~ThreadLock();
   void lock();
   void unlock();
};

PF_API uint64_t get_current_thread_id();

//global variable
extern uint16_t g_thread_quit_count;

template <typename Mutex>
class lock_guard {

 public:
   explicit lock_guard(Mutex &m) : m_(m) {
     m_.lock();
   }
   ~lock_guard() {
     m_.unlock();
   }

 private:
   Mutex &m_;
   explicit lock_guard(lock_guard &);
   lock_guard &operator = (lock_guard &);

};

}; //namespace pf_sys

//thread lock
PF_API extern pf_sys::ThreadLock g_thread_lock;

#endif //PF_THREAD_H_
