#include "pf/db/odbc/interface.h"
#include "pf/db/manager.h"

namespace pf_db {

Manager::Manager() {
  __ENTER_FUNCTION
    connector_type_ = kDBConnectorTypeODBC;
    odbc_system_ = NULL;
    isready_ = false;
  __LEAVE_FUNCTION
}

Manager::~Manager() {
  __ENTER_FUNCTION
    SAFE_DELETE(odbc_system_);
  __LEAVE_FUNCTION
}

bool Manager::init(const char *connection_or_dbname,
                   const char *username,
                   const char *password) {
  __ENTER_FUNCTION
    bool result = true;
    switch (connector_type_) {
      case kDBConnectorTypeODBC: {
        odbc_system_ = new odbc::System();
        Assert(odbc_system_);
        result = 
          odbc_system_->init(connection_or_dbname, username, password);
        isready_ = odbc_system_->getinterface()->is_prepare();
        break;
      }
      default:
        result = false;
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

int8_t Manager::get_connector_type() const {
  return connector_type_;
}
void Manager::set_connector_type(int8_t type) {
  connector_type_ = type;
}

int32_t Manager::get_columncount() const {
  __ENTER_FUNCTION
    int32_t result = 0;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = odbc_system_->get_columncount();
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return 0;
}

bool Manager::getresult() const {
  __ENTER_FUNCTION
    bool result = true;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = odbc_system_->getresult();
        break;
      default:
        result = false;
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Manager::query() {
  __ENTER_FUNCTION
    bool result = true;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = odbc_system_->query();
        break;
      default:
        result = false;
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Manager::fetch(int32_t orientation, int32_t offset) {
  __ENTER_FUNCTION
    bool result = true;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = odbc_system_->fetch(orientation, offset);
        break;
      default:
        result = false;
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

db_query_t *Manager::get_internal_query() {
  __ENTER_FUNCTION
    db_query_t *query_pointer = NULL;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        query_pointer = odbc_system_->get_internal_query();
        break;
      default:
        query_pointer = NULL;
        break;
    }
    return query_pointer;
  __LEAVE_FUNCTION
    return NULL;
}

int32_t Manager::get_affectcount() const {
  __ENTER_FUNCTION
    int32_t result = 0;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = odbc_system_->get_result_count();
        break;
      default:
        result = -1;
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

bool Manager::check_db_connect() {
  __ENTER_FUNCTION
    bool result = true;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = odbc_system_->check_db_connect();
        break;
      default:
        result = false;
        break;
    }
    isready_ = result; //Check connect.
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Manager::isready() const {
  return isready_;
}

float Manager::get_float(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    float result = 0.0f;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_float(column_index, error_code);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return 0.0f;
}

int64_t Manager::get_int64(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    int64_t result = -1;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_int64(column_index, error_code);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

uint64_t Manager::get_uint64(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    uint64_t result = 0;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_uint64(column_index, error_code);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return 0;
}

int32_t Manager::get_int32(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    int32_t result = -1;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_int32(column_index, error_code);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

uint32_t Manager::get_uint32(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    uint32_t result = 0;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_uint32(column_index, error_code);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
int16_t Manager::get_int16(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    int16_t result = -1;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_int16(column_index, error_code);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

uint16_t Manager::get_uint16(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    uint16_t result = 0;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_uint16(column_index, error_code);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
int8_t Manager::get_int8(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    int8_t result = -1;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_int8(column_index, error_code);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

uint8_t Manager::get_uint8(int32_t column_index, int32_t &error_code) {
  __ENTER_FUNCTION
    uint8_t result = 0;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_int8(column_index, error_code);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return 0;
}
   
int32_t Manager::get_string(int32_t column_index, 
                            char *buffer, 
                            int32_t buffer_length, 
                            int32_t &error_code) {
  __ENTER_FUNCTION
    int32_t result = -1;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_string(column_index,
                                                   buffer,
                                                   buffer_length,
                                                   error_code);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

int32_t Manager::get_field(int32_t column_index, 
                           char *buffer, 
                           int32_t buffer_length, 
                           int32_t &error_code) {
  __ENTER_FUNCTION
    int32_t result = -1;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_field(column_index,
                                                  buffer,
                                                  buffer_length,
                                                  error_code);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

int32_t Manager::get_binary(int32_t column_index, 
                            char *buffer, 
                            int32_t buffer_length, 
                            int32_t &error_code) {
  __ENTER_FUNCTION
    int32_t result = -1;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_binary(column_index,
                                                   buffer,
                                                   buffer_length,
                                                   error_code);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

int32_t Manager::get_binary_withdecompress(int32_t column_index, 
                                           char *buffer, 
                                           int32_t buffer_length, 
                                           int32_t &error_code) {
  __ENTER_FUNCTION
    int32_t result = -1;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_binary_withdecompress(
              column_index,
              buffer,
              buffer_length,
              error_code);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

const char *Manager::get_columnname(int32_t column_index) const {
  __ENTER_FUNCTION
    const char *result = NULL;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_name(column_index);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

const char *Manager::get_data(
    int32_t column_index, const char *_default) const {
  __ENTER_FUNCTION
    const char *result = NULL;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        result = 
          odbc_system_->getinterface()->get_data(column_index, _default);
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

int8_t Manager::gettype(int32_t column_index) {
  __ENTER_FUNCTION
    int8_t type = kDBColumnTypeString;
    switch (connector_type_) {
      case kDBConnectorTypeODBC:
        type = odbc_system_->gettype(column_index);
        break;
      default:
        break;
    }
    return type;
  __LEAVE_FUNCTION
    return kDBColumnTypeString;
}

} //namespace pf_db
