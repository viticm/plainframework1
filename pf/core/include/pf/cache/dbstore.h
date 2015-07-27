/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/pap )
 * $Id dbstore.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/10 16:11
 * @uses db cache store class
 *       cn: 数据库缓存，可实现定时保存的功能，目前该功能的实现为共享内存结构
 *           共享内存key如果未被注册，则自动注册为默认大小
 */
#ifndef PF_CACHE_DBSTORE_H_
#define PF_CACHE_DBSTORE_H_

#include "pf/cache/config.h"
#include "pf/db/config.h"
#include "pf/sys/memory/sharemap.h"
#include "pf/cache/storeinterface.h"
#include "pf/cache/define.h"

namespace pf_cache {

class PF_API DBStore : public StoreInterface {

 public:
   DBStore();
   virtual ~DBStore();

 public:

   //Set the database manager pointer.
   void set_dbmanager(pf_db::Manager *manager);

 public:

   //Retrieve an item from the cache by key.
   virtual void *get(const char *key);

   //Store an item in the cache for a given number of minutes.
   virtual void put(const char *key, void *value, int32_t minutes);

   //Store an item in the cache indefinitely.
   virtual void forever(const char *key, void *value);

   //Remove an item from the cache.
   virtual void forget(const char *key);

   //Remove all items from the cache.
   virtual void flush();

   //Get the cache key prefix.
   virtual const char *getprefix() const;

   //Set an item cache time.
   virtual void set_cachetime(const char *key, int32_t minutes);
   
   //Tick for logic.
   virtual void tick();

   //Register a key to share memory.
   virtual bool register_sharekey(int32_t key, 
                                  int32_t size,
                                  int32_t datasize = 0);

   //Retrieve node pointer of share memory.
   virtual void *get_sharenode(int32_t key);

   //Set the share memory key max count in cache.
   virtual bool set_sharekey_count(int32_t count);

   //Set cache count.
   virtual bool set_cachecount(int32_t count);

   //Store an item can recycle when the cache is full.
   virtual void recycle(const char *key);

   //Register a key to share memory extend.
   virtual bool register_sharekeyex(int32_t key, 
                                    int32_t size, 
                                    int32_t datasize = 0);

   //Retrieve node pointer of share memory extend.
   virtual void *get_sharenodeex(int32_t key);

   //Retrieve share pool pointer of share memory extend.
   virtual void *get_sharepoolex(int32_t key);

   //Retrieve share pool pointer of share memory.
   virtual void *get_sharepool(int32_t key);
  
   //Set the share memory key max count in cache extend.
   virtual bool set_sharekey_countex(int32_t count);

   //Set the share service.
   virtual void set_shareservice(bool flag);

   //Get key hash.
   virtual void *get_keyhash();

   //Register the key to key hash.
   virtual bool register_keyhash(int32_t key, 
                                 int32_t size, 
                                 bool create = false);


 protected:
   
   //The database manager instance pointer.
   pf_db::Manager *dbmanager_;

   //The key index hash map, for share memory [key_name] = index.

   //Remember the hash map base types.
   //pf_base::hashmap::Template<pf_base::variable_t, int32_t> keyhash_;
   //std::map<std::string, int32_t> keyhash_;
   pf_sys::memory::share::Map *keyhash_;

   //The key index to the register share memory.
   std::vector<db_sharenode_t *> sharekeys_;
   
   //The key index to the register share memory extend.
   std::vector<db_sharenodeex_t *> sharekeysex_;

   //The key index to the share pool.
   std::vector<db_sharepool_t *> sharepools_;

   //The key index to the share pool extend.
   std::vector<db_sharepoolex_t *> sharepoolsex_;

   //The recycle keys.
   std::map<std::string, int8_t> recyclekeys_;

   //The share service is true then will try create share memory.
   bool shareservice_;


};

}; //namespace pf_cache

#endif //PF_CACHE_DBSTORE_H_
