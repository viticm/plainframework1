/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id manager.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/30 19:51
 * @uses the db manager class
 */
#ifndef PF_DB_MANAGER_H_
#define PF_DB_MANAGER_H_

#include "pf/db/config.h"
#include "pf/db/odbc/system.h"

namespace pf_db {

class PF_API Manager {

 public:
   Manager();
   ~Manager();

 public:
   bool init(const char *connection_or_dbname,
             const char *username,
             const char *password);
   int8_t get_connector_type() const;
   void set_connector_type(int8_t type);
   db_query_t *get_internal_query();
   bool query();
   bool fetch(int32_t orientation = 1, int32_t offset = 0);
   int32_t get_affectcount() const;
   bool check_db_connect();
   bool isready() const;
   bool getresult() const;
   int32_t get_columncount() const;
   const char *get_columnname(int32_t column_index) const;

 public:
   float get_float(int32_t column_index, int32_t &error_code);
   int64_t get_int64(int32_t column_index, int32_t &error_code);
   uint64_t get_uint64(int32_t column_index, int32_t &error_code);
   int32_t get_int32(int32_t column_index, int32_t &error_code);
   uint32_t get_uint32(int32_t column_index, int32_t &error_code);
   int16_t get_int16(int32_t column_index, int32_t &error_code);
   uint16_t get_uint16(int32_t column_index, int32_t &error_code);
   int8_t get_int8(int32_t column_index, int32_t &error_code);
   uint8_t get_uint8(int32_t column_index, int32_t &error_code);
   int32_t get_string(int32_t column_index, 
                      char *buffer, 
                      int32_t buffer_length, 
                      int32_t &error_code);
   int32_t get_field(int32_t column_index, 
                     char *buffer, 
                     int32_t buffer_length, 
                     int32_t &error_code);
   int32_t get_binary(int32_t column_index, 
                      char *buffer, 
                      int32_t buffer_length, 
                      int32_t &error_code);
   int32_t get_binary_withdecompress(int32_t column_index, 
                                     char *buffer, 
                                     int32_t buffer_length, 
                                     int32_t &error_code);
   const char *get_data(int32_t column_index, const char *_default) const;
   int8_t gettype(int32_t column_index);

 protected:
   int8_t connector_type_;
   odbc::System *odbc_system_;
   bool isready_;

};

}; //namespace pf_db

#endif //PF_DB_MANAGER_H_
