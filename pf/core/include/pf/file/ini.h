/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework1 )
 * $Id ini.h
 * @link https://github.com/viticm/plainframework1 for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com/viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com/viticm.ti@gmail.com>
 * @date 2015/05/08 22:22
 * @uses file ini class
 */
#ifndef PF_FILE_INI_H_
#define PF_FILE_INI_H_

#include "pf/file/config.h"
#include "pf/base/hashmap/template.h"

#define INI_VALUE_MAX 1024
#define FILE_INI_NOTE (';')

namespace pf_file {

class PF_API Ini {

 public:
   Ini();
   Ini(const char *filename);
   virtual ~Ini();

 public:
   typedef std::map<std::string, int32_t> valueset_t;
   typedef std::map<std::string, valueset_t *> sectionset_t;

 public:
   bool open(const char *filename);
   void close();

 public:

   //Get variable.
   void get(const char *section, 
            const char *key, 
            pf_base::variable_t &variable);

   //Get data.
   sectionset_t *getdata();

   //Read string by position;
   const char *getstring(int32_t position);

 public:

   int32_t getint32(const char *section, const char *key, int32_t _default = 0);
   bool getint32_ifexist(const char *section, const char *key, int32_t &result);
   float getfloat(const char *section, const char *key, float _default = 0.0f);
   bool getfloat_ifexist(const char *section, const char *key, float &result);
   const char *getstring(const char *section, const char *key);
   bool getstring(const char *section, 
                  const char *key, 
                  char *str, 
                  int32_t size, 
                  const char *_default = "");
   bool getstring_ifexist(const char *section, 
                          const char *key, 
                          char *str, 
                          int32_t size);

 private:
   bool _getint32(const char *section, const char *key, int32_t &result);
   bool _getfloat(const char *section, const char *key, float &result);
   bool _getstring(const char *section, 
                   const char *key, 
                   char *str, 
                   int32_t size);
   void _trimstring(char *buffer);
   bool _parsesection(char *buffer, char **section);
   bool _parsekey(char *buffer, char **key, char **value);
   void _buffer_resize();
   char *get_bufferstring(int32_t position);
   int32_t _add_bufferstring(char *str);

 private:
   std::ifstream fstream_;
   char filename_[FILENAME_MAX];
   sectionset_t sectiondata_;
   valueset_t *current_section_;
   char *buffer_;
   int32_t bufferlength_;
   int32_t bufferlength_max_;

};

}; //namespace pf_file

#endif //PF_FILE_INI_H_
