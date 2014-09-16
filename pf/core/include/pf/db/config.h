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
#ifndef PF_DB_DEFINE_H_
#define PF_DB_DEFINE_H_

#include "pf/base/config.h"
#define SQL_LENGTH_MAX (1024*400)
//#define LONG_SQL_LENGTH_MAX 204800
#define DB_CONNECTION_NAME_LENGTH 128 
#define DB_DBNAME_LENGTH 128
#define DB_USER_NAME_LENGTH 32
#define DB_PASSWORD_LENGTH 32

struct db_query_t {
  char sql_str_[SQL_LENGTH_MAX];
  void clear() {
    memset(sql_str_, '\0', sizeof(sql_str_));
  }
  void parse(const char *temp, ...) {
    va_list argptr;
    va_start(argptr, temp);
    int nchars  = vsnprintf(sql_str_, 
                            sizeof(sql_str_) - 1, 
                            temp, 
                            argptr);
    int32_t sqlstr_length = static_cast<int32_t>(sizeof(sql_str_));
    va_end(argptr);
    if (-1 == nchars || sqlstr_length - 1 < nchars) {
      Assert(false);
    }
  }
};
/**
struct long_db_query_t {
  char sql_str_[LONG_SQL_LENGTH_MAX];
  void clear() {
    memset(sql_str_, '\0', LONG_SQL_LENGTH_MAX);
  }
  void parse(const char *temp, ...) {
    va_list argptr;
    va_start(argptr, temp);
    int nchars  = vsnprintf(sql_str_, 
                            sizeof(sql_str_) - 1, 
                            temp,
                            argptr);
    va_end(argptr);
    int32_t sqlstr_length = static_cast<int32_t>(sizeof(sql_str_));
    if (-1 == nchars || sqlstr_length - 1 < nchars) {
      Assert(false);
    }
  }
};
**/

//namespace pf_db {

typedef enum {
  kDBConnectorTypeODBC = 0, //方便以后扩展
} dbconnector_type_t;

//} //namespace pf_db

#endif //PF_DB_DEFINE_H_
