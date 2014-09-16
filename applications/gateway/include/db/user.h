/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id user.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/10 20:38
 * @uses the db user table operator module
 */
#ifndef DB_USER_H_
#define DB_USER_H_

#include "db/config.h"

namespace db {

namespace user {

void get_fullinfo(const char *name, user_t& info);

}; //namespace user

}; //namespace db

#endif //DB_USER_H_
