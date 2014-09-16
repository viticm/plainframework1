/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id base.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/27 14:55
 * @uses script cache base class
 */
#ifndef PF_SCRIPT_CACHE_BASE_H_
#define PF_SCRIPT_CACHE_BASE_H_

#include "pf/script/cache/config.h"

namespace pf_script {

namespace cache {

class PF_API Base {

 public:
   Base();
   ~Base();
   
 public:
   virtual bool init(int32_t id, const char *filename, FILE* fp);
   virtual void clear();

 public:
   int32_t getid() const;
   const char *get_filename() const;

 protected:
   int32_t id_;
   char filename_[FILENAME_MAX];

};

}; //namespace cache

}; //namespace pf_script

#endif //PF_SCRIPT_CACHE_BASE_H_
