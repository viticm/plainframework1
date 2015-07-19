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
   Template();
   ~Template();

 public:
   Template(const Template &object);
   Template(const Template *object);
   Template &operator = (const Template &object);
   Template *operator = (const Template *object);
   void init(uint32_t count);
   bool add(T_key key, T_value value);
   bool set(T_key key, T_value value);
   T_value get(T_key key);
   bool isfind(T_key key);
   bool remove(T_key key);
   void clear();
   uint32_t getcount() const;
   uint32_t get_maxcount() const;
   void set_maxcount(uint32_t count);
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

#include "pf/base/hashmap/template.tpp"

#endif //PF_BASE_HASHMAP_TEMPLATE_H_
