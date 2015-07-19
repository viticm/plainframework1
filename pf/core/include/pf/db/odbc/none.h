/**
 * PAP Engine ( https://github.com/viticm/plainframework1 )
 * $Id none.h
 * @link https://github.com/viticm/plainframework1 for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com/viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com/viticm.ti@gmail.com>
 * @date 2015/05/11 12:54
 * @uses your description
 */
#ifndef PF_DB_ODBC_NONE_H_
#define PF_DB_ODBC_NONE_H_

#include "pf/db/odbc/config.h"
#include "pf/util/compressor/mini.h"

namespace pf_db {

namespace odbc {

class PF_API Interface {

 public:
   Interface() {};
   ~Interface() {};
   
 public:
   bool connect(const char *, const char *, const char *) { return true; };
   bool connect() { return true; };
   bool close() { return true; };
   bool failed(int32_t) { return true; };
   bool collect_resultinfo() { return true; };
   int32_t get_error_code() { return -1; };
   const char *get_error_message() { return NULL; };
   bool is_connected() { return false; };
   int32_t get_affect_row_count() { return 0; };
   bool is_prepare() { return false; };
   void clear() {};
   void clear_no_commit() {};
   void clear_column_info() {};
   void clear_column_data() {};
   int32_t get_columncount() const { return 0; };
   bool fetch(int32_t, int32_t) { return true; } ;
   db_query_t *get_query() { return NULL; };
   bool execute() { return true; };
   bool execute(const char *) { return true; } ;

 public:
   float get_float(int32_t, int32_t &) { return .0f; };
   int64_t get_int64(int32_t, int32_t &) { return 0; };
   uint64_t get_uint64(int32_t, int32_t &) { return 0; };
   int32_t get_int32(int32_t, int32_t &) { return 0; };
   uint32_t get_uint32(int32_t, int32_t &) { return 0; };
   int16_t get_int16(int32_t, int32_t &) { return 0; };
   uint16_t get_uint16(int32_t, int32_t &) { return 0; };
   int8_t get_int8(int32_t, int32_t &) { return 0; };
   uint8_t get_uint8(int32_t, int32_t &) { return 0; };
   int32_t get_string(int32_t, char *, int32_t, int32_t &) { return 0; };
   int32_t get_field(int32_t, char *, int32_t, int32_t &) { return 0; };
   int32_t get_binary(int32_t, char *, int32_t, int32_t &) { return 0; };
   int32_t get_binary_withdecompress(int32_t, 
                                     char *, 
                                     int32_t, 
                                     int32_t &) { return 0; };
   const char *get_data(int32_t, const char *) { return NULL; };
   const char *get_data(const char *, const char *) { return NULL; };
   int32_t get_datalength(int32_t) const { return 0; };
   int32_t get_index(const char *) const { return 0; };
   int16_t get_size(int32_t) const { return 0; };
   int16_t get_precision(int32_t) const { return 0; };
   int16_t get_nullable(int32_t) const { return 0; } ;
   const char *get_name(int32_t) { return NULL; };
   int16_t get_type(int32_t) const { return 0; };
   const char *get_typename(int16_t) { return NULL; };
   pf_util::compressor::Mini *getcompressor() { return NULL; };

   //分析
   void diag_state() {};
   void save_error_log(const char *) {};
   void save_warning_log(const char *) {};
   void clear_env() {};
   void dump(int32_t) {};

};

}; //namespace odbc

}; //namespace pf_db

#endif //PF_DB_ODBC_NONE_H_
