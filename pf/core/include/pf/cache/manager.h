/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/pap )
 * $Id manager.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/10 17:16
 * @uses cache manager class
 */
#ifndef PF_CACHE_MANAGER_H_
#define PF_CACHE_MANAGER_H_

#include "pf/cache/config.h"

namespace pf_cache {

class PF_API Manager {

 public:
   Manager();
   ~Manager();

 public:
   Repository *create_dbdirver();
   Repository *get_dbdirver();

 private:
   Repository *dbdirver_;

};

}; //namespace pf_cache

#endif //PF_CACHE_MANAGER_H_
