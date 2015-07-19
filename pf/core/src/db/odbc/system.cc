#include "pf/base/util.h"
#include "pf/base/log.h"
#include "pf/db/odbc/interface.h"
#include "pf/db/odbc/system.h"

namespace pf_db {

namespace odbc {

System::System() {
  __ENTER_FUNCTION
    result_ = 0;
    result_count_ = 0;
    op_type_ = kDBOptionTypeInitEmpty;
    odbc_interface_ = NULL;
  __LEAVE_FUNCTION
}

System::~System() {
  __ENTER_FUNCTION
    SAFE_DELETE(odbc_interface_);
  __LEAVE_FUNCTION
}

bool System::init(const char *connectionname,
                  const char *username,
                  const char *password) {
  __ENTER_FUNCTION
    bool connected = true;
    odbc_interface_ = new Interface();
    Assert(odbc_interface_);
    connected = odbc_interface_->connect(connectionname, username, password);
    if (!connected) {
      SLOW_ERRORLOG(DB_MODULENAME,
                    "[db.odbc] (System::init) failed."
                    " connectionname: %s, username: %s, password: %s,"
                    " errormessage: %s",
                    connectionname,
                    username,
                    password,
                    get_error_message());
    }
    return connected;
  __LEAVE_FUNCTION
    return false;
}

uint32_t System::get_result_count() {
  __ENTER_FUNCTION
    return result_count_;
  __LEAVE_FUNCTION
    return 0;
}

bool System::fetch(int32_t orientation, int32_t offset) {
  __ENTER_FUNCTION
    bool result = odbc_interface_->fetch(orientation, offset);
    return result;
  __LEAVE_FUNCTION
    return false;
}

int32_t System::get_internal_affect_count() {
  __ENTER_FUNCTION
    return odbc_interface_->get_affect_row_count();
  __LEAVE_FUNCTION
    return -1;
}

bool System::is_prepare() {
  __ENTER_FUNCTION
    return odbc_interface_->is_prepare();
  __LEAVE_FUNCTION
    return false;
}

bool System::check_db_connect() {
  __ENTER_FUNCTION
    Assert(odbc_interface_);
    bool result = false;
#ifndef PF_CORE_WITH_NOODBC

    if (!odbc_interface_->is_connected()) {
      uint32_t tickcount = TIME_MANAGER_POINTER->get_tickcount();
      if (timer_.counting(tickcount) && odbc_interface_->connect()) {
        SLOW_DEBUGLOG("odbc",
          "[db.odbc] the connection reconnect successful!"
          " connection name: %s.",
          odbc_interface_->connection_name_);
        timer_.cleanup();
        result = true;
      } else {
        if (!timer_.isstart()) {
          SLOW_WARNINGLOG(DB_MODULENAME,
            "[db.odbc] the connection lost, try connect after 5 seconds!"
            " connection name: %s.",
            odbc_interface_->connection_name_);
          timer_.start(5000, tickcount);
        }
      }
    }

#endif
    return result;
  __LEAVE_FUNCTION
    return false;
}

db_query_t *System::get_internal_query() {
  __ENTER_FUNCTION
    db_query_t *db_query = odbc_interface_->get_query();
    return db_query;
  __LEAVE_FUNCTION
    return NULL;
}

bool System::load() {
  __ENTER_FUNCTION
    if (!is_prepare()) return false;
    if (!odbc_interface_) return false;
    op_type_ = kDBOptionTypeLoad;
    odbc_interface_->clear();
    result_ = odbc_interface_->execute();
    result_count_ = odbc_interface_->get_affect_row_count();
    return result_;
  __LEAVE_FUNCTION
    return false;
}

bool System::getresult() const {
  return result_;
}

bool System::query() {
  __ENTER_FUNCTION
    if (!is_prepare()) return false;
    if (!odbc_interface_) return false;
    odbc_interface_->clear();
    op_type_ = kDBOptionTypeQuery;
    result_ = odbc_interface_->execute();
    result_count_ = odbc_interface_->get_affect_row_count();
    return result_;
  __LEAVE_FUNCTION
    return false;
}

bool System::add_new() {
  __ENTER_FUNCTION
    if (!is_prepare()) return false;
    if (!odbc_interface_) return false;
    op_type_ = kDBOptionTypeAddNew;
    odbc_interface_->clear();
    result_ = odbc_interface_->execute();
    result_count_ = odbc_interface_->get_affect_row_count();
    return result_;
  __LEAVE_FUNCTION
    return false;
}

bool System::_delete() {
  __ENTER_FUNCTION
    if (!is_prepare()) return false;
    if (!odbc_interface_) return false;
    op_type_ = kDBOptionTypeDelete;
    odbc_interface_->clear();
    result_ = odbc_interface_->execute();
    result_count_ = odbc_interface_->get_affect_row_count();
    return result_;
  __LEAVE_FUNCTION
    return false;
}

bool System::save() {
  __ENTER_FUNCTION
    if (!is_prepare()) return false;
    if (!odbc_interface_) return false;
    op_type_ = kDBOptionTypeSave;
    odbc_interface_->clear();
    result_ = odbc_interface_->execute();
    result_count_ = odbc_interface_->get_affect_row_count();
    return result_;
  __LEAVE_FUNCTION
    return false;
}

int32_t System::get_error_code() {
  __ENTER_FUNCTION
    return odbc_interface_->get_error_code();
  __LEAVE_FUNCTION
    return -1;
}

const char *System::get_error_message() {
  __ENTER_FUNCTION
    return odbc_interface_->get_error_message();
  __LEAVE_FUNCTION
    return NULL;
}

Interface *System::getinterface() {
  return odbc_interface_;
}

int32_t System::get_columncount() const {
  __ENTER_FUNCTION
    int32_t result = 0;
    result = odbc_interface_->get_columncount();
    return result;
  __LEAVE_FUNCTION
    return 0;
}

int8_t System::gettype(int32_t column_index) {
  __ENTER_FUNCTION
    int8_t type = kDBColumnTypeString;
#ifndef PF_CORE_WITH_NOODBC

    int16_t typecode = odbc_interface_->get_type(column_index); 
    switch (typecode) {
      case SQL_CHAR:
        break;
      case SQL_VARCHAR:
        break;
      case SQL_LONGVARCHAR:
        break;
      case SQL_WCHAR:
        break;
      case SQL_WVARCHAR:
        break;
      case SQL_WLONGVARCHAR:
        break;
      case SQL_DECIMAL:
        break;
      case SQL_NUMERIC:
        type = kDBColumnTypeNumber;
        break;
      case SQL_SMALLINT:
        type = kDBColumnTypeNumber;
        break;
      case SQL_INTEGER:
        type = kDBColumnTypeNumber;
        break;
      case SQL_REAL:
        type = kDBColumnTypeNumber;
        break;
      case SQL_FLOAT:
        type = kDBColumnTypeNumber;
        break;
      case SQL_DOUBLE:
        type = kDBColumnTypeNumber;
        break;
      case SQL_BIT:
        type = kDBColumnTypeNumber;
        break;
      case SQL_TINYINT:
        type = kDBColumnTypeNumber;
        break;
      case SQL_BIGINT:
        type = kDBColumnTypeNumber;
        break;
      case SQL_TYPE_DATE:
        break;
      case SQL_TYPE_TIME:
        break;
      case SQL_TYPE_TIMESTAMP:
        break;
      case SQL_INTERVAL_MONTH:
        break;
      case SQL_INTERVAL_YEAR:
        break;
      case SQL_INTERVAL_YEAR_TO_MONTH:
        break;
      case SQL_INTERVAL_DAY:
        break;
      case SQL_INTERVAL_HOUR:
        break;
      case SQL_INTERVAL_MINUTE:
        break;
      case SQL_INTERVAL_SECOND:
        break;
      case SQL_INTERVAL_DAY_TO_HOUR:
        break;
      case SQL_INTERVAL_DAY_TO_MINUTE:
        break;
      case SQL_INTERVAL_DAY_TO_SECOND:
        break;
      case SQL_INTERVAL_HOUR_TO_MINUTE:
        break;
      case SQL_INTERVAL_HOUR_TO_SECOND:
        break;
      case SQL_INTERVAL_MINUTE_TO_SECOND:
        break;
      case SQL_GUID:
        break;
      case SQL_BINARY:
        break;
      case SQL_VARBINARY:
        break;
      case SQL_LONGVARBINARY:
        break;
      default:
        break;
    }

#else

  USE_PARAM(column_index);

#endif
    return type;
  __LEAVE_FUNCTION
    return kDBColumnTypeString;
}

} //namespace odbc

} //namespace pf_db
