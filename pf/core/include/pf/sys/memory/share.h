/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id share.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/30 14:59
 * @uses system share memory model
 */
#ifndef PF_SYS_MEMORY_SHARE_H_
#define PF_SYS_MEMORY_SHARE_H_

#include "pf/sys/memory/config.h"
#include "pf/db/config.h"
#include "pf/base/log.h"

#define SYS_MEMORY_SHARE_MANAGER_UNITDATA_MAX 5000
#define SYS_MEMORY_SHARE_OBJECT_MAX 20

namespace pf_sys {

namespace memory {

namespace share {

typedef bool (__stdcall *function_savenode)(void *, void *);

PF_API void lock(atword_t *flag, int8_t type);
PF_API void unlock(atword_t *flag, int8_t type);

typedef struct PF_API header_struct header_t;

struct header_struct {                                                 
  uint64_t key;                                                                    
  uint32_t size;                                                                   
  uint32_t version;                                                                
  uint32_t poolposition;
  atword_t flag;
  header_struct();                                                             
  ~header_struct();
  void clear();
  void lock(int8_t type);
  void unlock(int8_t type);
}; 

typedef struct PF_API dataheader_struct dataheader_t;
struct dataheader_struct {
  int32_t key;
  int32_t usestatus;
  atword_t flag;
  uint32_t version;
  int32_t poolid;
  uint32_t savetime;
  uint32_t minutes;
  void clear();
  dataheader_t &operator = (const dataheader_t &object);
  dataheader_t *operator = (const dataheader_t *object);
  dataheader_struct();
  ~dataheader_struct();
};

template <typename T>
struct data_template {
  dataheader_t head;
  int32_t datasize;
  T data;
  void lock(int8_t type);
  void unlock(int8_t type);
  void set_poolid(uint32_t id);
  uint32_t get_poolid() const;
  void set_usestatus(int32_t status, int8_t type);
  int32_t get_usestatus(int8_t type);
  int32_t single_usestatus() const;
  uint32_t get_savetime(int8_t type);
  void set_savetime(uint32_t time, int8_t type);
  void setminutes(uint32_t minutes, int8_t type);
  uint32_t getminutes(char int8_t);
  int32_t get_datasize() const;
  char *getdata();
  void set_datasize(int32_t size);
  void init();
  void headclear();
  void clear();
  data_template &operator = (const data_template &object);
  data_template *operator = (const data_template *object);
};

typedef enum {
  kSmptShareMemory,
} pooltype_t;

enum {
  kFlagFree = -1,
  kFlagSelfRead = 0x01, //共享内存自己读取
  kFlagSelfWrite = 0x02, //共享内存自己写
  kFlagMixedRead = 0x03, //混合内存读取
  kFlagMixedWrite = 0x04, //混合内存写
}; //共享内存的占用状态

typedef enum {
  kUseFree = 0,
  kUseReadyFree = 1,
  kUseFreed = 2,
  kUseHoldData = 3,
  kUseSendFree = 4
} use_t; //共享内存的使用状态

//static define --

namespace api {

#if __LINUX__
PF_API int32_t create(uint32_t key, uint32_t size);
PF_API int32_t open(uint32_t key, uint32_t size);
PF_API void close(int32_t handle);
PF_API char *map(int32_t handle);
#elif __WINDOWS__
PF_API HANDLE create(uint32_t key, uint32_t size);
PF_API HANDLE open(uint32_t key, uint32_t size);
PF_API void close(HANDLE handle);
PF_API char *map(HANDLE handle);
#endif
PF_API void unmap(char *pointer);

}; //namespace api

class PF_API Base {

 public:
   int32_t cmd_model_; //命令行模式，几种模式在config.h定义
   Base();
   ~Base();

 public:
   bool create(uint32_t key, uint32_t size);
   void destory();
   bool attach(uint32_t key, uint32_t size);
   char *get_data_pointer();
   char *get_data(uint32_t size, uint32_t index);
   uint32_t get_size() const;
   bool dump(const char *filename);
   bool merge_from_file(const char *filename);
   header_t *getheader();

 private:
   uint32_t size_;
   char *data_pointer_;
   char *header_;
#if __LINUX__
   int32_t handle_;
#elif __WINDOWS__
   HANDLE handle_;
#endif
  
};

template <typename T> //template class must be in one file
class UnitManager {
 
 public:
   T *data_[SYS_MEMORY_SHARE_MANAGER_UNITDATA_MAX];
   int32_t count_;

 public:
   UnitManager();
   ~UnitManager();
   void init();
   bool heartbeat(uint32_t time = 0);
   bool add_data(T *data);
   bool delete_data(T *data);
   T *get_data(uint16_t id);

};

template <typename T> //模板类只能定义在一个文件内
class UnitPool {

 public:
   UnitPool();
   ~UnitPool();
   bool init(uint32_t max_count, uint32_t key, uint8_t pooltype);
   bool release();
   T *new_obj();
   void delete_obj(T *obj);
   T *get_obj(int32_t index);
   uint32_t get_max_size();
   int32_t get_position();
   uint32_t get_key();
   bool dump(const char *filename);
   bool merge_from_file(const char *filename);
   bool isfull() const;
   uint32_t get_head_version();
   void set_head_version(uint32_t version);
   void set_datasize(uint32_t size);
   void set_position(uint32_t position);
   header_t *getheader();

 protected:
   T **obj_;
   uint32_t max_size_;
   int32_t position_;
   uint32_t per_datasize_; //每个数据可以扩展的大小，实际每个数据的内存为sizeof(T)+per_datasize_
   Base *ref_obj_pointer_;
   uint32_t key_;

};

template <typename T>
class Node {

 public:
   Node();
   ~Node();

 public:
   bool init(int32_t sizemax, int32_t key);
   void cleanup();
   bool initafter();
   bool tick();
   bool empty();
   bool tickflush();
   uint32_t getkey() const;
   uint32_t get_poolcount() const;
   uint32_t get_poolcount_max() const;
   void set_poolposition(uint32_t position);
   void set_readflag(int8_t flag);
   void set_writeflag(int8_t flag);
   void set_saveinterval(uint32_t time);
   void set_dbmanager(pf_db::Manager *dbmanger);
   void set_savecount_pertick(int8_t count);
   void setrecover(bool recover);
   void set_fuction_savenode(function_savenode function);
   bool flush(bool force, bool crash = false);
   void setminutes(uint32_t index, uint32_t minutes);
   int32_t get_holddata_count() const;
   UnitPool<T> *getpool();
   bool isfull() const;
   bool save(uint32_t index);
   bool remove(uint32_t index);
   T *get(uint32_t index);
   T *getnew();
   void destory();

 private:
   bool isready_;
   uint32_t final_savetime_;
   uint32_t last_checktime_;
   uint32_t lastversion_;
   uint32_t saveinterval_;
   int8_t writeflag_; //The memory write flag.
   int8_t readflag_; //The memory read flag.
   UnitPool<T> *pool_;
   int32_t key_;
   bool recover_;
   function_savenode function_savenode_;
   pf_db::Manager *dbmanager_;
   int8_t savecount_pertick_;

};

}; //namespace share

}; //namespace memory

}; //namespace pf_sys

#include "pf/sys/memory/share.tpp"

#endif //PF_SYS_MEMORY_SHARE_H_
