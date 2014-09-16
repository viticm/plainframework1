#include "engine/system.h"
#include "db/user.h"

namespace db {

namespace user {

void get_fullinfo(const char *name, user_t& info) {
  __ENTER_FUNCTION
    if (!ENGINE_SYSTEM_POINTER || !ENGINE_SYSTEM_POINTER->get_dbmanager())
      return;
    enum {
      kDBId = 1,
      kDBName,
      kDBPassword,
      kDBTrueName,
      kDBBirthday,
      kDBSuperPassword,
      kDBMoney,
      kDBTodayLoginTimes,
      kDBLocked,
      kDBVipLevel,
    };
    const char *kSqlStr = "SELECT `id`, `name`, `password`, `truename`,"
                          " `birthday`, `password2`, `money`,"
                          " `today_logintimes`, `locked`, `viplevel`"
                          " FROM `"DB_TABLE_USER"`"
                          " WHERE `name` = '%s'";
    ENGINE_SYSTEM_POINTER
      ->get_dbmanager()
      ->get_internal_query()
      ->parse(kSqlStr, name);
    bool query_result = ENGINE_SYSTEM_POINTER->get_dbmanager()->query();
    if (query_result && ENGINE_SYSTEM_POINTER->get_dbmanager()->fetch()) {
      int32_t error_code = 0;
      info.id = 
        ENGINE_SYSTEM_POINTER->get_dbmanager()->get_int32(kDBId, error_code);
      ENGINE_SYSTEM_POINTER
        ->get_dbmanager()
        ->get_string(kDBName, info.name, sizeof(info.name) - 1, error_code);
      ENGINE_SYSTEM_POINTER
        ->get_dbmanager()
        ->get_string(kDBPassword, 
                     info.password, 
                     sizeof(info.password) - 1, 
                     error_code);
      ENGINE_SYSTEM_POINTER
        ->get_dbmanager()
        ->get_string(kDBTrueName, 
                     info.truename, 
                     sizeof(info.truename) - 1, 
                     error_code);
      ENGINE_SYSTEM_POINTER
        ->get_dbmanager()
        ->get_string(kDBBirthday,
                     info.birthday,
                     sizeof(info.birthday) - 1,
                     error_code);
      ENGINE_SYSTEM_POINTER
        ->get_dbmanager()
        ->get_string(kDBSuperPassword,
                     info.superpassword,
                     sizeof(info.superpassword) - 1,
                     error_code);
      info.money = ENGINE_SYSTEM_POINTER
                   ->get_dbmanager()
                   ->get_uint32(kDBMoney, error_code);

      info.today_logintimes = ENGINE_SYSTEM_POINTER
                              ->get_dbmanager()
                              ->get_uint32(kDBTodayLoginTimes, error_code);
      uint8_t locked = ENGINE_SYSTEM_POINTER
                       ->get_dbmanager()
                       ->get_uint8(kDBLocked, error_code);
      info.locked = 0 == locked ? false : true;
      info.viplevel = ENGINE_SYSTEM_POINTER
                      ->get_dbmanager()
                      ->get_uint8(kDBVipLevel, error_code);
    }
  __LEAVE_FUNCTION
}

} //namespace user

} //namespace db
