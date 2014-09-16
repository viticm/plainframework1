/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id define.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/30 16:57
 * @uses the defines for db module
 */
#ifndef COMMON_DB_DEFINE_H_
#define COMMON_DB_DEFINE_H_

#include "common/db/config.h"

#pragma pack(push, 1)

namespace common {

namespace db {

typedef enum {
  kTypeUser = 0,
  kTypeCharacter,
} type_t;

/* structs { */
typedef struct globaldata_struct {
  int32_t poolid;
  int32_t data;
  void clear() {
    poolid = 0;
    data = 0;
  }
} globaldata_t;
/* } structs */

/* tables { */
extern const char *kTableNameGloablData;
extern const char *kTableNameRole;
/* } tables */

/* sqls { */
extern const char *kSqlSelectGlobalData;
extern const char *kSqlSaveGlobalData;
extern const char *kSqlGetRoleNumber;
/* } sqls */

}; //namespace db

}; //namespace common

#pragma pack(pop)

#endif //COMMON_DB_DEFINE_H_
