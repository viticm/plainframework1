/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/23 10:29
 * @uses the defines for db module
 */
#ifndef PF_DB_CONFIG_H_
#define PF_DB_CONFIG_H_

#include "pf/base/config.h"
#include "pf/base/hashmap/template.h"

#define SQL_LENGTH_MAX (1024*400)
//#define LONG_SQL_LENGTH_MAX 204800
#define DB_CONNECTION_NAME_LENGTH 128 
#define DB_DBNAME_LENGTH 128
#define DB_USER_NAME_LENGTH 32
#define DB_PASSWORD_LENGTH 32
#define DB_TABLENAME_LENGTH 64
#define DB_PREFIX_LENGTH 32
#define DB_COLUMN_COUNT_MAX 64

struct PF_API db_query_t {
  char sql_str_[SQL_LENGTH_MAX];
  void clear();
  void parse(const char *temp, ...);
  void concat(const char *temp, ...);
};

enum {
  kDBColumnTypeString = 0,
  kDBColumnTypeNumber,
}; //字段类型

/* db fetch and cache { */
typedef std::map<std::string, int8_t> db_keys_t;
/* } db fetch and cache */

typedef struct PF_API db_fetch_array_struct db_fetch_array_t;
struct db_fetch_array_struct {

  //Database cloumn names.
  pf_base::variable_array_t keys;

  //Result values.
  pf_base::variable_array_t values;

  db_fetch_array_struct();
  db_fetch_array_struct(const db_fetch_array_t &object);
  db_fetch_array_struct(const db_fetch_array_t *object);
  db_fetch_array_t &operator = (const db_fetch_array_t &object);
  db_fetch_array_t *operator = (const db_fetch_array_t *object);
  pf_base::variable_t *get(int32_t row, const char *key);
  pf_base::variable_t *get(int32_t row, int32_t column);
  uint32_t size();
  void clear();
};

typedef enum {
  kDBConnectorTypeODBC = 0, //方便以后扩展
} dbconnector_type_t;

namespace pf_db {

class Query;
class Manager;

namespace odbc {

class Interface;
class System;

}; //namespace odbc

}; //namespace pf_db

#endif //PF_DB_CONFIG_H_
