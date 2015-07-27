/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/pap )
 * $Id repository.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/10 15:27
 * @uses cache repository class
 */
#ifndef PF_CACHE_REPOSITORY_H_
#define PF_CACHE_REPOSITORY_H_

#include "pf/cache/config.h"
#include "pf/base/hashmap/template.h"

namespace pf_cache {

class PF_API Repository {

 public:
   Repository(StoreInterface *store);
   ~Repository();

 public:
   
   //Determine if an item exists in the cache.
   bool has(const char *key);
   
   //Retrieve an item from the cache by key.
   void *get(const char *key, void *_default = NULL);

   //Retrieve an item from the cache and delete it.
   void *pull(const char *key, void *_default = NULL);

   //Store an item in the cache.
   void put(const char *key, void *value, int32_t minutes = 0);

   //Store an item in the cache if the key does not exist.
   void add(const char *key, void *value, int32_t minutes = 0);

   //Get an item from the cache, or store the default value.
   void *remember(const char *key, int32_t minutes, function_callback callback);

   //Get an item from the cache, or store the default value forever.
   void *sear(const char *key, function_callback callback);

   //Get an item from the cache, or store the default value forever.
   void *remember_forever(const char *key, function_callback callback);

   void forget(const char *key);

   //Get the default cache time.
   int32_t get_default_cachetime() const;

   //Set the default cache time in minutes.
   void set_default_cachetime(int32_t minutes);

   //Get the cache store implementation.
   StoreInterface *getstore();

   //Set an item cache time.
   void set_cachetime(const char *key, int32_t minutes);
   
   //Tick for logic.
   void tick();

   //Register a key to share memory.
   bool register_sharekey(int32_t key, int32_t size, int32_t datasize = 0);

   //Retrieve node pointer of share memory.
   void *get_sharenode(int32_t key);

   //Set the share memory key max count in cache.
   bool set_sharekey_count(int32_t count);

   //Register a key to share memory extend.
   bool register_sharekeyex(int32_t key, int32_t size, int32_t datasize = 0);

   //Retrieve node pointer of share memory extend.
   void *get_sharenodeex(int32_t key);

   //Set the share memory key max count in cache extend.
   bool set_sharekey_countex(int32_t count);

   //Set cache count.
   bool set_cachecount(int32_t count);

   //Store an item can recycle when the cache is full.
   void recycle(const char *key);

   //Set the share service.
   void set_shareservice(bool flag);

   //Retrieve share pool pointer of share memory extend.
   void *get_sharepoolex(int32_t key);

   //Retrieve share pool pointer of share memory.
   void *get_sharepool(int32_t key);
   
   //Get key hash.
   void *get_keyhash();

   //Register the key to key hash.
   bool register_keyhash(int32_t key, int32_t size, bool create = false);

 protected:

   //The cache store implementation.
   StoreInterface *store_;

   //The default number of minutes to store items.
   int32_t minutes_;

};

}; //namespace pf_cache

#endif //PF_CACHE_REPOSITORY_H_
