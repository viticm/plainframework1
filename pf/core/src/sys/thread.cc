#include "pf/sys/thread.h"

pf_sys::ThreadLock g_thread_lock;

namespace pf_sys {

uint16_t g_thread_quit_count = 0;

//-- thread class
Thread::Thread() {
  __ENTER_FUNCTION
    id_ = 0;
    status_ = kReady;
#if __WINDOWS__
    thread_handle_ = NULL; 
#endif
  __LEAVE_FUNCTION
}

Thread::~Thread() {
  //do nothing
}

void Thread::start() {
  __ENTER_FUNCTION
    if (status_ != kReady) return;
#if __LINUX__
    int32_t result = pthread_create(reinterpret_cast<pthread_t *>(&id_), 
                                    NULL, 
                                    ps_thread_process, 
                                    this);
    if (result != 0) {
      char msg[32] = {0};
      snprintf(msg, 
               sizeof(msg) - 1, 
               "pthread_create failed, result: %d", 
               result);
      AssertEx(false, msg);
    }
#elif __WINDOWS__
    thread_handle_ = 
      ::CreateThread(NULL, 0, ps_thread_process, this, CREATE_SUSPENDED, &id_);
    Assert(thread_handle_ != NULL);
    id_ *= 1000; //???
    ResumeThread(thread_handle_);
#endif
  __LEAVE_FUNCTION
}

void Thread::stop() {
  //do nothing
}

void Thread::exit(void *retval) {
  __ENTER_FUNCTION
#if __LINUX__
    pthread_exit(retval);
#elif __WINDOWS__
    USE_PARAM(retval);
    ::CloseHandle(thread_handle_);
#endif
  __LEAVE_FUNCTION
}

void Thread::run() {
  //do nothing
}

#if __LINUX__
void *ps_thread_process(void *derived_thread) {
#elif __WINDOWS__
DWORD WINAPI ps_thread_process(void *derived_thread) {
#endif
  __ENTER_FUNCTION
    Thread* thread = static_cast<Thread*>(derived_thread);
    if (NULL == thread) return NULL;
    thread->set_status(Thread::kRunning);
    thread->run();
    thread->set_status(Thread::kExit);
#if __WINDOWS__
    thread->exit(NULL);
#endif
    g_thread_lock.lock();
    ++g_thread_quit_count;
    g_thread_lock.unlock();
  __LEAVE_FUNCTION
    return NULL;
}
#if __LINUX__
uint64_t Thread::get_id() {
#elif __WINDOWS__
DWORD Thread::get_id() {
#endif
  return id_;
}

Thread::status_t Thread::get_status() {
  return status_;
}

void Thread::set_status(status_t status) {
  __ENTER_FUNCTION
    status_ = status;
  __LEAVE_FUNCTION
}

//thread class --

//-- thread lock class
ThreadLock::ThreadLock() {
  __ENTER_FUNCTION
#if __LINUX__
    pthread_mutex_init(&mutex_, NULL);
#elif __WINDOWS__
    InitializeCriticalSection(&lock_);
#endif
  __LEAVE_FUNCTION
}

ThreadLock::~ThreadLock() {
  __ENTER_FUNCTION
#if __LINUX__
    pthread_mutex_destroy(&mutex_);
#elif __WINDOWS__
    DeleteCriticalSection(&lock_);
#endif
  __LEAVE_FUNCTION
}

void ThreadLock::lock() {
  __ENTER_FUNCTION
#if __LINUX__
    pthread_mutex_lock(&mutex_);
#elif __WINDOWS__
    EnterCriticalSection(&lock_);
#endif
  __LEAVE_FUNCTION
}

void ThreadLock::unlock() {
  __ENTER_FUNCTION
#if __LINUX__
    pthread_mutex_unlock(&mutex_);
#elif __WINDOWS__
    LeaveCriticalSection(&lock_);
#endif
  __LEAVE_FUNCTION
}

//thread lock class --
uint64_t get_current_thread_id() {
  __ENTER_FUNCTION
    uint64_t result = 0;
#if __LINUX__
    result = static_cast<uint64_t>(pthread_self());
#elif __WINDOWS__
    result = static_cast<uint64_t>(GetCurrentThreadId());
#endif
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

} //namespace pf_sys
