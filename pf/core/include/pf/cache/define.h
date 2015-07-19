/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id define.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/10 16:33
 * @uses cache define file
 */
#ifndef PF_CACHE_DEFINE_H_
#define PF_CACHE_DEFINE_H_

#include "pf/cache/config.h"
#include "pf/sys/memory/share.h"
#include "pf/db/define.h"

namespace pf_cache {

struct db_saveitem_struct {

  //Database cache values.
  //db_fetch_array_t cache;

  //Database column names.
  char keys[CACHE_SAVEITEM_KEYSIZE];

  //Database column types.
  char types[CACHE_SAVEITEM_TYPESIZE];

  //The name of the cache table.
  char tablename[DB_TABLENAME_LENGTH];

  //A string that should be prepended to keys.
  char prefix[DB_PREFIX_LENGTH];

  //Save to db condition.
  //pf_base::variable_array_t condition;
  char condition[CACHE_SAVEITEM_CONDITIONSIZE];

  //Extra params.
  int32_t paramex[4];

  //Construct function.
  db_saveitem_struct();

  //Clear all data.
  void clear();

  //Like database fetch array.
  bool datato_fetch_array(db_fetch_array_t &array, 
                          char *data, 
                          int32_t datalength);

  //Save data from fetch array.
  bool fetch_array_todata(db_fetch_array_t &array, 
                          char *data, 
                          int32_t datalength);

  //Is valid fetch array.
  bool isvalid_fetch_array(db_fetch_array_t &array, int32_t datalength);
  
  //Vector to condtion.
  bool savecondition(pf_base::variable_array_t &array);

  //Condition to vector.
  bool getcondition(pf_base::variable_array_t &array);

  //Is valid condition;
  bool isvalid_condition(pf_base::variable_array_t &condition);

  db_saveitem_t &operator = (const db_saveitem_t &object);
  db_saveitem_t *operator = (const db_saveitem_t *object);

}; //数据库缓存结构，多条数据，但个别有需要分开的请自行分开

struct db_saveitemex_struct {

  //Data.
  char data[128];

  //Construct function.
  db_saveitemex_struct();

  //Clear all data.
  void clear();

  db_saveitemex_t &operator = (const db_saveitemex_t &object);
  db_saveitemex_t *operator = (const db_saveitemex_t *object);

}; //现在这个扩展主要是为了来查询数据库，暂时比较鸡肋

}; //namespace pf_cache

#endif //PF_CACHE_DEFINE_H_
