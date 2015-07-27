/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework1 )
 * $Id sharemap.h
 * @link https://github.com/viticm/plainframework1 for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com/viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com/viticm.ti@gmail.com>
 * @date 2015/04/27 10:32
 * @uses share memory hash map
 */
#ifndef PF_SYS_MEMORY_SHAREMAP_H_
#define PF_SYS_MEMORY_SHAREMAP_H_

#include "pf/sys/memory/config.h"
#include "pf/sys/memory/share.h"

namespace pf_sys {

namespace memory {

namespace share {

typedef struct PF_API mapnode_struct _mapnode_t;

struct mapnode_struct {
  uint32_t hash;
  int32_t prev; //Prev index.
  int32_t next; //Node index.
  mapnode_struct();
  void clear();
};

typedef struct PF_API mapbucket_struct mapbucket_t;
struct mapbucket_struct {
  int32_t cur;
  mapbucket_struct();
  void clear();
};

typedef data_template<_mapnode_t> mapnode_t;


class PF_API MapPool : public UnitPool<mapnode_t> {

 public:
   MapPool();
   ~MapPool();

 public:
   bool init(uint32_t key, uint32_t countmax, uint32_t datasize, bool create);
   void delete_obj(mapnode_t *obj);
   char *getbuckets();
   uint32_t bucketindex(uint32_t hash);
   uint32_t hashkey(const char *str);

 private:
   char *getdata(uint32_t size, uint32_t index);

};

class PF_API Map {

 public:
   Map();
   ~Map();

 public:
   bool init(uint32_t key, 
             uint32_t countmax, 
             uint32_t keysize, 
             uint32_t valuesize,
             bool create = false);
   void cleanup();
   const char *get(const char *key);
   bool set(const char *key, const char *value);
   void remove(const char *key);
   MapPool *getpool();

 private:
   uint32_t keysize_;
   uint32_t valuesize_;
   MapPool *pool_;
   mapbucket_t *buckets_;

 private:
   mapnode_t *newnode(const char *key, const char *value);
   void addnode(mapnode_t *node);
   int32_t getref(const char *key);

};

}; //namespace share

}; //namespace memory

}; //namespace memory

#endif //PF_SYS_MEMORY_SHAREMAP_H_
