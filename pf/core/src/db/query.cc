#include "pf/base/string.h"
#include "pf/db/manager.h"
#include "pf/db/query.h"

namespace pf_db {

Query::Query() {
  __ENTER_FUNCTION
    db_query_ = NULL;
    manager_ = NULL;
    isready_ = false;
    memset(tablename_, 0, sizeof(tablename_));
  __LEAVE_FUNCTION
}

Query::Query(db_query_t *db_query) {
  __ENTER_FUNCTION
    manager_ = NULL;
    isready_ = false;
    memset(tablename_, 0, sizeof(tablename_));
    db_query_ = db_query;
    if (!is_null(db_query_)) isready_ = true;
  __LEAVE_FUNCTION
}

   
Query::~Query() {
  //do nothing
}

bool Query::init(Manager *manager) {
  __ENTER_FUNCTION
    if (NULL == manager || !manager->get_internal_query()) return false;
    manager_ = manager;
    isready_ = true;
    return true;
  __LEAVE_FUNCTION
    return false;
}

Manager *Query::getmanager() {
  return manager_;
}

db_query_t *Query::get_db_query() {
  __ENTER_FUNCTION
    db_query_t *result = NULL;
    result = is_null(db_query_) ? manager_->get_internal_query() : db_query_;
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

void Query::set_tablename(const char *tablename) {
  __ENTER_FUNCTION
    pf_base::string::safecopy(tablename_, tablename, sizeof(tablename_));
  __LEAVE_FUNCTION
}

bool Query::execute() {
  __ENTER_FUNCTION
    if (!isready_ || is_null(manager_)) return false;
    bool result = manager_->query();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Query::select(const char *string) {
  __ENTER_FUNCTION
    if (!isready_) return false;
    db_query_t *db_query = get_db_query();
    db_query->clear();
    db_query->concat("select %s", string);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Query::_delete(const char *string) {
  __ENTER_FUNCTION
    if (!isready_) return false;
    db_query_t *db_query = get_db_query();
    db_query->clear();
    db_query->concat("delete %s", string);
    return true;
  __LEAVE_FUNCTION
    return false;
}
   
bool Query::where(const char *string) {
  __ENTER_FUNCTION
    if (!isready_) return false;
    db_query_t *db_query = get_db_query();
    db_query->concat(" where %s", string);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Query::select(pf_base::variable_array_t &values) {
  __ENTER_FUNCTION
    if (!isready_) return false;
    uint32_t count = static_cast<uint32_t>(values.size());
    if (0 == count) return false;
    db_query_t *db_query = get_db_query();
    db_query->clear();
    db_query->concat("select");
    for (uint32_t i = 0; i < count; ++i) {
      db_query->concat(" %s%s", 
                       values[i].string(), 
                       i == count - 1 ? "" : ",");
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}
   
bool Query::_delete() {
  __ENTER_FUNCTION
    if (!isready_) return false;
    db_query_t *db_query = get_db_query();
    db_query->clear();
    db_query->concat("delete");
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Query::insert(pf_base::variable_array_t &keys, 
                   pf_base::variable_array_t &values) {
  __ENTER_FUNCTION
    if (!isready_) return false;
    if (0 == strlen(tablename_)) return false;
    uint32_t keycount = static_cast<uint32_t>(keys.size());
    uint32_t valuecount = static_cast<uint32_t>(values.size());
    if (0 == keycount || keycount != valuecount) return false;
    db_query_t *db_query = get_db_query();
    db_query->clear();
    uint32_t i;
    db_query->concat("insert into %s", tablename_);
    db_query->concat(" (");
    for (i = 0; i < keycount; ++i) {
      db_query->concat("%s%s",
                       keys[i].string(),
                       i == keycount - 1 ? "" : ", ");
    }
    db_query->concat(") values (");
    for (i = 0; i < keycount; ++i) { 
      db_query->concat(pf_base::kVariableTypeString == values[i].type ? 
                       "\'%s\'%s" : 
                       "%s%s",
                       values[i].string(),
                       i == keycount - 1 ? "" : ", ");
    }
    db_query->concat(")");
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Query::update(pf_base::variable_array_t &keys, 
                   pf_base::variable_array_t &values) {
  __ENTER_FUNCTION
    if (!isready_) return false;
    if (0 == strlen(tablename_)) return false;
    uint32_t keycount = static_cast<uint32_t>(keys.size());
    uint32_t valuecount = static_cast<uint32_t>(values.size());
    if (0 == keycount || keycount != valuecount) return false;
    db_query_t *db_query = get_db_query();
    db_query->clear();
    uint32_t i;
    db_query->concat("update %s set ", tablename_);
    for (i = 0; i < keycount; ++i) { 
      db_query->concat(pf_base::kVariableTypeString == values[i].type ? 
                       "%s=\'%s\'%s" : 
                       "%s=%s%s",
                       keys[i].string(),
                       values[i].string(),
                       i == keycount - 1 ? "" : ", ");
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}
   
bool Query::from() {
  __ENTER_FUNCTION
    if (!isready_) return false;
    if (0 == strlen(tablename_)) return false;
    db_query_t *db_query = get_db_query();
    db_query->concat(" from %s", tablename_);
    return true;
  __LEAVE_FUNCTION
    return false;
}
 
bool Query::where(pf_base::variable_t &key, 
                  pf_base::variable_t &value, 
                  const char *operator_str) {
  __ENTER_FUNCTION
    if (!isready_) return false;
    db_query_t *db_query = get_db_query();
    db_query->concat(" where ");
    db_query->concat(pf_base::kVariableTypeString == value.type ? 
                     "%s%s\'%s\'" : 
                     "%s%s%s",
                     key.string(),
                     operator_str,
                     value.string());
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Query::_and(pf_base::variable_t &key, 
                 pf_base::variable_t &value, 
                 const char *operator_str) {
  __ENTER_FUNCTION
    if (!isready_) return false;
    db_query_t *db_query = get_db_query();
    db_query->concat(" and ");
    db_query->concat(pf_base::kVariableTypeString == value.type ? 
                     "%s%s\'%s\'" : 
                     "%s%s%s",
                     key.string(),
                     operator_str,
                     value.string());
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Query::_or(pf_base::variable_t &key, 
                pf_base::variable_t &value, 
                const char *operator_str) {
  __ENTER_FUNCTION
    if (!isready_) return false;
    db_query_t *db_query = get_db_query();
    db_query->concat(" or ");
    db_query->concat(pf_base::kVariableTypeString == value.type ? 
                     "%s%s\'%s\'" : 
                     "%s%s%s",
                     key.string(),
                     operator_str,
                     value.string());
    return true;
  __LEAVE_FUNCTION
    return false;

}
   
bool Query::limit(int32_t m, int32_t n) {
  __ENTER_FUNCTION
    if (!isready_) return false;
    db_query_t *db_query = get_db_query();
    if (0 == n) {
      db_query->concat(" limit %d", m);
    } else {
      db_query->concat(" limit %d, %d", m, n);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Query::fetcharray(db_fetch_array_t &db_fetch_array) {
  __ENTER_FUNCTION
    using namespace pf_base;
    if (!isready_ || is_null(manager_)) return false;
    if (!manager_->fetch()) return false;
    int32_t columncount = manager_->get_columncount();
    if (columncount <= 0) return false;
    int32_t i = 0;
    //read keys
    for (i = 0; i < columncount; ++i) {
      const char *columnname = manager_->get_columnname(i);
      db_fetch_array.keys.push_back(columnname);
    }
    //read values
    do {
      for (i = 0; i < columncount; ++i) {
        variable_t value = manager_->get_data(i, "");
        int8_t columntype = manager_->gettype(i);
        value.type = static_cast<int8_t>(kDBColumnTypeString) == columntype ? 
                     static_cast<int8_t>(kVariableTypeString) :
                     static_cast<int8_t>(kVariableTypeNumber);
        db_fetch_array.values.push_back(value);
      }
    } while (manager_->fetch());
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace pf_db
