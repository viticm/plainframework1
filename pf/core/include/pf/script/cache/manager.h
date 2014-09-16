/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id manager.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/27 15:25
 * @uses script cache manager class
 */
#ifndef PF_SCRIPT_CACHE_MANAGER_H_
#define PF_SCRIPT_CACHE_MANAGER_H_

#include "pf/script/cache/config.h"
#include "pf/base/singleton.h"
#include "pf/script/cache/base.h"

namespace pf_script {

namespace cache {

class PF_API Manager : public pf_base::Singleton<Manager> {

 public:
   Manager();
   ~Manager();

 public:
   static Manager *getsingleton_pointer();
   static Manager &getsingleton();

 public:
   bool init(const char *filename, bool force_arraymode);
   void clear();
   Base *get_filedata(uint32_t id);
   const Base *get_const_filedata(uint32_t id) const;
   int32_t getcount() const;
   Base *get_filedata_byindex(int32_t index);
   bool isinit() const;

 protected:
   static const char *get_rootpath();
   static int32_t compare_with_filedata_id(const void *arg1, const void *arg2);
   bool read_tablesize_check(int32_t tablecount);
   void sort_filedata_array();

 private:
   bool isinit_;
   int32_t count_;
   Base *filedata_set_;

};

}; //namespace cache

}; //namespace pf_script

#define SCRIPT_CACHE_MANAGER_POINTER \
pf_script::cache::Manager::getsingleton_pointer()

#endif //PF_SCRIPT_CACHE_MANAGER_H_
