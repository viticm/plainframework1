#include "pf/base/time_manager.h"
#include "pf/event/system.h"

template <>
pf_event::System *pf_base::Singleton<pf_event::System>::singleton_ = NULL;

namespace pf_event {

System::System() {
  last_tickcount_ = 0;
}

System::~System() {
  //do nothing
}

System *System::getsingleton_pointer() {
  return singleton_;
}

System &System::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

void System::init(eventdefine_t events[]) {
  __ENTER_FUNCTION
    int32_t eventnumber = 
      static_cast<int32_t>(sizeof(events) / sizeof(eventdefine_t));
    int32_t i;
    for (i = 0; i < eventnumber; ++i) {
      index_asname_map_[events[i].event] = &(events[i]); 
      index_asid_map_[events[i].id] = &(events[i]);
    }
  __LEAVE_FUNCTION
}

void System::release() {
  //do nothing
}

void System::tick() {
 //do nothing
}

void System::push(const event_t &event) {
  __ENTER_FUNCTION
    if (!event.eventdefine) return;
    if (event.eventdefine->delayprocess) { //慢速处理队列
      delayquene_.push_back(event);
    }
    else {
      queue_.push_back(event);
    }
  __LEAVE_FUNCTION
}

void System::push(uint16_t id,std::vector<std::string> param) {
  __ENTER_FUNCTION
    if (index_asid_map_.find(id) == index_asid_map_.end()) return;
    event_t event;
    event.eventdefine = index_asid_map_[id];
    event.args = param;
    push(event);
  __LEAVE_FUNCTION
}

void System::push(uint16_t id) {
  __ENTER_FUNCTION
    if (index_asid_map_.find(id) == index_asid_map_.end()) return;
    event_t event;
    event.eventdefine = index_asid_map_[id];
    push(event);
  __LEAVE_FUNCTION
}

void System::push(uint16_t id, int32_t arg0) {
  __ENTER_FUNCTION
    if (index_asid_map_.find(id) == index_asid_map_.end()) return;
    event_t event;
    event.eventdefine = index_asid_map_[id];
    char temp[32] = {0};
    snprintf(temp, sizeof(temp) - 1, "%d", arg0);
    event.args.push_back(temp);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(uint16_t id, int32_t arg0, int32_t arg1) {
  __ENTER_FUNCTION
    if (index_asid_map_.find(id) == index_asid_map_.end()) return;
    event_t event;
    event.eventdefine = index_asid_map_[id];
    char temp[32] = {0};
    snprintf(temp, sizeof(temp) - 1, "%d", arg0);
    event.args.push_back(temp);
    snprintf(temp, sizeof(temp) - 1, "%d", arg1);
    event.args.push_back(temp);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(uint16_t id, int32_t arg0, int32_t arg1, int32_t arg2) {
  __ENTER_FUNCTION
    if (index_asid_map_.find(id) == index_asid_map_.end()) return;
    event_t event;
    event.eventdefine = index_asid_map_[id];
    char temp[32] = {0};
    snprintf(temp, sizeof(temp) - 1, "%d", arg0);
    event.args.push_back(temp);
    snprintf(temp, sizeof(temp) - 1, "%d", arg1);
    event.args.push_back(temp);
    snprintf(temp, sizeof(temp) - 1, "%d", arg2);
    event.args.push_back(temp);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(uint16_t id, float arg0) {
  __ENTER_FUNCTION
    if (index_asid_map_.find(id) == index_asid_map_.end()) return;
    event_t event;
    event.eventdefine = index_asid_map_[id];
    char temp[32] = {0};
    snprintf(temp, sizeof(temp) - 1, "%f", arg0);
    event.args.push_back(temp);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(uint16_t id, const char *arg0) {
  __ENTER_FUNCTION
    if (index_asid_map_.find(id) == index_asid_map_.end()) return;
    event_t event;
    event.eventdefine = index_asid_map_[id];
    event.args.push_back(arg0);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(uint16_t id, const char *arg0, const char *arg1) {
  __ENTER_FUNCTION
    if (index_asid_map_.find(id) == index_asid_map_.end()) return;
    event_t event;
    event.eventdefine = index_asid_map_[id];
    event.args.push_back(arg0);
    event.args.push_back(arg1);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(uint16_t id, const char *arg0, const char *arg1, int32_t arg2) {
  __ENTER_FUNCTION
    if (index_asid_map_.find(id) == index_asid_map_.end()) return;
    event_t event;
    event.eventdefine = index_asid_map_[id];
    event.args.push_back(arg0);
    event.args.push_back(arg1);
    char temp[32] = {0};
    snprintf(temp, sizeof(temp) - 1, "%d", arg2);
    event.args.push_back(temp);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(uint16_t id, 
                  const char *arg0, 
                  const char *arg1, 
                  int32_t arg2,
                  int32_t arg3) {
  __ENTER_FUNCTION
    if (index_asid_map_.find(id) == index_asid_map_.end()) return;
    event_t event;
    event.eventdefine = index_asid_map_[id];
    event.args.push_back(arg0);
    event.args.push_back(arg1);
    char temp[32] = {0};
    snprintf(temp, sizeof(temp) - 1, "%d", arg2);
    event.args.push_back(temp);
    snprintf(temp, sizeof(temp) - 1, "%d", arg3);
    event.args.push_back(temp);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(uint16_t id, 
                  const char *arg0, 
                  const char *arg1, 
                  const char *arg2) {
  __ENTER_FUNCTION
    if (index_asid_map_.find(id) == index_asid_map_.end()) return;
    event_t event;
    event.eventdefine = index_asid_map_[id];
    event.args.push_back(arg0);
    event.args.push_back(arg1);
    event.args.push_back(arg2);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(const std::string &eventname, 
                  std::vector<std::string> param) {
  __ENTER_FUNCTION
    if (index_asname_map_.find(eventname) == index_asname_map_.end()) return;
    event_t event;
    event.eventdefine = index_asname_map_[eventname];
    event.args = param;
    push(event);
  __LEAVE_FUNCTION
}

void System::push(const std::string &eventname) {
  __ENTER_FUNCTION
    if (index_asname_map_.find(eventname) == index_asname_map_.end()) return;
    event_t event;
    event.eventdefine = index_asname_map_[eventname];
    push(event);
  __LEAVE_FUNCTION
}

void System::push(const std::string &eventname, int32_t arg0) {
  __ENTER_FUNCTION
    if (index_asname_map_.find(eventname) == index_asname_map_.end()) return;
    event_t event;
    event.eventdefine = index_asname_map_[eventname];
    char temp[32] = {0};
    snprintf(temp, sizeof(temp) - 1, "%d", arg0);
    event.args.push_back(temp);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(const std::string &eventname, float arg0) {
  __ENTER_FUNCTION
    if (index_asname_map_.find(eventname) == index_asname_map_.end()) return;
    event_t event;
    event.eventdefine = index_asname_map_[eventname];
    char temp[32] = {0};
    snprintf(temp, sizeof(temp) - 1, "%f", arg0);
    event.args.push_back(temp);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(const std::string &eventname, int32_t arg0, int32_t arg1) {
  __ENTER_FUNCTION
    if (index_asname_map_.find(eventname) == index_asname_map_.end()) return;
    event_t event;
    event.eventdefine = index_asname_map_[eventname];
    char temp[32] = {0};
    snprintf(temp, sizeof(temp) - 1, "%d", arg0);
    event.args.push_back(temp);
    snprintf(temp, sizeof(temp) - 1, "%d", arg1);
    event.args.push_back(temp);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(const std::string &eventname, const char *arg0) {
  __ENTER_FUNCTION
    if (index_asname_map_.find(eventname) == index_asname_map_.end()) return;
    event_t event;
    event.eventdefine = index_asname_map_[eventname];
    event.args.push_back(arg0);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(const std::string &eventname, 
                  const char *arg0, 
                  const char *arg1) {
  __ENTER_FUNCTION
    if (index_asname_map_.find(eventname) == index_asname_map_.end()) return;
    event_t event;
    event.eventdefine = index_asname_map_[eventname];
    event.args.push_back(arg0);
    event.args.push_back(arg1);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(const std::string &eventname, 
                  const char *arg0, 
                  const char *arg1,
                  int32_t arg2) {
  __ENTER_FUNCTION
    if (index_asname_map_.find(eventname) == index_asname_map_.end()) return;
    event_t event;
    event.eventdefine = index_asname_map_[eventname];
    event.args.push_back(arg0);
    event.args.push_back(arg1);
    char temp[32] = {0};
    snprintf(temp, sizeof(temp) - 1, "%d", arg2);
    event.args.push_back(temp);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(const std::string &eventname, 
                  const char *arg0, 
                  const char *arg1,
                  int32_t arg2,
                  int32_t arg3) {
  __ENTER_FUNCTION
    if (index_asname_map_.find(eventname) == index_asname_map_.end()) return;
    event_t event;
    event.eventdefine = index_asname_map_[eventname];
    event.args.push_back(arg0);
    event.args.push_back(arg1);
    char temp[32] = {0};
    snprintf(temp, sizeof(temp) - 1, "%d", arg2);
    event.args.push_back(temp);
    snprintf(temp, sizeof(temp) - 1, "%d", arg3);
    event.args.push_back(temp);
    push(event);
  __LEAVE_FUNCTION
}

void System::push(const std::string &eventname, 
                  const char *arg0, 
                  const char *arg1,
                  const char *arg2) {
  __ENTER_FUNCTION
    if (index_asname_map_.find(eventname) == index_asname_map_.end()) return;
    event_t event;
    event.eventdefine = index_asname_map_[eventname];
    event.args.push_back(arg0);
    event.args.push_back(arg1);
    event.args.push_back(arg2);
    push(event);
  __LEAVE_FUNCTION
}

void System::registerhandle(const std::string &name,
                            function_eventhandle handle,
                            uint32_t ownerdata) {
  __ENTER_FUNCTION
    if (!handle) return;
    eventdefine_t* eventdefine = index_asname_map_[name];
    if (!eventdefine) return;
    eventdefine->listenfunction_notify.push_back(
        std::make_pair(handle, ownerdata));
  __LEAVE_FUNCTION
}

void System::processall() {
  __ENTER_FUNCTION
    //处理慢速队列
    if (!delayquene_.empty()) {
      uint32_t now_tickcount = TIME_MANAGER_POINTER->get_tickcount();
      uint32_t step_tickcount = now_tickcount - last_tickcount_;
      if (step_tickcount > 20) {
        last_tickcount_ = now_tickcount;
        const event_t &event = *(delayquene_.begin());
        process(event);
        delayquene_.erase(delayquene_.begin());
      }
    }

    register std::list<event_t>::iterator iterator;
    register std::list<event_t>::iterator prev_iterator;
    for (iterator = queue_.begin(); iterator != queue_.end(); ++iterator) {
      const event_t &event = *iterator;
      bool multipushed = false; //是否有同样的事件已经处理过
      for (prev_iterator = queue_.begin(); 
           prev_iterator != iterator; 
           ++prev_iterator) {
        if (*prev_iterator == *iterator) {
          multipushed = true;
          break;
        }
      } //for loop
      if (multipushed) continue;
      process(event);
    } //for loop
    queue_.clear(); //普通队列一帧内处理完
  __LEAVE_FUNCTION
}

void System::process(const event_t &event) {
  __ENTER_FUNCTION
    eventdefine_t* eventdefine = event.eventdefine;
    if (!eventdefine) return;
    if (!eventdefine->listenfunction_notify.empty()) {
      eventdefine_t::registerstruct::iterator iterator;
      for (iterator = eventdefine->listenfunction_notify.begin();
           iterator != eventdefine->listenfunction_notify.end();
           ++iterator) {
        if ((*iterator).first) {
          (*iterator).first(&event, (*iterator).second);
        }
      } //for loop
    }
  __LEAVE_FUNCTION
}

void System::unregisterhandle(const std::string &name,
                              function_eventhandle handle,
                              uint32_t ownerdata) {
  __ENTER_FUNCTION
    if (!handle) return;
    eventdefine_t* eventdefine = index_asname_map_[name];
    if (!eventdefine) return;
    eventdefine->listenfunction_notify.remove(
        std::make_pair(handle, ownerdata));
  __LEAVE_FUNCTION
}

} //namespace pf_event
