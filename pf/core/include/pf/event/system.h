/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id system.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/29 17:36
 * @uses your description
 */
#ifndef PF_EVENT_SYSTEM_H_
#define PF_EVENT_SYSTEM_H_

#include "pf/event/config.h"
#include "pf/base/singleton.h"

namespace pf_event {

class PF_API System : public pf_base::Singleton<System> {

 public:
   System();
   virtual ~System();

 public:
   static System *getsingleton_pointer();
   static System &getsingleton();
 
 public:
   virtual void push(uint16_t id, 
                     std::vector<std::string> param);
   virtual void push(uint16_t id);
   virtual void push(uint16_t id, int32_t arg0);
   virtual void push(uint16_t id, const char *arg0);
   virtual void push(uint16_t id, 
                     const char *arg0, 
                     const char *arg1);
   virtual void push(uint16_t id,
                     const char *arg0,
                     const char *arg1,
                     int32_t arg2);
   virtual void push(uint16_t id,
                     int32_t arg0,
                     int32_t arg1);
   virtual void push(uint16_t id,
                     int32_t arg0,
                     int32_t arg1,
                     int32_t arg2);
   virtual void push(uint16_t id,
                     const char *arg0,
                     const char *arg1,
                     int32_t arg2,
                     int32_t arg3);
   virtual void push(uint16_t id,
                     const char *arg0,
                     const char *arg1,
                     const char *arg2);
   //注册事件处理函数
   virtual void registerhandle(const std::string &name,
                               function_eventhandle handle,
                               uint32_t ownerdata = 0);
   //处理
   virtual void processall();
   //取消注册事件处理函数
   virtual void unregisterhandle(const std::string& name,
                                 function_eventhandle handle,
                                 uint32_t ownerdata);
  //extends
 public:
   virtual void push(uint16_t id, float arg0);
   virtual void push(const std::string &eventname, 
                     std::vector<std::string> param);
   virtual void push(const std::string &eventname);
   virtual void push(const std::string &eventname, int32_t arg0);
   virtual void push(const std::string &eventname, float arg0);
   virtual void push(const std::string &eventname, const char *arg0);
   virtual void push(const std::string &eventname, 
                     const char *arg0, 
                     const char *arg1);
   virtual void push(const std::string &eventname,
                     const char *arg0,
                     const char *arg1,
                     int32_t arg2);
   virtual void push(const std::string &eventname,
                     int32_t arg0,
                     int32_t arg1);
   virtual void push(const std::string &eventname,
                     const char *arg0,
                     const char *arg1,
                     int32_t arg2,
                     int32_t arg3);
   virtual void push(const std::string &eventname,
                     const char *arg0,
                     const char *arg1,
                     const char *arg2);

 public:
   virtual void init(eventdefine_t events[]);
   virtual void release();
   virtual void tick();


 protected:
   std::map<std::string, eventdefine_t *> index_asname_map_;
   std::map<uint16_t, eventdefine_t *>
     index_asid_map_;
   std::list<event_t> queue_;
   std::list<event_t> delayquene_; //慢速队列
   uint32_t last_tickcount_; //上次处理事件的事件或帧

 protected:
   void push(const event_t &event); //压入事件
   void process(const event_t &event); //处理事件

};

}; //namespace pf_event

#define EVENT_SYSTEM_POINTER pf_event::System::getsingleton_pointer()

#endif //PF_EVENT_SYSTEM_H_
