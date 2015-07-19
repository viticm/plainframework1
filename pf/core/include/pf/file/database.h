/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id database.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/26 17:21
 * @uses as database class
 */
#ifndef PF_FILE_DATABASE_H_
#define PF_FILE_DATABASE_H_

#include "pf/file/config.h"
#include "pf/base/hashmap/config.h"

namespace pf_file {

class PF_API Database {

 public:
   typedef struct file_head_struct {
     uint32_t identify; //标示
     int32_t field_number; //列数
     int32_t record_number; //记录数
     int32_t string_block_size; //字符串区大小
     file_head_struct() {
       identify = FILE_DATABASE_INDENTIFY;
       field_number = -1;
       record_number = -1;
       string_block_size = -1;
     }
   } file_head_t;
   
   typedef enum { //field type
     kTypeInt = 0,
     kTypeFloat = 1,
     kTypeString = 2,
   } field_type_enum;

   typedef std::vector<field_type_enum> field_type;
   
   // 字段数据，即excel的一个单元格
   // 读取的顺序根据从左到右，自上而下的顺序读取，即以行读取
   // 注："[]" 表示excel一个单元格，下面的例子为三列数据 
   // 表格的结构体的数据类型为int32_t、const char *、float三种
   // [value1] [value2] [value3]
   // [value4] [value5] [value6]
   union field_data {
     float float_value;
     int32_t int_value;
     const char *string_value; //can't change excel value in memory
     field_data() {/** do nothing **/}
     field_data(float value) {float_value = value;}
     field_data(int32_t value) {int_value = value;}
     field_data(const char *value) {string_value = value;}
   };

   typedef std::vector<field_data> data_buffer;

 public:
   Database(uint32_t id);
   virtual ~Database();

 public:
   bool open_from_txt(const char *filename);
   bool open_from_memory(const char *memory, 
                         const char *end, 
                         const char *filename = NULL);
   virtual const field_data *search_index_equal(int32_t index) const;
   virtual const field_data *search_position(int32_t line, 
                                             int32_t column) const;
   virtual const field_data *search_first_column_equal(
       int32_t column, 
       const field_data &value) const;
   uint32_t get_id() const; //获得ID
   int32_t get_field_number() const;
   int32_t get_record_number() const;
   const char *get_fieldname(int32_t index);
   int32_t get_fieldindex(const char *name);
   const field_data *get_fielddata(int32_t line, const char *name);
   uint8_t get_fieldtype(int32_t index);
   void create_index(int32_t column = 0, const char *filename = 0);

 public:
   static const char *get_line_from_memory(char *str, 
                                           int32_t size, 
                                           const char *memory,
                                           const char *end);
   static bool field_equal(field_type_enum type, 
                           const field_data &a, 
                           const field_data &b);

 public:
   bool save_tobinary(const char *filename);
   bool save_totext(const char *filename);
   bool save_totext_line(std::vector<std::string> _data);
  
 protected:
   typedef hash_map<int32_t, field_data*> field_hashmap;
   uint32_t id_;
   field_type type_;
   int32_t record_number_;
   int32_t field_number_;
   data_buffer data_buffer_; //所有表格数据容器
   std::vector<std::string> fieldnames_;
   char *string_buffer_; //表格中所有字符串组成的数组，重复的字符串使用一个地址
   int32_t string_buffer_size_;
   field_hashmap hash_index_;
   int32_t index_column_;

 protected:
   bool open_from_memory_text(const char *memory, 
                              const char *end, 
                              const char *filename = NULL);
   bool open_from_memory_binary(const char *memory, 
                                const char *end, 
                                const char *filename = NULL);

};

}; //namespace pf_file

#endif //PF_FILE_DATABASE_H_
