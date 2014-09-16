#include "common/db/define.h"

namespace common {

namespace db {

/* tables {*/
const char *kTableNameGloablData = "t_global";
const char *kTableNameRole = "t_role";
/* } tables */


/* sqls { */
const char *kSqlSelectGlobalData = "SELECT poolid, data FROM %s";
const char *kSqlSaveGlobalData = "UPDATE %s SET data = %d WHERE poolid = %d";
const char *kSqlGetRoleNumber = "SELECT COUNT(`id`) AS `count` FROM %s"
                                " WHERE `account` = '%s'";
/* } sqls */


} //namespace db

} //namespace common
