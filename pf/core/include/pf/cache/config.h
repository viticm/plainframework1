/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id config.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/10 14:36
 * @uses cache config class
 */
#ifndef PF_CACHE_CONFIG_H_
#define PF_CACHE_CONFIG_H_

#include "pf/base/config.h"
#include "pf/sys/memory/share.h"

#define CACHE_DBNODE_MAX 20
#define CACHE_SHARE_POOLSIZE 1000
#define CACHE_SHARE_KEYSIZE 30
#define CACHE_SAVEITEM_KEYSIZE (1024)
#define CACHE_SAVEITEM_TYPESIZE (64)
#define CACHE_SAVEITEM_CONDITIONSIZE (512)
#define CACHE_SHARE_HASHKEY_SIZE (128)
#define CACHE_SHARE_HASHVALUE_SIZE (64)

namespace pf_cache {

class StoreInterface;
class Repository;
class DBStore;
class Manager;

//Call back function for store.
typedef void *(__stdcall *function_callback)();

//DB save item struct.
typedef struct PF_API db_saveitem_struct db_saveitem_t;

//DB save item extend struct.
typedef struct PF_API db_saveitemex_struct db_saveitemex_t;

//DB share data struct.
typedef pf_sys::memory::share::data_template<db_saveitem_t> db_sharedata_t;

//DB extend data.
typedef pf_sys::memory::share::data_template<db_saveitemex_t> db_sharedataex_t;

//DB save share node.
typedef pf_sys::memory::share::Node<db_sharedata_t> db_sharenode_t;

//DB save share node ex.
typedef pf_sys::memory::share::Node<db_sharedataex_t> db_sharenodeex_t;

//DB share pool.
typedef pf_sys::memory::share::UnitPool<db_sharedata_t> db_sharepool_t;

//DB share pool extend.
typedef pf_sys::memory::share::UnitPool<db_sharedataex_t> db_sharepoolex_t;

};

#endif //PF_CACHE_CONFIG_H_
