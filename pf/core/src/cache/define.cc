#include "pf/base/string.h"
#include "pf/cache/define.h"

namespace pf_cache {

//Construct function.
db_saveitem_struct::db_saveitem_struct() {
  __ENTER_FUNCTION
    clear();
  __LEAVE_FUNCTION
}

void db_saveitem_struct::clear() {
  __ENTER_FUNCTION
    memset(tablename, 0, sizeof(tablename));
    memset(prefix, 0, sizeof(prefix));
    memset(keys, 0, sizeof(keys));
    memset(types, 0, sizeof(types));
    memset(condition, 0, sizeof(condition));
    paramex[0] = -1;
    paramex[1] = -1;
    paramex[2] = -1;
    paramex[3] = -1;
  __LEAVE_FUNCTION
}  

db_saveitem_t &db_saveitem_struct::operator = (const db_saveitem_t &object) {
  __ENTER_FUNCTION
    using namespace pf_base::string;
    safecopy(tablename, object.tablename, sizeof(tablename));
    safecopy(prefix, object.prefix, sizeof(prefix));
    safecopy(keys, object.keys, sizeof(keys));
    safecopy(types, object.types, sizeof(types));
    safecopy(condition, object.condition, sizeof(condition));
    paramex[0] = object.paramex[0];
    paramex[1] = object.paramex[1];
    paramex[2] = object.paramex[2];
    paramex[3] = object.paramex[3];
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
db_saveitem_t *db_saveitem_struct::operator = (const db_saveitem_t *object) {
  __ENTER_FUNCTION
    *this = *object;
    return this;
  __LEAVE_FUNCTION
    return this;
}

bool db_saveitem_struct::datato_fetch_array(db_fetch_array_t &array,
                                            char *data,
                                            int32_t datalength) {
  __ENTER_FUNCTION
    using namespace pf_base::string;
    if (0 == datalength) return false;
    array.clear();
    if (strlen(keys) > 0) {
      std::vector<std::string> _array;
      explode(keys, _array, "\t", true, true);
      for (uint32_t i = 0; i < _array.size(); ++i) {
        pf_base::variable_t variable = _array[i];
        array.keys.push_back(variable);
      }
    }
    if (NULL == data || '\0' == data[0]) return false;
    std::vector<std::string> typearray;
    explode(types, typearray, "\t", true, true);
    if (typearray.size() != array.keys.size()) return false;
    array.values.clear();
    char temp[1024 * 100] = {0};
    std::vector<std::string> valuearray;
    safecopy(temp, data, sizeof(temp));
    explode(temp, valuearray, "\t", true, true);
    for (uint32_t i = 0; i < valuearray.size(); ++i) {
      int8_t type = 
        static_cast<int8_t>(toint64(typearray[i % typearray.size()].c_str()));
      pf_base::variable_t variable;
      variable = valuearray[i];
      variable.type = type;
      array.values.push_back(variable);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}  

bool db_saveitem_struct::fetch_array_todata(db_fetch_array_t &array,
                                            char *data,
                                            int32_t datalength) {
  __ENTER_FUNCTION
    using namespace pf_base::string;
    if (!isvalid_fetch_array(array, datalength)) return false;
    memset(keys, 0, sizeof(keys));
    uint32_t keysize = static_cast<uint32_t>(array.keys.size());
    uint32_t i = 0;
    for (i = 0; i < keysize; ++i) {
      const char *format = 0 == i ? "%s" : "\t%s";
      uint32_t havelength = static_cast<uint32_t>(strlen(keys));
      snprintf(keys + havelength, 
               sizeof(keys) - 1, 
               format, 
               array.keys[i].string());
    }
    memset(types, 0, sizeof(types));
    memset(data, 0, datalength);
    for (i = 0; i < array.values.size(); ++i) {
      if (i < array.keys.size()) {
        const char *format = 0 == i ? "%d" : "\t%d";
        uint32_t length = static_cast<uint32_t>(strlen(types));
        snprintf(types + length, 
                 sizeof(types) - 1, 
                 format, 
                 array.values[i].type);
      }
      const char *format = 0 == i ? "%s" : "\t%s";
      uint32_t havelength = static_cast<uint32_t>(strlen(data));
      if (static_cast<int32_t>(havelength) >= datalength - 1) break;
      snprintf(data + havelength, 
               datalength - 1, 
               format,  
               array.values[i].string());
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}
  
bool db_saveitem_struct::isvalid_fetch_array(db_fetch_array_t &array,
                                             int32_t datalength) {
  __ENTER_FUNCTION
    db_keys_t::iterator _iterator;
    if (array.values.size() != 0) {
      if ((array.values.size() % array.keys.size()) != 0) return false;
      if (array.size() < 1) return false;
    }
    uint32_t keysize = static_cast<uint32_t>(array.keys.size());
    if (keysize > sizeof(types) - 1) return false;
    uint32_t i = 0;
    for (i = 0; i < keysize; ++i) {
      if (!is_null(strstr("\t", array.keys[i].string()))) return false;
    }
    uint32_t datasize = 0;   
    for (i = 0; i < array.values.size(); ++i) {
      if (!is_null(strstr("\t", array.values[i].string()))) return false;
      datasize += static_cast<uint32_t>(strlen(array.values[i].string()));
    }
    bool result = datalength > static_cast<int32_t>(datasize);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool db_saveitem_struct::savecondition(pf_base::variable_array_t &array) {
  __ENTER_FUNCTION
    if (!isvalid_condition(array)) return false;
    memset(condition, 0, sizeof(condition));
    for (uint32_t i = 0; i < array.size(); ++i) {
      const char *format = 0 == i ? "%s" : "\t%s";
      uint32_t havelength = static_cast<uint32_t>(strlen(condition));
      snprintf(condition + havelength,
               sizeof(condition) - 1, 
               format, 
               array[i].string());
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool db_saveitem_struct::getcondition(pf_base::variable_array_t &array) {
  __ENTER_FUNCTION
    using namespace pf_base::string;
    if (0 == strlen(condition)) return true; //空条件
    array.clear();
    std::vector<std::string> _array;
    explode(condition, _array, "\t", true, true);
    for (uint32_t i = 0; i < _array.size(); ++i) {
      pf_base::variable_t variable = _array[i];
      array.push_back(variable);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}
  
bool db_saveitem_struct::isvalid_condition(
    pf_base::variable_array_t &array) {
  __ENTER_FUNCTION
    uint32_t datasize = 0;
    for (uint32_t i = 0; i < array.size(); ++i) {
      if (!is_null(strstr("\t", array[i].string()))) return false;
      datasize += static_cast<uint32_t>(strlen(array[i].string()));
    }
    bool result = sizeof(condition) > datasize;
    return result;
  __LEAVE_FUNCTION
    return false;
}

db_saveitemex_struct::db_saveitemex_struct() {
  clear();
}

void db_saveitemex_struct::clear() {
  memset(data, 0, sizeof(data));
}
  
db_saveitemex_t &
  db_saveitemex_struct::operator = (const db_saveitemex_t &object) {
  __ENTER_FUNCTION
    using namespace pf_base::string;
    safecopy(data, object.data, sizeof(data));
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
db_saveitemex_t *
  db_saveitemex_struct::operator = (const db_saveitemex_t *object) {
  __ENTER_FUNCTION
    if (object) *this = *object;
    return this;
  __LEAVE_FUNCTION
    return this;
}  

}; //namespace pf_cache
