/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id integer.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/27 11:32
 * @uses base hashmap module template class
 */
#ifndef PF_BASE_HASHMAP_TEMPLATE_H_
#define PF_BASE_HASHMAP_TEMPLATE_H_

#include "pf/base/hashmap/config.h"

namespace pf_base {

namespace hashmap {

template <class T_key, class T_value>
class Template {
 public:
   typedef hash_map<T_key, T_value> hashmap_t;
   typedef typename hashmap_t::iterator iterator_t;

 public:
   Template() {
     maxcount_ = 0;
   }
   ~Template() {
     maxcount_ = 0;
   }
   void init(uint32_t count) {
     __ENTER_FUNCTION
       clear();
       maxcount_ = count;
     __LEAVE_FUNCTION
   }
   bool add(T_key key, T_value value) {
     __ENTER_FUNCTION
       if (hashmap_.size() >= maxcount_) return false;
       hashmap_.insert(typename hashmap_t::value_type(key, value));
       return true;
     __LEAVE_FUNCTION
       return false;
   }
   bool set(T_key key, T_value value) {
     __ENTER_FUNCTION
       iterator_t iterator = hashmap_.find(key);
       if (iterator != hashmap_.end()) {
         iterator->second = value;
         return true;
       }
       return false;
     __LEAVE_FUNCTION
       return false;
   }
   T_value get(T_key key) {
     __ENTER_FUNCTION
       iterator_t iterator = hashmap_.find(key);
       if (iterator != hashmap_.end()) return iterator->second;
       return 0;
     __LEAVE_FUNCTION
       return 0;
   }
   bool isfind(T_key key) {
     __ENTER_FUNCTION
       iterator_t iterator = hashmap_.find(key);
       if (iterator != hashmap_.end()) return true;
       return false;
     __LEAVE_FUNCTION
       return false;
   }
   bool remove(T_key key) {
     __ENTER_FUNCTION
       iterator_t iterator = hashmap_.find(key);
       if (iterator != hashmap_.end()) {
         hashmap_.erase(iterator);
         return true;
       }
       return false;
     __LEAVE_FUNCTION
       return false;
   }
   void clear() {
     __ENTER_FUNCTION
       hashmap_.clear();
     __LEAVE_FUNCTION
   }
   uint32_t getcount() const {
     return static_cast<uint32_t>(hashmap_.size());
   }
   uint32_t get_maxcount() const {
     return maxcount_;
   }
   iterator_t begin() {
     return hashmap_.begin();
   }
   iterator_t end() {
     return hashmap_.end();
   }

 private:
   hashmap_t hashmap_;
   iterator_t iterator_;
   uint32_t maxcount_;

};

}; //namespace hashmap

}; //namespace pf_base

#endif //PF_BASE_HASHMAP_TEMPLATE_H_
