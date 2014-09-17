#include "pf/base/log.h"
#include "pf/base/util.h"
#include "pf/base/string.h"
#include "pf/base/time_manager.h"
#include "pf/db/odbc/interface.h"

namespace pf_db {

namespace odbc {

Interface::Interface() {
  __ENTER_FUNCTION
    connected_ = false;
    affect_count_ = -1;
    result_ = SQL_SUCCESS;
    sql_henv_ = NULL;
    sql_hdbc_ = NULL;
    sql_hstmt_ = NULL;
    memset(error_message_, '\0', sizeof(error_message_));
    memset(connection_name_, '\0', sizeof(connection_name_));
    memset(user_, '\0', sizeof(user_));
    memset(password_, '\0', sizeof(password_));
    query_.clear();
    column_count_ = 0;
    column_names_ = NULL;
    column_type_ = NULL;
    column_typenames_ = NULL;
    column_size_ = NULL;
    column_precision_ = NULL;
    column_nullable_ = NULL;
    column_values_ = NULL;
    column_valuelengths_ = NULL;
    column_info_allocator_.init(column_info_buffer, sizeof(column_info_buffer));
    column_value_allocator_.init(column_value_buffer, 
                                 sizeof(column_value_buffer));
  __LEAVE_FUNCTION
}

Interface::~Interface() {
  __ENTER_FUNCTION
    if (sql_hstmt_) SQLFreeHandle(SQL_HANDLE_STMT, sql_hstmt_);
    if (sql_hdbc_) SQLDisconnect(sql_hdbc_);
    if (sql_hdbc_) SQLFreeHandle(SQL_HANDLE_DBC, sql_hdbc_);
    if (sql_henv_) SQLFreeHandle(SQL_HANDLE_ENV, sql_henv_);
  __LEAVE_FUNCTION
}

bool Interface::connect(const char *connection_name,
                        const char *user,
                        const char *password) {
  __ENTER_FUNCTION
    using namespace pf_base;
    close(); //first disconnect
    if (connection_name != NULL)
      string::safecopy(connection_name_, 
                       connection_name, 
                       sizeof(connection_name_));
    if (user != NULL) string::safecopy(user_, user, sizeof(user_));
    if (password != NULL)
      string::safecopy(password_, password, sizeof(password_));
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sql_henv_);
    SQLSetEnvAttr(sql_henv_, 
                  SQL_ATTR_ODBC_VERSION, 
                  reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 
                  SQL_IS_INTEGER);
    SQLAllocHandle(SQL_HANDLE_DBC, sql_henv_, &sql_hdbc_);
    result_ = SQLConnect(sql_hdbc_,
                         reinterpret_cast<SQLCHAR*>(connection_name_),
                         SQL_NTS,
                         reinterpret_cast<SQLCHAR*>(user_),
                         SQL_NTS,
                         reinterpret_cast<SQLCHAR*>(password_),
                         SQL_NTS);
    if (SQL_SUCCESS != result_ && SQL_SUCCESS_WITH_INFO != result_) {
      char log_buffer[512];
      memset(log_buffer, '\0', sizeof(log_buffer));
      snprintf(log_buffer, 
               sizeof(log_buffer) - 1,
               "connection name: %s, connect username: %s, password: %s", 
               connection_name_,
               user_,
               password);
      SLOW_ERRORLOG(DB_MODULENAME, 
                    "[db.odbc] (Interface::connect) failed. %s", 
                    log_buffer);
      diag_state();
      return false;
    }
    result_ = SQLAllocHandle(SQL_HANDLE_STMT, sql_hdbc_, &sql_hstmt_);
    if (result_ != SQL_SUCCESS && result_ != SQL_SUCCESS_WITH_INFO) {
      sql_hstmt_ = NULL;
      return false;
    }
    connected_ = true;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Interface::connect() {
  __ENTER_FUNCTION
    close(); //first disconnect
#ifdef MUST_CLOSE_HENV_HANDLE
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sql_henv_);
    SQLSetEnvAttr(sql_henv_, 
                  SQL_ATTR_ODBC_VERSION, 
                  static_cast<SQLPOINTER>(SQL_OV_ODBC3), 
                  SQL_IS_INTEGER);
#endif
    SQLAllocHandle(SQL_HANDLE_DBC, sql_henv_, &sql_hdbc_);
    result_ = SQLConnect(sql_hdbc_,
                         reinterpret_cast<SQLCHAR*>(connection_name_),
                         SQL_NTS,
                         reinterpret_cast<SQLCHAR*>(user_),
                         SQL_NTS,
                         reinterpret_cast<SQLCHAR*>(password_),
                         SQL_NTS);
    if (result_ != SQL_SUCCESS && result_ != SQL_SUCCESS_WITH_INFO) {
      char log_buffer[512];
      memset(log_buffer, '\0', sizeof(log_buffer));
      snprintf(log_buffer, 
               sizeof(log_buffer) - 1,
               "connection name: %s connect user: %s", 
               connection_name_,
               user_); 
      SLOW_ERRORLOG(DB_MODULENAME, 
                    "[db.odbc] (Interface::connect) failed, %s", 
                    log_buffer);
      diag_state();
      return false;
    }
    result_ = SQLAllocHandle(SQL_HANDLE_STMT, sql_hdbc_, &sql_hstmt_);
    if (result_ != SQL_SUCCESS && result_ != SQL_SUCCESS_WITH_INFO) {
      sql_hstmt_ = NULL;
      return false;
    }
    connected_ = true;
    return true;
  __LEAVE_FUNCTION
    return false;
}

int32_t Interface::get_columncount() const {
  __ENTER_FUNCTION
    int32_t result = static_cast<int32_t>(column_count_);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

bool Interface::close() {
  __ENTER_FUNCTION
    if (sql_hstmt_) {
      try {
        SQLCloseCursor(sql_hstmt_);
        SQLFreeStmt(sql_hstmt_, SQL_UNBIND);
        SQLFreeHandle(SQL_HANDLE_STMT, sql_hstmt_);
        sql_hstmt_ = NULL;
      }
      catch(...) {
        sql_hstmt_ = NULL;
      }
    }

    if (sql_hdbc_) {
      try {
        SQLDisconnect(sql_hdbc_);
        SQLFreeHandle(SQL_HANDLE_DBC, sql_hdbc_);
        sql_hdbc_ = NULL;
      }
      catch(...) {
        sql_hdbc_ = NULL;
      }
    }

#ifdef MUST_CLOSE_HENV_HANDLE
    if (sql_henv_) {
      try {
        SQLFreeHandle(SQL_HANDLE_ENV, sql_henv_);
        sql_henv_ = NULL;
      }
      catch(...) {
        sql_henv_ = NULL;
      }
    }
#endif
    connected_ = false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Interface::failed(int32_t code) {
  __ENTER_FUNCTION
    if (SQL_SUCCESS == code || SQL_SUCCESS_WITH_INFO == code) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Interface::collect_resultinfo() {
  __ENTER_FUNCTION
    using namespace pf_base;
    if (!connected_ || !sql_hstmt_) return false;
    if (failed(SQLRowCount(sql_hstmt_, &affect_count_))) return false;
    if (failed(SQLNumResultCols(sql_hstmt_, &column_count_))) return false;
    clear_column_info();
    column_names_ = reinterpret_cast<char **>(
        column_info_allocator_.calloc(sizeof(char *), column_count_ + 1));
    column_values_ = reinterpret_cast<char **>(
        column_info_allocator_.calloc(sizeof(char *), column_count_ + 1)); 
    column_valuelengths_ = reinterpret_cast<SQLINTEGER *>(
        column_info_allocator_.calloc(sizeof(SQLINTEGER), column_count_ + 1));
    column_type_ = reinterpret_cast<SQLSMALLINT *>(
        column_info_allocator_.calloc(sizeof(SQLSMALLINT), column_count_ + 1));
    column_typenames_ = reinterpret_cast<char **>(
        column_info_allocator_.calloc(sizeof(char *), column_count_ + 1));
    column_size_ = reinterpret_cast<SQLULEN *>(
        column_info_allocator_.calloc(sizeof(SQLULEN), column_count_ + 1));
    column_precision_ = reinterpret_cast<SQLSMALLINT *>(
        column_info_allocator_.calloc(sizeof(SQLSMALLINT), column_count_ + 1));
    column_nullable_ = reinterpret_cast<SQLSMALLINT *>(
        column_info_allocator_.calloc(sizeof(SQLSMALLINT), column_count_ + 1));
    for (SQLUSMALLINT column = 0; column < column_count_; ++column) {
      SQLCHAR name[DB_ODBC_COLUMN_NAME_LENGTH_MAX] = {0};
      SQLSMALLINT namelength = 0;
      if (failed(SQLDescribeCol(sql_hstmt_, 
                                column + 1, 
                                name, 
                                sizeof(name), 
                                &namelength, 
                                column_type_ + column, 
                                column_size_ + column, 
                                column_precision_ + column, 
                                column_nullable_ + column))) {
        return false;
      }
      name[namelength] = '\0';
      column_names_[column] = reinterpret_cast<char *>(
          column_info_allocator_.malloc(namelength + 1));
      string::safecopy(column_names_[column], 
                       reinterpret_cast<const char *>(name), 
                       namelength + 1);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Interface::execute() {
  if (strlen(query_.sql_str_) <= 0) return false;
  try {
    //int column_index;
    result_ = SQLExecDirect(sql_hstmt_, 
                            reinterpret_cast<SQLCHAR*>(query_.sql_str_), 
                            SQL_NTS);
    if ((result_ != SQL_SUCCESS) && 
        (result_ != SQL_SUCCESS_WITH_INFO) &&
        (result_ != SQL_NO_DATA)) {
      diag_state();
      return false;
    }
    result_ = static_cast<SQLRETURN>(collect_resultinfo());
    return 0 == result_ ? false : true;
  } catch(...) {
    char temp[8092] = {0};
    snprintf(temp, sizeof(temp) - 1, "Huge Error occur: %s", query_.sql_str_);
    save_error_log(temp);
    return false;
  }
}

bool Interface::execute(const char *sql_str) {
  __ENTER_FUNCTION
    memset(query_.sql_str_, '\0', sizeof(query_.sql_str_));
    strncpy(query_.sql_str_, sql_str, sizeof(query_.sql_str_) - 1);
    return execute();
  __LEAVE_FUNCTION
    return false;
}

void Interface::clear_no_commit() {
  __ENTER_FUNCTION
    SQLCloseCursor(sql_hstmt_);
    SQLFreeStmt(sql_hstmt_, SQL_UNBIND);
  __LEAVE_FUNCTION
}

void Interface::clear_column_info() {
  __ENTER_FUNCTION
    column_info_allocator_.clear();
  __LEAVE_FUNCTION
}

void Interface::clear_column_data() {
  __ENTER_FUNCTION
    if (column_count_ > 0) {
      for (int32_t column = 0; column < column_count_; ++column) {
        if (column_values_[column] != NULL) {
          column_value_allocator_.free(column_values_[column]);
          column_values_[column] = NULL;
        }
      }
    }
    column_value_allocator_.clear();
  __LEAVE_FUNCTION
}

void Interface::clear() {
  __ENTER_FUNCTION
    SQLCloseCursor(sql_hstmt_);
    SQLFreeStmt(sql_hstmt_, SQL_UNBIND);
    clear_column_info();
    clear_column_data();
  __LEAVE_FUNCTION
}

bool Interface::fetch(int32_t orientation, int32_t offset) {
  __ENTER_FUNCTION
    clear_column_data();
    SQLRETURN resultcode;
    if (NULL == sql_hstmt_ || column_count_ < 1) return false;
    if (SQL_FETCH_NEXT == orientation && 0 == offset) {
      resultcode = SQLFetch(sql_hstmt_);
      if (failed(resultcode)) {
        if (resultcode != SQL_NO_DATA) diag_state();
        return false;
      }
    } else {
      resultcode = SQLFetchScroll(sql_hstmt_, 
                                  static_cast<SQLSMALLINT>(orientation), 
                                  offset);
      if (failed(resultcode)) {
        if (resultcode != SQL_NO_DATA) diag_state();
        return false;
      }
    }
    SQLSMALLINT column;
    for (column = 0; column < column_count_; ++column) {
      char work_data[512] = {0};
      SQLLEN data_length;
      SQLSMALLINT fetchtype = get_typemapping(column_type_[column]);
      if (fetchtype != SQL_C_BINARY) fetchtype = SQL_C_CHAR;
      resultcode = SQLGetData(sql_hstmt_, 
                              column + 1, 
                              fetchtype, 
                              work_data, 
                              sizeof(work_data) - 1, 
                              &data_length);
      if (failed(resultcode)) {
        if (resultcode != SQL_NO_DATA) diag_state();
        return false;
      }
      if (SQL_NULL_DATA == data_length) {
        column_values_[column] = NULL;
        column_valuelengths_[column] = 0;
      } else if (SQL_SUCCESS_WITH_INFO == resultcode) {
        if (data_length > static_cast<SQLLEN>(sizeof(work_data) - 1)) {
          data_length = static_cast<SQLLEN>(sizeof(work_data) - 1);
          if (SQL_C_CHAR == fetchtype) {
            while((data_length > 1) && (0 == work_data[data_length - 1])) 
              --data_length;
          }
        }
        column_values_[column] = reinterpret_cast<char *>(
            column_value_allocator_.malloc(data_length + 1));
        memcpy(column_values_[column], work_data, data_length);
        column_values_[column][data_length] = '\0';
        column_valuelengths_[column] = data_length;
        for (;;) {
          SQLINTEGER chunklength;
          resultcode = SQLGetData(sql_hstmt_, 
                                  static_cast<SQLSMALLINT>(column) + 1, 
                                  fetchtype, 
                                  work_data, 
                                  sizeof(work_data) - 1,
                                  &data_length);
          if (SQL_NO_DATA == resultcode) break;
          if (failed(resultcode)) {
            if (resultcode != SQL_NO_DATA) diag_state();
            return false;
          }
          if (data_length > static_cast<SQLLEN>(sizeof(work_data) - 1) && 
              SQL_NO_TOTAL == data_length) {
            chunklength = sizeof(work_data) - 1;
            if (SQL_C_CHAR == fetchtype) {
              while((chunklength > 1) && (work_data[chunklength - 1] == 0)) 
                --chunklength;
            }
          } else {
            chunklength = data_length;
          }
          work_data[chunklength] = '\0';
          column_values_[column] = reinterpret_cast<char *>(
              column_value_allocator_.realloc(
                column_values_[column],
                column_valuelengths_[column] + chunklength + 1));
          memcpy(column_values_[column] + column_valuelengths_[column], 
                 work_data, 
                 chunklength);
          column_values_[column][data_length] = '\0';
        } //for
      } else {
        column_valuelengths_[column] = data_length;
        column_values_[column] = reinterpret_cast<char *>(
            column_value_allocator_.malloc(data_length + 1));
        memcpy(column_values_[column], work_data, data_length);
        column_values_[column][data_length] = '\0';
      }
      if (SQL_C_CHAR == fetchtype && column_values_[column] != NULL) {
        char *target = column_values_[column];
        size_t end = strlen(target);
        while (end > 0 && ' ' == target[end - 1]) target[--end] = '\0';
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

float Interface::get_float(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    if (column_index > column_count_) {
      error_code = QUERY_NO_COLUMN;
      Assert(false);
      return QUERY_NO_COLUMN;
    }
    if (NULL == column_values_[column_index - 1]) {
      error_code = QUERY_NULL;
      Assert(false);
      return 0.0f;
    } else {
      error_code = QUERY_OK;
      return static_cast<float>(atof(get_data(column_index - 1)));
    }
  __LEAVE_FUNCTION
    return 0.0f;
}

int64_t Interface::get_int64(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    using namespace pf_base;
    char temp[32] = {0};
    get_string(column_index, temp, sizeof(temp), error_code);
    int64_t result = string::toint64(temp);
    return result;
  __LEAVE_FUNCTION
    return -1;
}

uint64_t Interface::get_uint64(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    using namespace pf_base;
    char temp[32] = {0};
    get_string(column_index, temp, sizeof(temp), error_code);
    int64_t result = string::touint64(temp);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

int32_t Interface::get_int32(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    if (column_index > column_count_) {
      error_code = QUERY_NO_COLUMN;
      Assert(false);
      return QUERY_NO_COLUMN;
    }
    if (NULL == column_values_[column_index - 1]) {
      error_code = QUERY_NULL;
      Assert(false);
      return QUERY_NULL;
    } else {
      error_code = QUERY_OK;
      int32_t result = atoi(get_data(column_index - 1));
      return result;
    }
  __LEAVE_FUNCTION
    return QUERY_NULL;
}

uint32_t Interface::get_uint32(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    if (column_index > column_count_) {
      error_code = QUERY_NO_COLUMN;
      Assert(false);
      return 0;
    }
    if (NULL == column_values_[column_index - 1]) {
      error_code = QUERY_NULL;
      Assert(false);
      return 0;
    } else {
      error_code = QUERY_OK;
      const char *data = get_data(column_index - 1);
      char *endpointer = NULL;
      double double_value = strtod(data, &endpointer);
      uint32_t result = static_cast<uint32_t>(double_value);
      return result;
    }
  __LEAVE_FUNCTION
    return 0;
}

int16_t Interface::get_int16(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    int16_t result = static_cast<int16_t>(get_int32(column_index, error_code));
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint16_t Interface::get_uint16(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    uint16_t result = 
      static_cast<uint16_t>(get_int32(column_index, error_code));
    return result;
  __LEAVE_FUNCTION
    return 0;
}

int8_t Interface::get_int8(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    int8_t result = static_cast<int8_t>(get_int32(column_index, error_code));
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint8_t Interface::get_uint8(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    uint8_t result = static_cast<uint8_t>(get_int32(column_index, error_code));
    return result;
  __LEAVE_FUNCTION
    return 0;
}

int32_t Interface::get_string(int32_t column_index, 
                              char *buffer, 
                              int32_t buffer_length, 
                              int32_t &error_code) {
  __ENTER_FUNCTION
    if (column_index > column_count_) {
      error_code = QUERY_NO_COLUMN;
      buffer[0] = '\0';
      Assert(false);
      return QUERY_NO_COLUMN;
    }
    if (NULL == column_values_[column_index - 1]) {
      error_code = QUERY_NULL;
      buffer[0] = '\0';
      Assert(false);
    } else {
      int32_t data_length = get_datalength(column_index - 1);
      if (data_length <=buffer_length) {
        strncpy(buffer, get_data(column_index - 1), buffer_length);
      } else {
        char message[8092] = {0};
        snprintf(message, 
                 sizeof(message) - 1, 
                 "buffer_length: %d, data_length: %d, data: %s", 
                 buffer_length,
                 data_length,
                 get_data(column_index - 1));
        AssertEx(false, message);
      }
      error_code = QUERY_OK;
      return data_length;
    }
    return QUERY_NO_COLUMN;
  __LEAVE_FUNCTION
    return QUERY_NO_COLUMN;
}

int32_t Interface::get_binary(int32_t column_index, 
                              char *buffer, 
                              int32_t buffer_length, 
                              int32_t &error_code) {
  __ENTER_FUNCTION
    if (column_index > column_count_) {
      error_code = QUERY_NO_COLUMN;
      buffer[0] = '\0';
      Assert(false);
      return QUERY_NO_COLUMN;
    }
    if (NULL == column_values_[column_index - 1]) {
      error_code = QUERY_NULL;
      buffer[0] = '\0';
      Assert(false);
    } else {
      int32_t data_length = get_datalength(column_index - 1);
      if (data_length <= buffer_length) {
        memcpy(buffer, get_data(column_index - 1), buffer_length);
      } else {
        char message[8092] = {0};
        snprintf(message, 
                 sizeof(message) - 1, 
                 "buffer_length: %d, data_length: %d, data: %s", 
                 buffer_length,
                 data_length,
                 get_data(column_index - 1));
        AssertEx(false, message);
      }
      error_code = QUERY_OK;
      return data_length;
    }
    return QUERY_NO_COLUMN;
  __LEAVE_FUNCTION
    return QUERY_NO_COLUMN;
}
   
int32_t Interface::get_binary_withdecompress(int32_t column_index, 
                                             char *buffer, 
                                             int32_t buffer_length, 
                                             int32_t &error_code) {
  __ENTER_FUNCTION
    if (column_index > column_count_) {
      error_code = QUERY_NO_COLUMN;
      buffer[0] = '\0';
      Assert(false);
      return QUERY_NO_COLUMN;
    }
    if (NULL == column_values_[column_index - 1]) {
      error_code = QUERY_NULL;
      buffer[0] = '\0';
      Assert(false);
    } else {
      error_code = QUERY_OK;
      int32_t data_length = get_datalength(column_index - 1);
      if (0 == data_length) return 0;
      if (false == getcompressor()->decompress(
            reinterpret_cast<const unsigned char *>(
              get_data(column_index - 1)), data_length)) {
        Assert(false);
        return 0;
      }
      int32_t decompress_buffersize = 
        static_cast<int32_t>(getcompressor()->get_decompress_buffersize());
      if (decompress_buffersize > buffer_length) {
        char message[8092] = {0};
        snprintf(message, 
                 sizeof(message) - 1, 
                 "decompress size: %d, realsize: %d", 
                 getcompressor()->get_decompress_buffersize(),
                 data_length);
        AssertEx(false, message);
      }
      memcpy(buffer, 
             getcompressor()->get_decompress_buffer(), 
             getcompressor()->get_decompress_buffersize());
      int32_t result = getcompressor()->get_decompress_buffersize();
      return result;
    }
    return 0;
  __LEAVE_FUNCTION
    return 0;
}

int32_t Interface::get_field(int32_t column_index, 
                             char *buffer, 
                             int32_t buffer_length, 
                             int32_t &error_code) {
  __ENTER_FUNCTION
    if (column_index > column_count_) {
      error_code = QUERY_NO_COLUMN;
      buffer[0] = '\0';
      Assert(false);
      return 0;
    }
    if (NULL == column_values_[column_index - 1]) {
      error_code = QUERY_NULL;
      buffer[0] = '\0';
      Assert(false);
    } else {
      uint32_t out_length = 0;
      pf_base::util::string_tobinary(get_data(column_index - 1), 
                                            get_datalength(column_index - 1), 
                                            buffer, 
                                            buffer_length, 
                                            out_length);
      if (static_cast<int32_t>(out_length) <= buffer_length) {
        error_code = QUERY_OK;
      } else {
        char message[8092] = {0};
        snprintf(message, 
                 sizeof(message) - 1,
                 "buffer_length: %d, data_length: %d, data: %s",
                 buffer_length,
                 get_datalength(column_index - 1),
                 get_data(column_index - 1));
        AssertEx(false, message);
      }
      return out_length;
    }
    return 0;
  __LEAVE_FUNCTION
    return 0;
}

void Interface::diag_state() {
  __ENTER_FUNCTION
    int32_t j = 1;
    SQLINTEGER native_error;
    SQLCHAR sql_state[6] = {0};
    SQLSMALLINT msg_length;
    memset(error_message_, 0, ERROR_MESSAGE_LENGTH_MAX);
    while ((result_ = SQLGetDiagRec(SQL_HANDLE_DBC, 
                                    sql_hdbc_,
                                    static_cast<SQLUSMALLINT>(j), 
                                    sql_state,
                                    &native_error,
                                    error_message_, 
                                    sizeof(error_message_), 
                                    &msg_length)) != SQL_NO_DATA) {
      ++j;
    }
    error_message_[ERROR_MESSAGE_LENGTH_MAX - 1] = '\0';
    if (0 == strlen(reinterpret_cast<const char*>(error_message_))) {
      result_ = SQLError(sql_henv_,
                         sql_hdbc_,
                         sql_hstmt_,
                         sql_state,
                         &native_error,
                         error_message_,
                         sizeof(error_message_),
                         &msg_length);
    }
    error_code_ = native_error;
    switch (error_code_) {
      case 2601: { //repeat
        break;
      }
      case 1062: {
        break;
      }
      default: {
        close();
      }
    }
    char error_buffer[512];
    memset(error_buffer, '\0', sizeof(error_buffer));
    snprintf(error_buffer,
             sizeof(error_buffer) - 1,
             "error code: %d, error msg: %s,error sql: %s", 
             error_code_, 
             error_message_,
             query_.sql_str_);
    save_error_log(error_buffer);
  __LEAVE_FUNCTION
}

void Interface::save_error_log(const char *log) {
  __ENTER_FUNCTION
    if (0 == strlen(log)) return;
    char filename[FILENAME_MAX] = {0};
    snprintf(filename, 
             sizeof(filename) - 1,
             "./log/dberror_%.4d-%.2d-%.2d.log",
             TIME_MANAGER_POINTER->get_year(),
             TIME_MANAGER_POINTER->get_month(),
             TIME_MANAGER_POINTER->get_day());
    FILE *fp = fopen(filename, "a");
    if (fp) {
      fwrite(log, 1, strlen(log), fp);
      fwrite(LF, 1, strlen(LF), fp);
      fclose(fp);
    }
    ERRORPRINTF(log);
  __LEAVE_FUNCTION
}

void Interface::save_warning_log(const char *log) {
  __ENTER_FUNCTION
    if (0 == strlen(log)) return;
    char filename[FILENAME_MAX] = {0};
    snprintf(filename, 
             sizeof(filename) - 1,
             "./log/dbwarning_%.4d-%.2d-%.2d.log",
             TIME_MANAGER_POINTER->get_year(),
             TIME_MANAGER_POINTER->get_month(),
             TIME_MANAGER_POINTER->get_day());
    FILE *fp = fopen(filename, "a");
    if (fp) {
      fwrite(log, 1, strlen(log), fp);
      fwrite(LF, 1, strlen(LF), fp);
      fclose(fp);
    }
    WARNINGPRINTF(log);
  __LEAVE_FUNCTION
}

void Interface::clear_env() {
  __ENTER_FUNCTION
    if (sql_hstmt_) {
      SQLCloseCursor(sql_hstmt_);
      SQLFreeStmt(sql_hstmt_, SQL_UNBIND);
      SQLFreeHandle(SQL_HANDLE_STMT,sql_hstmt_);
      sql_hstmt_ = NULL;
    }
    if (sql_hdbc_) {
      SQLDisconnect(sql_hdbc_);
      SQLFreeHandle(SQL_HANDLE_DBC, sql_hdbc_);
      sql_hdbc_ = NULL;
    }
#ifdef MUST_CLOSE_HENV_HANDLE
    if (sql_henv_) {
      SQLFreeHandle(SQL_HANDLE_ENV, sql_henv_);
      sql_henv_ = NULL;
    }
#endif
  __LEAVE_FUNCTION
}
//dump field
void Interface::dump(int32_t column_index) {
  __ENTER_FUNCTION
    char filename[FILENAME_MAX] = {0};
    snprintf(filename, 
             sizeof(filename) - 1,
             "./log/dbfield_%.4d-%.2d-%.2d.log",
             TIME_MANAGER_POINTER->get_year(),
             TIME_MANAGER_POINTER->get_month(),
             TIME_MANAGER_POINTER->get_day());
    FILE *fp = fopen(filename, "a");
    if (fp) {
      fwrite("begin", 1, 5, fp);
      fwrite(get_data(column_index - 1), 
             1, 
             get_datalength(column_index - 1), 
             fp);
      fclose(fp);
    }
  __LEAVE_FUNCTION
}

int32_t Interface::get_error_code() {
  __ENTER_FUNCTION
    return error_code_;
  __LEAVE_FUNCTION
    return 0;
}

const char *Interface::get_error_message() {
  __ENTER_FUNCTION
    const char *result = reinterpret_cast<const char *>(error_message_);
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

bool Interface::is_connected() {
  __ENTER_FUNCTION
    return connected_;
  __LEAVE_FUNCTION
    return false;
}

int Interface::get_affect_row_count() {
  __ENTER_FUNCTION
    return affect_count_;
  __LEAVE_FUNCTION
    return 0;
}

bool Interface::is_prepare() {
  __ENTER_FUNCTION
    return connected_;
  __LEAVE_FUNCTION
    return false;
}

db_query_t& Interface::get_query() {
   return query_;
}

SQLSMALLINT Interface::get_typemapping(SQLSMALLINT typecode) {
  __ENTER_FUNCTION
    SQLSMALLINT type = typecode; 
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
        type = SQL_C_CHAR;
        break;
      case SQL_DECIMAL:
        break;
      case SQL_NUMERIC:
        type = SQL_C_NUMERIC;
        break;
      case SQL_SMALLINT:
        type = SQL_C_SSHORT;
        break;
      case SQL_INTEGER:
        type = SQL_C_SLONG;
        break;
      case SQL_REAL:
        type = SQL_C_FLOAT;
        break;
      case SQL_FLOAT:
        break;
      case SQL_DOUBLE:
        type = SQL_C_DOUBLE;
        break;
      case SQL_BIT:
        break;
      case SQL_TINYINT:
        break;
      case SQL_BIGINT:
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
        type = SQL_C_CHAR;
        break;
      case SQL_BINARY:
        break;
      case SQL_VARBINARY:
        break;
      case SQL_LONGVARBINARY:
        type = SQL_C_BINARY;
        break;
      default:
        type = SQL_C_CHAR;
        break;
    }
    return type;
  __LEAVE_FUNCTION
    return SQL_C_CHAR;
}

const char *Interface::get_data(int32_t column, const char *_default) {
  __ENTER_FUNCTION
    if (column < 0 || column > column_count_) return _default;
    if (column_values_[column] != NULL) return column_values_[column];
    return _default;
  __LEAVE_FUNCTION
    return _default;
}

const char *Interface::get_data(const char *columnname, const char *_default) {
  __ENTER_FUNCTION
    int32_t column = get_column(columnname);
    const char *data = get_data(column, _default);
    return data;
  __LEAVE_FUNCTION
    return _default;
}

int32_t Interface::get_column(const char *columnname) const {
  __ENTER_FUNCTION
    int32_t result = -1;
    for (int32_t i = 0; i < column_count_; ++i) {
      if (0 == strcmp(columnname, column_names_[i])) {
        result = i;
        break;
      }
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

int32_t Interface::get_datalength(int32_t column) const {
  __ENTER_FUNCTION
    if (column < 0 || column > column_count_) return 0;
    if (column_values_[column] != NULL) return column_valuelengths_[column];
    return 0;
  __LEAVE_FUNCTION
    return 0;
}

int16_t Interface::get_size(int32_t column) const {
  __ENTER_FUNCTION
    if (column < 0 || column > column_count_) return -1;
    int16_t result = static_cast<int16_t>(column_size_[column]);
    return result;
  __LEAVE_FUNCTION
    return -1;
}

int16_t Interface::get_precision(int32_t column) const {
  __ENTER_FUNCTION
    if (column < 0 || column > column_count_) return -1;
    return column_precision_[column];
  __LEAVE_FUNCTION
    return -1;
}

int16_t Interface::get_nullable(int32_t column) const {
  __ENTER_FUNCTION
    if (column < 0 || column > column_count_) return -1;
    return column_nullable_[column];
  __LEAVE_FUNCTION
    return -1;
}
   
const char *Interface::get_name(int32_t column) {
  __ENTER_FUNCTION
    if (column < 0 || column > column_count_) return NULL;
    return column_names_[column];
  __LEAVE_FUNCTION
    return NULL;
}
  
int16_t Interface::get_type(int32_t column) const {
  __ENTER_FUNCTION
    if (column < 0 || column > column_count_) return -1;
    return column_type_[column];
  __LEAVE_FUNCTION
    return -1;
}
   
const char *Interface::get_typename(int16_t column) {
  __ENTER_FUNCTION
    if (column < 0 || column > column_count_) return NULL;
    return column_typenames_[column];
  __LEAVE_FUNCTION
    return NULL;
}

pf_util::compressor::Mini *Interface::getcompressor() {
  __ENTER_FUNCTION
    return &compressor_;
  __LEAVE_FUNCTION
    return NULL;
}

} //namespace odbc

} //namespace pf_db
