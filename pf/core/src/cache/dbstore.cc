#include "pf/base/string.h"
#include "pf/base/log.h"
#include "pf/cache/define.h"
#include "pf/cache/dbstore.h"

namespace pf_cache {

DBStore::DBStore() {
  __ENTER_FUNCTION
    GLOBAL_VALUES["core_cache"] = "cache";
    dbmanager_ = NULL;
    sharekeys_.clear();
    sharekeysex_.clear();
    sharepools_.clear();
    sharepoolsex_.clear();
    shareservice_ = false;
    keyhash_ = NULL;
  __LEAVE_FUNCTION
}

DBStore::~DBStore() {
  __ENTER_FUNCTION
    for (uint32_t i = 0; i < sharekeys_.size(); ++i)
      SAFE_DELETE(sharekeys_[i]);
    for (uint32_t i = 0; i < sharekeysex_.size(); ++i) {
      SAFE_DELETE(sharekeysex_[i]);
    }
    if (!shareservice_) {
      for (uint32_t i = 0; i < sharepools_.size(); ++i) {
        SAFE_DELETE(sharepools_[i]);
      }
      for (uint32_t i = 0; i < sharepoolsex_.size(); ++i) {
        SAFE_DELETE(sharepoolsex_[i]);
      }
    }
    sharekeys_.clear();
    sharekeysex_.clear();
    sharepools_.clear();
    sharepoolsex_.clear();
    SAFE_DELETE(keyhash_);
  __LEAVE_FUNCTION
}

void DBStore::set_dbmanager(pf_db::Manager *manager) {
  dbmanager_ = manager;
}

void *DBStore::get(const char *key) {
  __ENTER_FUNCTION
    using namespace pf_base;
    void *result = NULL;
    if (is_null(keyhash_)) return result;
    
    const char *hash = keyhash_->get(key);
    if (is_null(hash)) return result;
    //Recycle.
    if (recyclekeys_.find(key) != recyclekeys_.end()) {
      recyclekeys_.erase(key);
    }

    pf_base::variable_t temp = hash;
    int32_t index = temp.int32();
    std::vector<std::string> array;
    string::explode(key, array, "\t", true, true);
    if (array.size() != 2) return result; //Key must be "key_name".
    pf_base::variable_t _key;
    _key = array[0];
    db_sharepool_t *db_sharepool =
      reinterpret_cast<db_sharepool_t *>(get_sharepool(_key.int32()));
    if (!db_sharepool) {
      db_sharepool = 
        reinterpret_cast<db_sharepool_t *>(get_sharepoolex(_key.int32()));
    }
    if (!db_sharepool || index >= db_sharepool->get_position()) return result;
    result = reinterpret_cast<void *>(db_sharepool->get_obj(index));
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

void DBStore::put(const char *key, void *value, int32_t minutes) {
  __ENTER_FUNCTION
    using namespace pf_base;
    using namespace pf_sys::memory::share;
    if (is_null(value) || is_null(keyhash_)) return;
    
    //Recycle.
    if (recyclekeys_.find(key) != recyclekeys_.end()) {
      recyclekeys_.erase(key);
    }
    
    pf_base::variable_t sharekey = 0;
    pf_base::variable_t poolindex = 0;
    std::vector<std::string> array;
    string::explode(key, array, "\t", true, true);
    if (array.size() != 2) return; //Key must be "key_name".
    sharekey = array[0];
    db_sharepool_t *db_sharepool = 
      reinterpret_cast<db_sharepool_t *>(get_sharepool(sharekey.int32()));
    if (!db_sharepool) {
      db_sharepool =
        reinterpret_cast<db_sharepool_t *>(get_sharepoolex(sharekey.int32()));
    }
    if (!db_sharepool) {
      /**
      if (!register_sharekey(sharekey.int32(), CACHE_SHARE_POOLSIZE)) {
        SLOW_ERRORLOG(GLOBAL_VALUES["core_cache"].string(),
                      "[cache] (DBStore::put) register share key error"
                      " key: %d, value: %p, minutes: %d",
                      key,
                      value,
                      minutes);
        return;
      }
      db_sharenode = 
        reinterpret_cast<db_sharenode_t *>(get_sharenode(sharekey.int32()));
      **/
      return;
    }
    db_sharedata_t &db_sharedata = *((db_sharedata_t *)value);
    db_sharedata_t *db_savedata = NULL;
    const char *hash = keyhash_->get(key);
    if (!is_null(hash)) {
      variable_t temp = hash;
      poolindex = temp.int32();
      db_savedata = db_sharepool->get_obj(poolindex.uint32());
      *db_savedata = db_sharedata;
    } else {
      if (db_sharepool->isfull()) {
        pf_base::variable_t recyclekey;
        std::map<std::string, int8_t>::iterator _iterator;
        _iterator = recyclekeys_.begin();
        if (_iterator != recyclekeys_.end()) {
          recyclekey = _iterator->first;
          if (recyclekeys_.find(recyclekey.string()) != recyclekeys_.end()) {
            forget(recyclekey.string());
            recyclekeys_.erase(recyclekey.string());
          }
        }
      } //full and recycle keys

      db_savedata = db_sharepool->new_obj();
      if (!is_null(db_savedata)) {
        poolindex = db_savedata->get_poolid();
        keyhash_->set(key, poolindex.string());
      } else {
        SLOW_ERRORLOG(GLOBAL_VALUES["core_cache"].string(),
                      "[cache] (DBStore::put) share node is full"
                      " key: %d, value: %p, minutes: %d",
                      sharekey.int32(),
                      value,
                      minutes);
        return;
      }
    }

    //Set cahe time.
    if (!is_null(db_savedata)) {
      db_savedata->setminutes(static_cast<uint32_t>(minutes), kFlagMixedWrite);
    }
  __LEAVE_FUNCTION
}

void DBStore::forever(const char *key, void *value) {
  put(key, value, 0);
}

void DBStore::forget(const char *key) {
  __ENTER_FUNCTION
    using namespace pf_base;
    if (is_null(keyhash_)) return;
    const char *hash = keyhash_->get(key);
    if (is_null(hash)) return;
    pf_base::variable_t sharekey = 0;
    pf_base::variable_t poolindex = hash;
    std::vector<std::string> array;
    string::explode(key, array, "\t", true, true);
    if (array.size() != 2) return; //Key must be "key_name".
    sharekey = array[0];
    if (shareservice_) {
      db_sharenode_t *db_sharenode = 
        reinterpret_cast<db_sharenode_t *>(get_sharenode(sharekey.int32()));
      if (is_null(db_sharenode)) {
        db_sharenode =
          reinterpret_cast<db_sharenode_t *>(get_sharenodeex(sharekey.int32()));
      }
      if (is_null(db_sharenode)) return;
      db_sharenode->save(poolindex.int32());
      db_sharenode->remove(poolindex.int32());
    } else {
      db_sharepool_t *db_sharepool =
        reinterpret_cast<db_sharepool_t *>(get_sharepool(sharekey.int32()));
      if (is_null(db_sharepool)) {
        db_sharepool =
          reinterpret_cast<db_sharepool_t *>(get_sharepoolex(sharekey.int32()));
      }
      if (is_null(db_sharepool)) return;
      db_sharepool->delete_obj(db_sharepool->get_obj(poolindex.int32()));
    }
  __LEAVE_FUNCTION
}

void DBStore::flush() {
  __ENTER_FUNCTION
    if (shareservice_) {
      uint32_t unsave_datacount = 1;
      do {
        for (uint32_t i = 0; i < sharekeys_.size(); ++i)
          sharekeys_[i]->flush(true);
        unsave_datacount = GLOBAL_VALUES["app_unsave_datacount"].uint32();
      } while (unsave_datacount > 0);
    }
  __LEAVE_FUNCTION
}

const char *DBStore::getprefix() const {
  return NULL;
}

void DBStore::set_cachetime(const char *key, int32_t minutes) {
  __ENTER_FUNCTION
    using namespace pf_base;
    if (is_null(keyhash_)) return;
    const char *hash = keyhash_->get(key);
    if (is_null(hash)) return;
    pf_base::variable_t sharekey = 0;
    pf_base::variable_t poolindex = hash;
    std::vector<std::string> array;
    string::explode(key, array, "\t", true, true);
    if (array.size() != 2) return; //Key must be "key_name".
    sharekey = array[0];
    db_sharenode_t *db_sharenode = 
      reinterpret_cast<db_sharenode_t *>(get_sharenode(sharekey.int32()));
    if (is_null(db_sharenode)) return;
    //Set cahe time.
    db_sharenode->setminutes(poolindex.uint32(), minutes);
  __LEAVE_FUNCTION
}
   
void DBStore::tick() {
  __ENTER_FUNCTION
    GLOBAL_VALUES["unsave_datacount"] = 0;
    for (uint32_t i = 0; i < sharekeys_.size(); ++i) {
      sharekeys_[i]->tick();
      GLOBAL_VALUES["unsave_datacount"] += sharekeys_[i]->get_holddata_count();
    }
  __LEAVE_FUNCTION
}

bool DBStore::register_sharekey(int32_t key, int32_t size, int32_t datasize) {
  __ENTER_FUNCTION
    using namespace pf_sys::memory::share;
    bool result = false;
    if (shareservice_) {
      db_sharenode_t *_db_sharenode = 
        reinterpret_cast<db_sharenode_t *>(get_sharenode(key));
      if (!is_null(_db_sharenode)) return false;
      db_sharenode_t *db_sharenode = new db_sharenode_t;
      if (is_null(db_sharenode) || is_null(db_sharenode->getpool())) 
        return false;
      db_sharenode->getpool()->set_datasize(datasize);
      result = db_sharenode->init(size, key);
      sharekeys_.push_back(db_sharenode);
      sharepools_.push_back(db_sharenode->getpool());
    } else {
      db_sharepool_t *db_sharepool = new db_sharepool_t;
      if (is_null(db_sharepool)) return false;
      db_sharepool->set_datasize(datasize);
      result = db_sharepool->init(size, key, kSmptShareMemory + 1);
      sharepools_.push_back(db_sharepool);
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

void *DBStore::get_sharenode(int32_t key) {
  __ENTER_FUNCTION
    void *result = NULL;
    for (uint32_t i = 0; i < sharekeys_.size(); ++i) {
      if (sharekeys_[i]->getkey() == static_cast<uint32_t>(key)) {
        result = reinterpret_cast<void *>(sharekeys_[i]);
      }
    }
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

bool DBStore::register_sharekeyex(int32_t key, int32_t size, int32_t datasize) {
   __ENTER_FUNCTION
    using namespace pf_sys::memory::share;
    bool result = false;
    if (shareservice_) {
      db_sharenodeex_t *_db_sharenodeex = 
        reinterpret_cast<db_sharenodeex_t *>(get_sharenodeex(key));
      if (!is_null(_db_sharenodeex)) return false;
      db_sharenodeex_t *db_sharenodeex = new db_sharenodeex_t;
      if (is_null(db_sharenodeex) || is_null(db_sharenodeex->getpool()))
        return false;
      db_sharenodeex->getpool()->set_datasize(datasize);
      result = db_sharenodeex->init(size, key);
      sharekeysex_.push_back(db_sharenodeex);
      sharepoolsex_.push_back(db_sharenodeex->getpool());
    } else {
      db_sharepoolex_t *db_sharepoolex = new db_sharepoolex_t;
      if (is_null(db_sharepoolex)) return false;
      db_sharepoolex->set_datasize(datasize);
      result = db_sharepoolex->init(size, key, kSmptShareMemory + 1);
      sharepoolsex_.push_back(db_sharepoolex);
    }
    return result;
  __LEAVE_FUNCTION
    return false; 
}

void *DBStore::get_sharenodeex(int32_t key) {
  __ENTER_FUNCTION
    void *result = NULL;
    for (uint32_t i = 0; i < sharekeysex_.size(); ++i) {
      if (sharekeysex_[i]->getkey() == static_cast<uint32_t>(key)) {
        result = reinterpret_cast<void *>(sharekeysex_[i]);
      }
    }
    return result;
  __LEAVE_FUNCTION
    return NULL;
}
  
bool DBStore::set_sharekey_countex(int32_t count) {
  __ENTER_FUNCTION
    USE_PARAM(count);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool DBStore::set_sharekey_count(int32_t count) {
  __ENTER_FUNCTION
    USE_PARAM(count);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool DBStore::set_cachecount(int32_t count) {
  __ENTER_FUNCTION
    USE_PARAM(count);
    return true;
  __LEAVE_FUNCTION
    return false;
}

void DBStore::recycle(const char *key) {
  __ENTER_FUNCTION
    pf_base::variable_t _key = key;
    if (recyclekeys_.find(key) == recyclekeys_.end()) return;
    recyclekeys_[key] = 1;
  __LEAVE_FUNCTION
}

void *DBStore::get_sharepoolex(int32_t key) {
  __ENTER_FUNCTION
    void *result = NULL;
    for (uint8_t i = 0; i < sharepoolsex_.size(); ++i) {
      if (sharepoolsex_[i]->get_key() == static_cast<uint32_t>(key)) {
        result = sharepoolsex_[i];
        break;
      }
    }
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

void *DBStore::get_sharepool(int32_t key) {
  __ENTER_FUNCTION
    void *result = NULL;
    for (uint8_t i = 0; i < sharepools_.size(); ++i) {
      if (sharepools_[i]->get_key() == static_cast<uint32_t>(key)) {
        result = sharepools_[i];
        break;
      }
    }
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

void DBStore::set_shareservice(bool flag) {
  shareservice_ = flag;
}

void *DBStore::get_keyhash() {
  __ENTER_FUNCTION
    void *result = static_cast<void *>(keyhash_);
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

bool DBStore::register_keyhash(int32_t key, int32_t size, bool create) {
  __ENTER_FUNCTION
    using namespace pf_sys::memory;
    if (!is_null(keyhash_)) return false;
    keyhash_ = new share::Map;
    if (is_null(keyhash_)) return false;
    uint32_t keysize = CACHE_SHARE_HASHKEY_SIZE;
    uint32_t valuesize = CACHE_SHARE_HASHVALUE_SIZE;
    bool result = keyhash_->init(key, size, keysize, valuesize, create);
    return result;
  __LEAVE_FUNCTION
    return false;
}

}; //namespace pf_cache
