/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id query.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/08 21:33
 * @uses database query class
 *       cn: 数据库查询器，统一各种语法查询，目前功能较简单
 */
#ifndef PF_DB_QUERY_H_
#define PF_DB_QUERY_H_

#include "pf/db/config.h"

namespace pf_db {

class PF_API Query {

 public:
   Query();
   Query(db_query_t *db_query);
   ~Query();

 public:
   bool init(Manager *manager);
   Manager *getmanager();
   void set_tablename(const char *tablename);
   db_query_t *get_db_query();

 public:
   bool select(const char *string);
   bool _delete(const char *string);
   bool insert(const char *string);
   bool update(const char *string);
   bool where(const char *string);

 public:
   bool select(pf_base::variable_array_t &values);
   bool _delete();
   bool insert(pf_base::variable_array_t &keys, 
               pf_base::variable_array_t &values);
   bool update(pf_base::variable_array_t &keys, 
               pf_base::variable_array_t &values);
   bool from();
   bool where(pf_base::variable_t &key, 
              pf_base::variable_t &value, 
              const char *operator_str);
   bool _and(pf_base::variable_t &key, 
             pf_base::variable_t &value, 
             const char *operator_str);
   bool _or(pf_base::variable_t &key, 
            pf_base::variable_t &value, 
            const char *operator_str);
   bool limit(int32_t m, int32_t n = 0);

 public:
   bool execute();
   bool fetcharray(db_fetch_array_t &db_fetch_array);

 private:
   char tablename_[DB_TABLENAME_LENGTH];
   Manager *manager_;
   db_query_t *db_query_;
   bool isready_;

};

}; //namespace pf_db

#endif //PF_DB_QUERY_H_
