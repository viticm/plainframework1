#include "pf/base/log.h"
#include "pf/sys/thread.h"

pf_sys::ThreadLock g_thread_lock;
pf_sys::ThreadIndent *g_thread_indent = NULL;

template<> pf_sys::ThreadIndent *
	pf_base::Singleton<pf_sys::ThreadIndent>::singleton_ = NULL;

namespace pf_sys {

uint16_t g_thread_quit_count = 0;

//-- thread class
Thread::Thread() {
  __ENTER_FUNCTION
    tickcount_ = 0;
    steps_ = 0;
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
    if (status_ != kReady || GLOBAL_VALUES["app_status"] == kAppStatusStop) 
      return;
    
    //Thread safe quit.
    {
#ifdef _DEBUG
      DEBUGPRINTF("start1 thread count: %d", 
                  GLOBAL_VALUES["thread_count"].int32());
#endif
      lock_guard<ThreadLock> autolock(g_thread_lock);
      ++GLOBAL_VALUES["thread_count"];
#ifdef _DEBUG
      DEBUGPRINTF("start2 thread count: %d", 
                  GLOBAL_VALUES["thread_count"].int32());
#endif
    }
 #if __LINUX__
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int32_t result = pthread_create(reinterpret_cast<pthread_t *>(&id_), 
                                    &attr, 
                                    ps_thread_process, 
                                    this);
    pthread_attr_destroy(&attr);
    if (result != 0) {
      char msg[32] = {0};
      snprintf(msg, 
               sizeof(msg) - 1, 
               "pthread_create failed, result: %d", 
               result);
      AssertEx(false, msg);
    }
    pthread_detach(id_);
#elif __WINDOWS__
    thread_handle_ = 
      ::CreateThread(NULL, 0, ps_thread_process, this, CREATE_SUSPENDED, &id_);
    Assert(thread_handle_ != NULL);
    id_ *= 1000;
    ResumeThread(thread_handle_);
#endif
  __LEAVE_FUNCTION
}

void Thread::stop() {
  //do nothing
}

void Thread::exit(void *retval) {
  __ENTER_FUNCTION
    //Thread safe quit.
    {
#ifdef _DEBUG
      DEBUGPRINTF("exit1 thread count: %d", 
                  GLOBAL_VALUES["thread_count"].int32());
#endif
      lock_guard<ThreadLock> autolock(g_thread_lock);
      --GLOBAL_VALUES["thread_count"];
#ifdef _DEBUG
      DEBUGPRINTF("exit2 thread count: %d", 
                  GLOBAL_VALUES["thread_count"].int32());
#endif
    }
#if __LINUX__
    //pthread_exit(retval);
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
    Thread *thread = static_cast<Thread *>(derived_thread);
    if (NULL == thread) return NULL;
    thread->set_status(Thread::kRunning);
    thread->run();
    thread->set_status(Thread::kExit);
    thread->exit(NULL);
    g_thread_lock.lock();
    ++g_thread_quit_count;
    g_thread_lock.unlock();
#if __LINUX__
    pthread_cancel(thread->get_id());
    void *result = (void *)thread->get_id();
    return result;
#elif __WINDOWS__
    return 0;
#endif
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

int32_t Thread::get_tickcount() const {
  return tickcount_;
}

int32_t Thread::getsteps() const {
  return steps_;
}

bool Thread::isinit() const {
  return false;
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
    uint64_t result = get_origine_thread_id();
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t get_origine_thread_id() {
  __ENTER_FUNCTION
     uint64_t result = 0;
#if __LINUX__
    result = static_cast<uint64_t>(pthread_self());
#elif __WINDOWS__
    result = static_cast<uint64_t>(GetCurrentThreadId() * 1000);
#endif
    return result;
  __LEAVE_FUNCTION
    return 0;
}

ThreadIndent *ThreadIndent::getsingleton_pointer() {
  return singleton_;
}

ThreadIndent &ThreadIndent::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

ThreadIndent::ThreadIndent() {
  __ENTER_FUNCTION
    for (int32_t i = 0; i < kCapsMax; ++i) {
      ids_[i] = 0;
      indents_[i] = 0;
    }
  __LEAVE_FUNCTION
}

ThreadIndent::~ThreadIndent() {
  //do nothing
}

void ThreadIndent::add(uint64_t id) {
  __ENTER_FUNCTION
    static ThreadLock lock;
    lock_guard<ThreadLock> autolock(lock);
    for (int32_t i = 0; i < kCapsMax; ++i) {
      if (ids_[i] == id) break;
      if (0 == ids_[i]) {
        ids_[i] = id;
        break;
      }
    }
  __LEAVE_FUNCTION
}

void ThreadIndent::setindent(uint64_t id, int32_t value) {
  __ENTER_FUNCTION
    for (int32_t i = 0; i < kCapsMax; ++i) {
      if (0 == ids_[i]) break;
      if (ids_[i] == id) indents_[i] = value;
    }
  __LEAVE_FUNCTION
}

int32_t ThreadIndent::getindent(uint64_t id) const {
  __ENTER_FUNCTION
    int32_t result = 0;
    for (int32_t i = 0; i < kCapsMax; ++i) {
      if (0 == ids_[i]) break;
      if (ids_[i] == id) {
        result = indents_[i];
        break;
      }
    }
    return result;
  __LEAVE_FUNCTION
    return 0;
}

ThreadPool::ThreadPool() {
  __ENTER_FUNCTION
    for (uint16_t i = 0; i < kThreadMax; ++i) {
      threads_[i] = NULL;
      tickcounts_[i] = 0;
    }
    count_ = 0;
    position_ = 0;
  __LEAVE_FUNCTION
}

ThreadPool::~ThreadPool() {
  __ENTER_FUNCTION
    for (uint16_t i = 0; i < kThreadMax; ++i) {
      SAFE_DELETE(threads_[i]);
    }
    count_ = 0;
    position_ = 0;
  __LEAVE_FUNCTION
}

bool ThreadPool::start() {
  __ENTER_FUNCTION
    for (uint16_t i = 0; i < kThreadMax; ++i) {
      if (threads_[i]) threads_[i]->start();
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ThreadPool::stop() {
  __ENTER_FUNCTION
    for (uint16_t i = 0; i < kThreadMax; ++i) {
      if (threads_[i]) threads_[i]->stop();
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ThreadPool::add(Thread *thread, int32_t index) {
  __ENTER_FUNCTION
    if (index < 0 || index >= kThreadMax) return false;
    Assert(thread && NULL == threads_[index]);
    threads_[index] = thread;
    ++position_;
    Assert(position_ <= kThreadMax);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ThreadPool::remove(uint64_t id) {
  __ENTER_FUNCTION
    bool result = false;
    for (uint16_t i = 0; i < kThreadMax; ++i) {
      if (threads_[i] && threads_[i]->get_id() == id) {
        position_ = i;
        threads_[i] = NULL;
        --count_;
        Assert(count_ >= 0);
        result = true;
        break;
      }
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

Thread *ThreadPool::get_byid(uint64_t id) {
  __ENTER_FUNCTION
    Thread *thread = NULL;
    for (uint16_t i = 0; i < kThreadMax; ++i) {
      if (threads_[i] && threads_[i]->get_id() == id) {
        thread = threads_[i];
        break;
      }
    }
    return thread;
  __LEAVE_FUNCTION
    return NULL;
}

Thread *ThreadPool::get_byindex(int32_t index) {
  __ENTER_FUNCTION
    Thread *thread = NULL;
    if (index >= kThreadMax) return thread;
    thread = threads_[index];
    return thread;
  __LEAVE_FUNCTION
    return NULL;
}

void ThreadPool::saveinfo() {
  __ENTER_FUNCTION
    for (uint16_t i = 0; i < kThreadMax; ++i) {
      if (NULL == threads_[i]) continue;
      int32_t tickcount = threads_[i]->get_tickcount();
      if (tickcounts_[i] != tickcount) {
        tickcounts_[i] = tickcount;
      } else {
        SLOW_DEBUGLOG("thread",
                      "[sys] (ThreadPool::saveinfo)"
                      " thread id: %d, tickcount: %d, steps: %d, status: %d",
                      threads_[i]->get_id(),
                      threads_[i]->get_tickcount(),
                      threads_[i]->getsteps(),
                      threads_[i]->get_status());
      }
    }
  __LEAVE_FUNCTION
}

bool ThreadPool::is_allstarted() {
  __ENTER_FUNCTION
    bool result = true;
    for (uint16_t i = 0; i < kThreadMax; ++i) {
      if (threads_[i] && !threads_[i]->isinit()) {
        result = false;
        break;
      }
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

} //namespace pf_sys
