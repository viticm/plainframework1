#include "pf/cache/storeinterface.h"
#include "pf/cache/repository.h"

namespace pf_cache {

Repository::Repository(StoreInterface *store) {
  store_ = store;
  minutes_ = 0;
}

Repository::~Repository() {
  SAFE_DELETE(store_);
}

   
bool Repository::has(const char *key) {
  return !is_null(get(key));
}
   
void *Repository::get(const char *key, void *_default) {
  __ENTER_FUNCTION
    void *result = _default;
    if (is_null(store_)) return result;
    result = store_->get(key);
    if (is_null(result)) result = _default;
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

void *Repository::pull(const char *key, void *_default) {
  __ENTER_FUNCTION
    if (is_null(store_)) return NULL; 
    void *result = get(key, _default);
    store_->forget(key);
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

void Repository::put(const char *key, void *value, int32_t minutes) {
  __ENTER_FUNCTION
    if (!is_null(store_)) {
      store_->put(key, value, minutes);
    }
  __LEAVE_FUNCTION
}

void Repository::add(const char *key, void *value, int32_t minutes) {
  __ENTER_FUNCTION
    if (is_null(get(key))) put(key, value, minutes);
  __LEAVE_FUNCTION
}

void Repository::forget(const char *key) {
  __ENTER_FUNCTION
    if (is_null(store_)) store_->forget(key);
  __LEAVE_FUNCTION
}

void *Repository::remember(const char *key, 
                           int32_t minutes, 
                           function_callback callback) {
  __ENTER_FUNCTION
    void *result = NULL;
    if (!is_null(result = get(key))) return result;
    if (!is_null(callback)) {
      put(key, result = (*callback)(), minutes);
    }
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

void *Repository::sear(const char *key, function_callback callback) {
  return remember_forever(key, callback);
}

void *Repository::remember_forever(const char *key, 
                                   function_callback callback) {
  __ENTER_FUNCTION
    if (is_null(store_)) return NULL; 
    void *result = NULL;
    if (!is_null(result = get(key))) return result;
    if (!is_null(callback)) { 
      store_->forever(key, result = (*callback)());
    }
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

int32_t Repository::get_default_cachetime() const {
  return minutes_;
}

void Repository::set_default_cachetime(int32_t minutes) {
  minutes_ = minutes;
}

StoreInterface *Repository::getstore() {
  return store_;
}

void Repository::set_cachetime(const char *key, int32_t minutes) {
  __ENTER_FUNCTION
    if (!is_null(store_)) store_->set_cachetime(key, minutes);
  __LEAVE_FUNCTION
}
   
void Repository::tick() {
  __ENTER_FUNCTION
    if (!is_null(store_)) store_->tick();
  __LEAVE_FUNCTION
}

bool Repository::register_sharekey(int32_t key, 
                                   int32_t size, 
                                   int32_t datasize) {
  __ENTER_FUNCTION
    bool result = false;
    if (!is_null(store_)) 
      result = store_->register_sharekey(key, size, datasize);
    return result;
  __LEAVE_FUNCTION
    return false;
}

void *Repository::get_sharenode(int32_t key) {
  __ENTER_FUNCTION
    void *result = NULL;
    if (!is_null(store_)) result = store_->get_sharenode(key);
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

bool Repository::set_sharekey_count(int32_t count) {
  __ENTER_FUNCTION
    bool result = false;
    if (!is_null(store_)) result = store_->set_sharekey_count(count);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Repository::set_cachecount(int32_t count) {
  __ENTER_FUNCTION
    bool result = false;
    if (!is_null(store_)) result = store_->set_cachecount(count);
    return result;
  __LEAVE_FUNCTION
    return false;
}

void Repository::recycle(const char *key) {
  __ENTER_FUNCTION
    if (!is_null(store_)) store_->recycle(key);
  __LEAVE_FUNCTION
}

bool Repository::register_sharekeyex(int32_t key, 
                                     int32_t size, 
                                     int32_t datasize) {
  __ENTER_FUNCTION
    bool result = false;
    if (!is_null(store_)) 
      result = store_->register_sharekeyex(key, size, datasize);
    return result;
  __LEAVE_FUNCTION
    return false;
}

void *Repository::get_sharenodeex(int32_t key) {
  __ENTER_FUNCTION
    void *result = NULL;
    if (!is_null(store_)) result = store_->get_sharenodeex(key);
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

bool Repository::set_sharekey_countex(int32_t count) {
  __ENTER_FUNCTION
    bool result = false;
    if (!is_null(store_)) result = store_->set_sharekey_countex(count);
    return result;
  __LEAVE_FUNCTION
    return false;
}

void Repository::set_shareservice(bool flag) {
  if (!is_null(store_)) store_->set_shareservice(flag);
}

void *Repository::get_keyhash() {
  __ENTER_FUNCTION
    void *result = NULL;
    if (!is_null(store_)) result = store_->get_keyhash();
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

void *Repository::get_sharepool(int32_t key) {
  __ENTER_FUNCTION
    void *result = NULL;
    if (!is_null(store_)) result = store_->get_sharepool(key);
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

void *Repository::get_sharepoolex(int32_t key) {
  __ENTER_FUNCTION
    void *result = NULL;
    if (!is_null(store_)) result = store_->get_sharepoolex(key);
    return result;
  __LEAVE_FUNCTION
    return NULL;
}
   
bool Repository::register_keyhash(int32_t key, int32_t size, bool create) {
  __ENTER_FUNCTION
    bool result = false;
    if (!is_null(store_)) result = store_->register_keyhash(key, size, create);
    return result;
  __LEAVE_FUNCTION
    return false;
}

} //namespace pf_cache
