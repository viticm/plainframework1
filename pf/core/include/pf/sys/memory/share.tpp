/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id share.tpp
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/10 10:56
 * @uses system share memory template class implement
 */
#ifndef PF_SYS_MEMORY_SHARE_TPP_
#define PF_SYS_MEMORY_SHARE_TPP_

#include "pf/sys/memory/config.h"
#include "pf/base/time_manager.h"
#include "pf/sys/memory/share.h"

namespace pf_sys {

namespace memory {

namespace share {

template <typename T>
UnitManager<T>::UnitManager() {
  __ENTER_FUNCTION
    count_ = 0;
    memset(data_, 0, sizeof(T *) * SYS_MEMORY_SHARE_MANAGER_UNITDATA_MAX);
  __LEAVE_FUNCTION
}

template <typename T>
UnitManager<T>::~UnitManager() {
  //do nothing
}

template <typename T>
void UnitManager<T>::init() {
  __ENTER_FUNCTION
    count_ = 0;
  __LEAVE_FUNCTION
}

template <typename T>
bool UnitManager<T>::add_data(T *data) {
  __ENTER_FUNCTION
    Assert(count_ < SYS_MEMORY_SHARE_MANAGER_UNITDATA_MAX);
    if (count_ >= SYS_MEMORY_SHARE_MANAGER_UNITDATA_MAX) return false;
    data_[count_] = data;
    ++count_;
    data_->set_id(count_);
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <typename T>
bool UnitManager<T>::delete_data(T *data) {
  __ENTER_FUNCTION
    uint32_t id = data->get_id();
    Assert(id < static_cast<uint32_t>(count_));
    if (id >= static_cast<uint32_t>(count_)) return false;
    data_[id] = data_[count_ - 1];
    data->set_id(ID_INVALID);
    --count_;
  __LEAVE_FUNCTION
    return false;
}


template <typename T>
T *UnitManager<T>::get_data(uint16_t id) {
  __ENTER_FUNCTION
    Assert(id < static_cast<uint32_t>(count_));
    if (id >= static_cast<uint32_t>(count_)) return false;
    return data_[id];
  __LEAVE_FUNCTION
    return false;
}


template <typename T>
UnitPool<T>::UnitPool() {
  __ENTER_FUNCTION
    obj_ = NULL;
    ref_obj_pointer_ = NULL;
    max_size_ = 0;
    position_ = -1;
    key_ = 0;
    per_datasize_ = 0;
  __LEAVE_FUNCTION
}

template <typename T>
UnitPool<T>::~UnitPool() {
  __ENTER_FUNCTION
    SAFE_DELETE(ref_obj_pointer_);
    SAFE_DELETE_ARRAY(obj_);
  __LEAVE_FUNCTION
}

template <typename T>
void UnitPool<T>::set_datasize(uint32_t size) {
  __ENTER_FUNCTION
    per_datasize_ = size;
  __LEAVE_FUNCTION
}

template <typename T>
bool UnitPool<T>::init(uint32_t max_count, uint32_t key, uint8_t pooltype) {
  __ENTER_FUNCTION
    ref_obj_pointer_ = new Base();
    Assert(ref_obj_pointer_);
    if (!ref_obj_pointer_) return false;
    ref_obj_pointer_->cmd_model_ = GLOBAL_VALUES["app_cmdmodel"].int32();
    bool result = true;
    bool needinit = false;
    result = ref_obj_pointer_->attach(
        key, 
        (sizeof(T) + per_datasize_ ) * max_count + sizeof(header_t));
    if (kSmptShareMemory == pooltype && !result) {
      result = ref_obj_pointer_->create(
          key, 
          (sizeof(T) + per_datasize_) * max_count + sizeof(header_t));
      needinit = true;
    } else if (!result) {
      return false;
    }
    if (!result && kCmdModelClearAll == ref_obj_pointer_->cmd_model_) {
      return true;
    } else if (!result) {
      SLOW_ERRORLOG(
          "sharememory", 
          "[sys][sharememory] (UnitPool::init) failed");
      Assert(result);
      return result;
    }
    max_size_ = max_count;
    position_ = 0;
    obj_ = new T * [max_size_];
    if (is_null(obj_)) return false;
    uint32_t i;
    for (i = 0; i < max_size_; ++i) {
      char *pointer = 
        ref_obj_pointer_->get_data(sizeof(T) + per_datasize_, i);
      if (per_datasize_ > 0 && needinit) {
        memset(&pointer[sizeof(T)], 0, per_datasize_);
      }
      obj_[i] = reinterpret_cast<T *>(pointer);
      obj_[i]->set_datasize(per_datasize_);
      if (NULL == obj_[i]) {
        Assert(false);
        return false;
      }
      if (needinit) {
        obj_[i]->init();
      }
    }
    key_ = key;
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <typename T>
bool UnitPool<T>::release() {
  __ENTER_FUNCTION
    if (ref_obj_pointer_) ref_obj_pointer_->destory();
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <typename T>
bool UnitPool<T>::isfull() const {
  return position_ == static_cast<int32_t>(max_size_ - 1);
}
template <typename T>
T *UnitPool<T>::new_obj() {
  __ENTER_FUNCTION
    Assert(ref_obj_pointer_);
    header_t *header = ref_obj_pointer_->getheader();
    Assert(header);
    header->lock(kFlagMixedWrite); //Safe lock header.
    if (header->poolposition >= max_size_) {
      header->unlock(kFlagMixedWrite); //Safe unlock header.
      return NULL;
    }
    T *obj = obj_[header->poolposition];
    obj->set_poolid(static_cast<uint32_t>(header->poolposition)); //this function 
                                                        //must define in T*
    ++(header->poolposition);
    obj->head.key = key_;
    header->unlock(kFlagMixedWrite); //Safe unlock header.
    return obj;
  __LEAVE_FUNCTION
    return NULL;
}

template <typename T>
void UnitPool<T>::delete_obj(T *obj) {
  __ENTER_FUNCTION
    Assert(obj != NULL && ref_obj_pointer_ != NULL);
    header_t *header = ref_obj_pointer_->getheader();
    Assert(header);
    header->lock(kFlagMixedWrite); //Safe lock header.
    Assert(header->poolposition > 0);
    if (NULL == obj || header->poolposition <= 0) {
      header->unlock(kFlagMixedWrite);
      return;
    }
    uint32_t delete_index = obj->get_poolid(); //this function 
                                                //must define in T*
    Assert(delete_index < header->poolposition);
    --(header->poolposition);
    if (delete_index >= header->poolposition) {
      header->unlock(kFlagMixedWrite);
      return;
    }
    T *_delete_obj = obj_[delete_index];
    //这里不应该是指针交换，而是数据拷贝，切记（设计问题）
    //obj_[delete_index] = obj_[header->poolposition];
    //obj_[header->poolposition] = _delete_obj;
    //obj_[delete_index]->set_poolid(delete_index);
    T *swap_obj = obj_[header->poolposition];
    char *pointer = reinterpret_cast<char *>(_delete_obj);
    char *swappointer = reinterpret_cast<char *>(swap_obj);
    uint32_t datasize = sizeof(T) + per_datasize_;
    memcpy(pointer, swappointer, datasize);
    obj_[header->poolposition]->set_poolid(static_cast<uint32_t>(ID_INVALID));
    header->unlock(kFlagMixedWrite); //Safe unlock header.
  __LEAVE_FUNCTION
}

template <typename T>
T *UnitPool<T>::get_obj(int32_t index) {
  __ENTER_FUNCTION
    Assert(index >= 0 && static_cast<uint32_t>(index) < max_size_);
    return obj_[index];
  __LEAVE_FUNCTION
    return NULL;
}

template <typename T>
uint32_t UnitPool<T>::get_max_size() {
  __ENTER_FUNCTION
    return max_size_;
  __LEAVE_FUNCTION
    return 0;
}

template <typename T>
int32_t UnitPool<T>::get_position() {
  __ENTER_FUNCTION
    Assert(ref_obj_pointer_);
    if (!ref_obj_pointer_) return -1;
    header_t *header = ref_obj_pointer_->getheader();
    Assert(header);
    header->lock(kFlagMixedRead);
    int32_t position = header->poolposition;
    header->unlock(kFlagMixedRead);
    return position;
  __LEAVE_FUNCTION
    return -1;
}

template <typename T>
uint32_t UnitPool<T>::get_key() {
  __ENTER_FUNCTION
    return key_;
  __LEAVE_FUNCTION
    return 0;
}

template <typename T>
bool UnitPool<T>::dump(const char *filename) {
 __ENTER_FUNCTION
   Assert(ref_obj_pointer_);
   if (!ref_obj_pointer_) return false;
   return ref_obj_pointer_->dump(filename);
 __LEAVE_FUNCTION
   return false;
}

template <typename T>
bool UnitPool<T>::merge_from_file(const char *filename) {
  __ENTER_FUNCTION
    Assert(ref_obj_pointer_);
    if (!ref_obj_pointer_) return false;
    ref_obj_pointer_->merge_from_file(filename);
  __LEAVE_FUNCTION
    return false;
}

template <typename T>
uint32_t UnitPool<T>::get_head_version() {
  __ENTER_FUNCTION
    uint32_t version = 0;
    Assert(ref_obj_pointer_);
    if (!ref_obj_pointer_) return false;
    header_t *header = ref_obj_pointer_->getheader();
    header->lock(kFlagMixedRead);
    version = header->version;
    header->unlock(kFlagMixedRead);
    return version;
  __LEAVE_FUNCTION
    return 0;
}

template <typename T>
void UnitPool<T>::set_head_version(uint32_t version) {
  __ENTER_FUNCTION
    Assert(ref_obj_pointer_);
    if (!ref_obj_pointer_) return;
    header_t *header = ref_obj_pointer_->getheader();
    header->lock(kFlagMixedWrite);
    header->version = version;
    header->unlock(kFlagMixedWrite);
  __LEAVE_FUNCTION
}

template <typename T>
header_t *UnitPool<T>::getheader() {
  __ENTER_FUNCTION
    Assert(ref_obj_pointer_);
    header_t *header = ref_obj_pointer_->getheader();
    return header;
  __LEAVE_FUNCTION
    return NULL;
}

template <typename T>
void UnitPool<T>::set_position(uint32_t position) {
  position_ = position;
}

template <typename T>
void data_template<T>::lock(int8_t type) {
  share::lock(&head.flag, type);
}

template <typename T>
void data_template<T>::unlock(int8_t type) {
  share::unlock(&head.flag, type); 
}
   
template <typename T>
int32_t data_template<T>::get_datasize() const {
  return datasize;
}
   
template <typename T>
void data_template<T>::set_datasize(int32_t size) {
  datasize = size;
}

template <typename T>
char *data_template<T>::getdata() {
  __ENTER_FUNCTION
    char *data = ((char *)this) + sizeof(data_template<T>);
    return data;
  __LEAVE_FUNCTION
    return NULL;
}
template <typename T>
data_template<T> &data_template<T>::operator = (const data_template &object) {
  __ENTER_FUNCTION
    head = object.head;
    data = object.data;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
template <typename T>
data_template<T> *data_template<T>::operator = (const data_template *object) {
  __ENTER_FUNCTION
    if (object) {
      head = object->head;
      data = object->data;
    }
    return this;
  __LEAVE_FUNCTION
    return this;
}

template <typename T>
void data_template<T>::set_poolid(uint32_t id) {
  head.poolid = id;
}

template <typename T>
uint32_t data_template<T>::get_poolid() const {
  return head.poolid;
}

template <typename T>
void data_template<T>::set_usestatus(int32_t status, int8_t type) {
  lock(type);
  head.usestatus = status;
  unlock(type);
}

template <typename T>
int32_t data_template<T>::get_usestatus(int8_t type) {
  int32_t status;
  lock(type);
  status = head.usestatus;
  unlock(type);
  return status;
}
template <typename T>
int32_t data_template<T>::single_usestatus() const {
  return head.usestatus;
}

template <typename T>
uint32_t data_template<T>::get_savetime(int8_t type) {
  uint32_t savetime;
  lock(type);
  savetime = head.savetime;
  unlock(type);
  return savetime;
}

template <typename T>
void data_template<T>::set_savetime(uint32_t time, int8_t type) {
  lock(type);
  head.savetime = time;
  unlock(type);
}

template <typename T>
uint32_t data_template<T>::getminutes(char type) {
  uint32_t minutes;
  lock(type);
  minutes = head.minutes;
  unlock(type);
  return minutes;
}

template <typename T>
void data_template<T>::setminutes(uint32_t minutes, int8_t type) {
  lock(type);
  head.minutes = minutes;
  unlock(type);
}

template <typename T>
void data_template<T>::init() {
  clear();
}

template <typename T>
void data_template<T>::clear() {
  head.clear();
  data.clear();
}

template <typename T>
void data_template<T>::headclear() {
  head.clear();
}

template <typename T>
Node<T>::Node() {
  cleanup();
  pool_ = new UnitPool<T>;
  Assert(pool_);
}

template <typename T>
Node<T>::~Node() {
  SAFE_DELETE(pool_);
}

template <typename T>
void Node<T>::cleanup() {
  __ENTER_FUNCTION
    isready_ = false;
    final_savetime_ = 0;
    last_checktime_ = 0;
    lastversion_ = 0;
    pool_ = NULL;
    key_ = 0;
    recover_ = false;
    readflag_ = kFlagFree;
    writeflag_ = kFlagFree;
    saveinterval_ = SYS_MEMORY_SHARENODE_SAVEINTERVAL;
    function_savenode_ = NULL;
    savecount_pertick_ = SYS_MEMORY_SHARENODE_SAVECOUNT_PERTICK;
  __LEAVE_FUNCTION
}

template <typename T>
bool Node<T>::init(int32_t sizemax, int32_t key) {
  __ENTER_FUNCTION
    if (is_null(pool_)) return false;
    if (!pool_->init(sizemax, key, kSmptShareMemory)) return false;
    pool_->set_head_version(0);
    last_checktime_ = TIME_MANAGER_POINTER->get_tickcount();
    lastversion_ = 0;
    bool result = initafter();
    return result;
  __LEAVE_FUNCTION
    return false;
}

template <typename T>
bool Node<T>::initafter() {
  __ENTER_FUNCTION
    if (is_null(pool_)) return false;
    if (GLOBAL_VALUES["app_cmdmodel"] == kCmdModelClearAll) return true;
    int32_t poolsize = get_poolcount();
    if (recover_) {
      for (int32_t i = 0; i < poolsize; ++i) {
        T *object = pool_->get_obj(i);
        if (is_null(object)) {
          Assert(object);
          return false;
        }
        int32_t usestatus = object->get_usestatus(writeflag_);
        if (kUseSendFree == usestatus) {
          object->set_usestatus(kUseReadyFree, writeflag_);
        }
      }
      SLOW_LOG(APPLICATION_NAME, 
               "[sys.memory.share] Node<T>::initafter recover");
      return true;
    }
    for (int32_t i = 0; i < poolsize; ++i) {
      T *object = pool_->get_obj(i);
      if (is_null(object)) {
        Assert(object);
        return false;
      }
      object->headclear();
      object->get_usestatus(writeflag_);
      object->set_usestatus(kUseFree, writeflag_);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <typename T>
bool Node<T>::tick() {
  __ENTER_FUNCTION
    uint32_t time = TIME_MANAGER_POINTER->get_tickcount();
    uint32_t diff = static_cast<uint32_t>(
	  fabs(static_cast<double>(time - last_checktime_)));
    if (diff > SYS_MEMORY_SHARENODE_DETECT_IDLE) {
      last_checktime_ = time;
      uint32_t version = pool_->get_head_version();
      if (version == lastversion_ && lastversion_ > 0) {
        lastversion_ = 0;
        SLOW_LOG(APPLICATION_NAME, "receive crash command.");
        bool result = flush(true, true);
        pool_->set_head_version(0);
        return result;
      }
    }
    
    if (GLOBAL_VALUES["appexit"] == 1) {
      flush(true, false);
    } else {
      tickflush();
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}
   
template <typename T>
bool Node<T>::empty() {
  __ENTER_FUNCTION
    if (is_null(pool_)) return false;
    int32_t poolsize_max = pool_->get_max_size();
    for (int32_t i = 0; i < poolsize_max; ++i) {
      T *object = pool_->get_obj(i);
      if (is_null(object)) {
        Assert(object);
        return false;
      }
      object->clear();
      int32_t usestatus = object->get_usestatus(writeflag_);
      object->set_usestatus(kUseFree, writeflag_);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <typename T>
bool Node<T>::tickflush() {
  __ENTER_FUNCTION
    bool result = flush(false, false);
    return result;
  __LEAVE_FUNCTION
    return false;
}

template <typename T>   
void Node<T>::set_readflag(int8_t flag) {
  readflag_ = flag;
}

template <typename T>   
void Node<T>::set_poolposition(uint32_t position) {
  if (pool_) pool_->set_position(position);
}

template <typename T>
UnitPool<T> *Node<T>::getpool() {
  return pool_;
}

template <typename T>   
void Node<T>::set_writeflag(int8_t flag) {
  writeflag_ = flag;
}

template <typename T>   
void Node<T>::set_saveinterval(uint32_t time) {
  saveinterval_ = time;
}

template <typename T>
void Node<T>::set_fuction_savenode(function_savenode function) {
  function_savenode_ = function;
}

template <typename T>
void Node<T>::set_savecount_pertick(int8_t count) {
  savecount_pertick_ = count;
}

template <typename T>
void Node<T>::set_dbmanager(pf_db::Manager * manager) {
  dbmanager_ = manager;
}

template <typename T>
uint32_t Node<T>::getkey() const {
  __ENTER_FUNCTION
    uint32_t key = 0;
    if (!is_null(pool_)) key = pool_->get_key();
    return key;
  __LEAVE_FUNCTION
    return 0;
}

template <typename T>
uint32_t Node<T>::get_poolcount() const {
  __ENTER_FUNCTION
    uint32_t count = 0;
    if (!is_null(pool_)) count = pool_->get_position();
    return count;
  __LEAVE_FUNCTION
    return 0;
}

template <typename T>
uint32_t Node<T>::get_poolcount_max() const {
  __ENTER_FUNCTION
    uint32_t count = 0;
    if (!is_null(pool_)) count = pool_->get_max_size();
    return count;
  __LEAVE_FUNCTION
    return 0;
}

template <typename T>
bool Node<T>::isfull() const {
  __ENTER_FUNCTION
    bool result = true;
    if (!is_null(pool_)) 
      result = pool_->isfull();
    return result;
  __LEAVE_FUNCTION
    return true;
}

template <typename T>
int32_t Node<T>::get_holddata_count() const {
  __ENTER_FUNCTION
    if (is_null(pool_)) return 0;
    int32_t poolsize = get_poolcount();
    int32_t hold_datacount = 0;
    int32_t i = 0;
    for (i = 0; i < poolsize; ++i) {
      T *object = pool_->get_obj(i);
      if (is_null(object)) {
        Assert(object);
        return 0;
      }
      int32_t usestatus = object->single_usestatus();
      if (kUseHoldData == usestatus || 
          kUseReadyFree == usestatus || 
          kUseSendFree == usestatus) ++hold_datacount;
    }
    return hold_datacount;
  __LEAVE_FUNCTION
    return 0;
}

template <typename T>   
bool Node<T>::flush(bool force, bool) {
  __ENTER_FUNCTION
    if (is_null(pool_)) return false;
    int32_t poolsize = pool_->get_position();
    int32_t hold_datacount = 0;
    int32_t wait_savecount = 0;
    int32_t i = 0;
    for (i = 0; i < poolsize; ++i) {
      T *object = pool_->get_obj(i);
      if (is_null(object)) {
        Assert(object);
        return false;
      }
      int32_t usestatus = object->single_usestatus();
      if (kUseHoldData == usestatus || 
          kUseReadyFree == usestatus || 
          kUseSendFree == usestatus) ++hold_datacount;
      if (kUseReadyFree == usestatus || kUseSendFree == usestatus) 
        ++wait_savecount;
    }
    int32_t current_ticksavecount = 0;
    static int32_t saveoffset = 0;
    i = saveoffset;
    if (i > poolsize) {
      i = 0;
      saveoffset = 0;
    }
    
    for (; i < poolsize; ++i) {
      T *object = pool_->get_obj(i);
      if (is_null(object)) {
        Assert(object);
        return false;
      }

      uint32_t time = TIME_MANAGER_POINTER->get_tickcount();
      uint32_t savetime = object->get_savetime(readflag_);
      /* cache time { */
      uint32_t minutes = object->getminutes(readflag_);
      if (minutes != 0) {
        if ((savetime + minutes * 60 * 1000) < time) {
          save(i);
          remove(i);
        }
      }
      /* } cache time */

      int32_t usestatus = object->get_usestatus(readflag_);
      if (kUseHoldData == usestatus || 
          kUseReadyFree == usestatus || 
          kUseSendFree == usestatus) {
        if (time > savetime || kUseReadyFree == usestatus || force) {
          /* force { */
          if (force) {
            if (time < (savetime - saveinterval_ + 10000)) {
              if (time < (savetime - saveinterval_)) {
                object->set_savetime(time + saveinterval_, writeflag_);
              }
              continue;
            }
          }
          /* } force */
          
          if (!save(i)) {
            SLOW_ERRORLOG(APPLICATION_NAME,
                          "[sys.memory.share] Node<T>::flush save failed"
                          " key: %d, pool index: %d",
                          key_,
                          i);
            return false;
          } else {
            if (current_ticksavecount > savecount_pertick_) return true;
          }

        } //if (time > save ...
      }
    } //for
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <typename T>
void Node<T>::setminutes(uint32_t index, uint32_t minutes) {
  __ENTER_FUNCTION
    if (is_null(pool_)) return;
    uint32_t poolsize_max = pool_->get_max_size();
    if (index >= poolsize_max) return;
    T *object = pool_->get_obj(index);
    if (!is_null(object)) {
      object->setminutes(minutes, writeflag_);
    }
  __LEAVE_FUNCTION
}

template <typename T>
bool Node<T>::save(uint32_t index) {
  __ENTER_FUNCTION
    if (is_null(pool_)) return false;
    uint32_t time = TIME_MANAGER_POINTER->get_tickcount();
    uint32_t poolsize_max = pool_->get_max_size();
    if (index >= poolsize_max) return false;
    T *object = pool_->get_obj(index);
    object->head.key = getkey();
    if (!is_null(object) && !is_null(function_savenode_)) {
      if ((*function_savenode_)(object, dbmanager_)) {
        object->set_savetime(time + saveinterval_, writeflag_);
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <typename T>
bool Node<T>::remove(uint32_t index) {
  __ENTER_FUNCTION
    if (is_null(pool_)) return false;
    uint32_t poolsize_max = pool_->get_max_size();
    if (index >= poolsize_max) return false;
    T *object = pool_->get_obj(index);
    pool_->delete_obj(object);
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <typename T>
T *Node<T>::get(uint32_t index) {
  __ENTER_FUNCTION
    T *result = NULL;
    if (is_null(pool_)) return result;
    uint32_t poolsize_max = pool_->get_max_size();
    if (index >= poolsize_max) return result;
    result = pool_->get_obj(index);
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

template <typename T>
T *Node<T>::getnew() {
  __ENTER_FUNCTION
    T *result = NULL;
    if (is_null(pool_)) return result;
    result = pool_->new_obj();
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

template <typename T>
void Node<T>::destory() {
  __ENTER_FUNCTION
    if (is_null(pool_)) return;
    pool_->release();
    SAFE_DELETE(pool_);
    cleanup();
  __LEAVE_FUNCTION
}

}; //namespace share

}; //namespace memory

}; //namespace pf_sys

#endif //PF_SYS_MEMORY_SHARE_TPP_
