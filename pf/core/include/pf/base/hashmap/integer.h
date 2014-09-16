/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id integerex.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/27 12:09
 * @uses base hashmap module integer extend template class 
 */
#ifndef PF_BASE_HASHMAP_INTEGEREX_H_
#define PF_BASE_HASHMAP_INTEGEREX_H_

#include "pf/base/hashmap/config.h"

namespace pf_base {

namespace hashmap {

class PF_API Integer {

 public:
   typedef hash_map<uint32_t, void *> hashmap_t;
   typedef hashmap_t::iterator iterator_t;

 public:
   Integer();
   ~Integer();

 public:
   void init(uint32_t count);
   bool add(uint32_t id, void *pointer);
   void *get(uint32_t);
   bool remove(uint32_t);
   uint32_t getcount() const;
   uint32_t get_maxcount() const;
   void clear();
   iterator_t begin();
   iterator_t end();

 private:
   hashmap_t hashmap_;
   iterator_t iterator_;
   uint32_t maxcount_;

};

}; //namespace hashmap

}; //namespace pf_base

#endif //PF_BASE_HASHMAP_INTEGEREX_H_
