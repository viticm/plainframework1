/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/10 20:41
 * @uses the db module config file
 */
#ifndef DB_CONFIG_H_
#define DB_CONFIG_H_

#include "pf/base/config.h"
#include "common/define/macros.h"
#include "application/config.h"

#define DB_TABLE_USER "users"

namespace db {

struct _user {
  uint32_t id;
  char name[ACCOUNT_LENGTH_MAX];
  char password[PASSWORD_LENGTH_MAX];
  char truename[ROLE_NAME_LENGTH_MAX];
  char birthday[DATE_LENGTH_MAX];
  char superpassword[PASSWORD_LENGTH_MAX];
  uint32_t money;
  uint32_t today_logintimes;
  bool locked;
  uint8_t viplevel;
  _user() {
    id = 0;
    memset(name, 0, sizeof(name));
    memset(password, 0, sizeof(password));
    memset(truename, 0, sizeof(truename));
    memset(birthday, 0, sizeof(birthday));
    memset(superpassword, 0, sizeof(superpassword));
    money = 0;
    today_logintimes = 0;
    locked = false;
    viplevel = 0;
  };
};

typedef _user user_t;

}; //namespace db

#endif
