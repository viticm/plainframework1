/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/19 11:01
 * @uses the common db odbc base config
 */
#ifndef PF_DB_ODBC_CONFIG_H_
#define PF_DB_ODBC_CONFIG_H_

#include "pf/db/config.h"

#define HOST_LENGTH 30
#define CONNECTION_NAME_LENGTH 32
#define COLUMN_NAME_MAX 30 //column name max length
#define COLUMN_INFO_BUFFER_MAX (1024*8) //normal one column value length
#define COLUMN_VALUE_BUFFER_MAX (1024*400)
#define ERROR_MESSAGE_LENGTH_MAX 255
#define DB_ODBC_COLUMN_NAME_LENGTH_MAX 256

#endif //PF_DB_ODBC_CONFIG_H_
