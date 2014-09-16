/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id singleton.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/18 16:05
 * @uses Template class for creating single-instance global classes.
 */
#ifndef PF_BASE_SINGLETON_H_
#define PF_BASE_SINGLETON_H_

#include "pf/base/config.h"

namespace pf_base {

template <typename T> class Singleton {
 
 public:
   Singleton() {
     Assert(!singleton_);
     singleton_ = static_cast<T *>(this);
   }

   ~Singleton() {
     Assert(singleton_);
     singleton_ = NULL;
   }

   static T &getsingleton() {
     Assert(singleton_);
     return *singleton_;
   }

   static T *getsingleton_pointer() {
     return singleton_;
   }

 protected:
   static T *singleton_;

 private:
   Singleton(const Singleton<T> &);
   Singleton& operator=(const Singleton<T> &);

};

}; //namespace pf_base

#endif //PF_BASE_SINGLETON_H_
