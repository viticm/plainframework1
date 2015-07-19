#include "pf/sys/memory/sharemap.h"

using namespace pf_sys::memory::share;
mapnode_struct::mapnode_struct(){
  clear();
}
  
void mapnode_struct::clear() {
  prev = INDEX_INVALID;
  hash = 0;
  next = INDEX_INVALID;
}  

mapbucket_struct::mapbucket_struct() {
  clear();
}
  
void mapbucket_struct::clear() {
  cur = INDEX_INVALID;
}

MapPool::MapPool() {
  //do nothing
}

MapPool::~MapPool() {
  //do nothing
}

bool MapPool::init(uint32_t key, 
                   uint32_t countmax, 
                   uint32_t datasize, 
                   bool create) {
  __ENTER_FUNCTION
    set_datasize(datasize);
    ref_obj_pointer_ = new Base();
    Assert(ref_obj_pointer_);
    if (!ref_obj_pointer_) return false;
    ref_obj_pointer_->cmd_model_ = GLOBAL_VALUES["app_cmdmodel"].int32();
    bool result = true;
    bool needinit = false;
    uint32_t headersize = sizeof(header_t);
    uint32_t bucketsize = sizeof(mapbucket_t) * countmax;
    uint32_t full_datasize = (sizeof(mapnode_t) + per_datasize_) * countmax;
    uint32_t memorysize = headersize + bucketsize + full_datasize;
    result = ref_obj_pointer_->attach(key, memorysize);
    if (create && !result) {
      result = ref_obj_pointer_->create(key, memorysize);
      needinit = true;
    } else if (!result) {
      return false;
    }
    if (!result && kCmdModelClearAll == ref_obj_pointer_->cmd_model_) {
      return true;
    } else if (!result) {
      SLOW_ERRORLOG(
          "sharememory", 
          "[sys][sharememory] (UnitPool::init) failed");
      Assert(result);
      return result;
    }    
    max_size_ = countmax;
    position_ = 0;
    obj_ = new mapnode_t * [max_size_];
    if (is_null(obj_)) return false;
    uint32_t i;
    mapbucket_t *buckets = reinterpret_cast<mapbucket_t *>(getbuckets());
    for (i = 0; i < max_size_; ++i) {
      char *pointer = getdata(sizeof(mapnode_t) + per_datasize_, i);
      obj_[i] = reinterpret_cast<mapnode_t *>(pointer);
      if (NULL == obj_[i]) {
        Assert(false);
        return false;
      }
      obj_[i]->set_datasize(per_datasize_);
      if (per_datasize_ > 0 && needinit) {
        memset(&pointer[sizeof(mapnode_t)], 0, per_datasize_);
      }
      if (needinit) {
        buckets[i].clear();
        obj_[i]->init();
      }
    }    
    key_ = key;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void MapPool::delete_obj(mapnode_t *obj) {
  __ENTER_FUNCTION
    Assert(obj != NULL && ref_obj_pointer_ != NULL);
    header_t *header = ref_obj_pointer_->getheader();
    Assert(header);
    header->lock(kFlagMixedWrite); //Safe lock header.
    Assert(header->poolposition > 0);
    if (NULL == obj || header->poolposition <= 0) {
      header->unlock(kFlagMixedWrite);
      return;
    }
    uint32_t delete_index = obj->get_poolid(); //this function 
                                                //must define in T*
    Assert(delete_index < header->poolposition);
    if (delete_index >= header->poolposition) {
      header->unlock(kFlagMixedWrite);
      return;
    }
    --(header->poolposition);
    mapnode_t *node = obj_[delete_index];

    //Safe to swap list.
    mapnode_t *swapnode = obj_[header->poolposition];
    uint32_t datasize = sizeof(mapnode_t) + per_datasize_;
    char *pointer = reinterpret_cast<char *>(node);
    char *swappointer = reinterpret_cast<char *>(swapnode);
    memcpy(pointer, swappointer, datasize);
    if (node) {
      if (node->data.prev != ID_INVALID) {
        mapnode_t *prevnode = get_obj(node->data.prev);
        prevnode->lock(kFlagMixedWrite);
        prevnode->data.next = delete_index;
        prevnode->unlock(kFlagMixedWrite);
      }
    }

    //Safe to swap bucket.
    uint32_t _bucketindex = bucketindex(obj_[delete_index]->data.hash);
    char *_buckets = getbuckets();
    mapbucket_t *buckets = reinterpret_cast<mapbucket_t *>(_buckets);
    if (buckets[_bucketindex].cur == 
        static_cast<int32_t>(header->poolposition)) {
      buckets[_bucketindex].cur = delete_index;
    }
    header->unlock(kFlagMixedWrite); //Safe unlock header.
  __LEAVE_FUNCTION
}
uint32_t MapPool::bucketindex(uint32_t hash) {
  __ENTER_FUNCTION
    uint32_t index = hash & (get_max_size() - 1);
    return index;
  __LEAVE_FUNCTION
    return 0;
}
   
uint32_t MapPool::hashkey(const char *str) {
  __ENTER_FUNCTION
    uint32_t hash = 5381;
    while (*str) {
      hash = ((hash << 5) + hash) ^ *str++;
    }
    return hash;
  __LEAVE_FUNCTION
    return 0;
}

char *MapPool::getdata(uint32_t size, uint32_t index) {
  __ENTER_FUNCTION
    char *result = NULL;
    if (!ref_obj_pointer_) return result;
    char *data = ref_obj_pointer_->get_data_pointer();
    uint32_t bucketsize = sizeof(mapbucket_t) * max_size_;
    char *realdata = data + bucketsize;
    uint32_t data_fullsize = (sizeof(mapnode_t) + per_datasize_) * max_size_;
    Assert(size > 0);
    Assert(size * index <= data_fullsize - size);
    result = (size <= 0 || size * index > data_fullsize - size) ? 
              NULL : 
              realdata + size * index;
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

char *MapPool::getbuckets() {
  __ENTER_FUNCTION
    char *data = ref_obj_pointer_->get_data_pointer();
    return data;
  __LEAVE_FUNCTION
    return NULL;
}

//Map class.

Map::Map() {
  __ENTER_FUNCTION
    cleanup();
    pool_ = new MapPool;
    Assert(pool_);
  __LEAVE_FUNCTION
}

Map::~Map() {
  SAFE_DELETE(pool_);
}

bool Map::init(uint32_t key, 
               uint32_t countmax, 
               uint32_t keysize, 
               uint32_t valuesize,
               bool create) {
  __ENTER_FUNCTION
    uint32_t datasize = (keysize + 1) + (valuesize + 1);
    bool result = pool_->init(key, countmax, datasize, create);
    if (!result) return result;
    buckets_ = reinterpret_cast<mapbucket_t *>(pool_->getbuckets());
    Assert(buckets_);
    keysize_ = keysize;
    valuesize_ = valuesize;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Map::cleanup() {
  __ENTER_FUNCTION    
    pool_ = NULL;
    keysize_ = 0;
    valuesize_ = 0;
    buckets_ = NULL;
  __LEAVE_FUNCTION
}
   
const char *Map::get(const char *key) {
  __ENTER_FUNCTION
    const char *result = NULL;
    int32_t index = getref(key);
    if (index != INDEX_INVALID) {
      mapnode_t *node = pool_->get_obj(index);
      uint32_t valuepos = sizeof(mapnode_t) + keysize_ + 2;
      result = reinterpret_cast<char *>(node) + valuepos;
    }
    return result;
  __LEAVE_FUNCTION
    return NULL;
}
   
bool Map::set(const char *key, const char *value) {
  __ENTER_FUNCTION
    int32_t index = getref(key);
    uint32_t valuesize = static_cast<uint32_t>(strlen(value));
    mapnode_t *node = NULL;
    uint32_t valuepos = sizeof(mapnode_t) + keysize_ + 2;
    valuesize = valuesize > valuesize_ ? valuesize_ : valuesize;
    if (index != INDEX_INVALID) {
      node = pool_->get_obj(index);
      Assert(node);
      if (is_null(node)) return false;
      node->lock(kFlagMixedWrite);
      char *pointer = reinterpret_cast<char *>(node) + valuepos;
      memset(pointer, 0, valuesize_ + 1);
      memcpy(pointer, value, valuesize);
      node->unlock(kFlagMixedWrite);
    } else {
      node = newnode(key, value);
      if (is_null(node)) return false;
      addnode(node);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Map::remove(const char *key) {
  __ENTER_FUNCTION
    mapnode_t *node = NULL;
    int32_t index = getref(key);
    mapbucket_t *buckets = reinterpret_cast<mapbucket_t *>(pool_->getbuckets());
    header_t *header = pool_->getheader();
    uint32_t hash = 0;
    if (index != INDEX_INVALID) {
      node = pool_->get_obj(index);
      hash = node->data.hash;
      if (node->data.prev != INDEX_INVALID) {
        mapnode_t *prevnode = pool_->get_obj(node->data.prev);
        prevnode->lock(kFlagMixedWrite);
        if (prevnode) prevnode->data.next = node->data.next;
        prevnode->unlock(kFlagMixedWrite);
      }
      uint32_t _bucketindex = pool_->bucketindex(hash);
      if (_bucketindex >= 0 && _bucketindex < pool_->get_max_size()) {
        header->lock(kFlagMixedWrite);
        mapbucket_t *bucket = &buckets[_bucketindex];
        if (bucket->cur == static_cast<int32_t>(node->get_poolid())) 
          bucket->cur = node->data.next;
        header->unlock(kFlagMixedWrite);
      }
      pool_->delete_obj(node);
      //node->data.clear();
    }
  __LEAVE_FUNCTION
}

   
MapPool *Map::getpool() {
  return pool_;
}   

mapnode_t *Map::newnode(const char *key, const char *value) {
  __ENTER_FUNCTION
    mapnode_t *node = NULL;
    uint32_t keypos = sizeof(mapnode_t);
    uint32_t valuepos = keypos + keysize_ + 2;
    node = pool_->new_obj();
    if (is_null(node)) return node;
    char *pointer = reinterpret_cast<char *>(node);
    uint32_t keysize = static_cast<uint32_t>(strlen(key));
    uint32_t valuesize = static_cast<uint32_t>(strlen(value));
    uint32_t hash = pool_->hashkey(key);
    keysize = keysize > keysize_ ? keysize_ : keysize;
    valuesize = valuesize > valuesize_ ? valuesize_ : valuesize;
    node->lock(kFlagMixedWrite);
    node->data.hash = hash;
    memset(pointer + keypos, 0, keysize_ + 1);
    memcpy(pointer + keypos, key, keysize);
    memset(pointer + valuepos, 0, valuesize + 1);
    memcpy(pointer + valuepos, value, valuesize);
    node->unlock(kFlagMixedWrite);
    return node;
  __LEAVE_FUNCTION
    return NULL;
}
   
void Map::addnode(mapnode_t *node) {
  __ENTER_FUNCTION
    node->lock(kFlagMixedWrite);
    int32_t n = pool_->bucketindex(node->data.hash);
    mapbucket_t *buckets = reinterpret_cast<mapbucket_t *>(pool_->getbuckets());
    node->data.next = buckets[n].cur;
    if (node->data.next != INDEX_INVALID) {
      mapnode_t *_node = pool_->get_obj(node->data.next);
      _node->data.prev = node->get_poolid();
    }
    buckets[n].cur = node->get_poolid();
    node->unlock(kFlagMixedWrite);
  __LEAVE_FUNCTION
}
   
   
int32_t Map::getref(const char *key) {
  __ENTER_FUNCTION
    int32_t result = INDEX_INVALID;
    uint32_t hash = pool_->hashkey(key);
    mapbucket_t *buckets = reinterpret_cast<mapbucket_t *>(pool_->getbuckets());
    if (pool_->get_position() > 0) {
      uint32_t _bucketindex = pool_->bucketindex(hash);
      Assert(_bucketindex >= 0 && _bucketindex < pool_->get_max_size());
      mapbucket_t *bucket = &buckets[_bucketindex];
      uint32_t keypos = sizeof(mapnode_t);
      int32_t index = bucket->cur;
      while (index != INDEX_INVALID) {
        mapnode_t *node = pool_->get_obj(index);
        Assert(node);
        char *pointer = reinterpret_cast<char *>(node);
        if (node->data.hash == hash && 0 == strcmp(pointer + keypos, key)) {
          result = index;
          break;
        }
        index = node->data.next;
      }
    }
    return result;
  __LEAVE_FUNCTION
    return ID_INVALID;
}
