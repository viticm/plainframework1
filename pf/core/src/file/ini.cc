#include <memory>
#include <iostream>
#include "pf/base/string.h"
#include "pf/file/ini.h"

namespace pf_file {

Ini::Ini() {
  __ENTER_FUNCTION
    buffer_ = NULL;
    current_section_ = NULL;
    bufferlength_ = 0;
    bufferlength_max_ = 0;
    memset(filename_, 0, FILENAME_MAX);
  __LEAVE_FUNCTION
}

Ini::Ini(const char *filename) {
  __ENTER_FUNCTION
    buffer_ = NULL;
    current_section_ = NULL;
    bufferlength_ = 0;
    bufferlength_max_ = 0;
    memset(filename_, 0, FILENAME_MAX);
    open(filename);
  __LEAVE_FUNCTION
}

Ini::~Ini() {
  close();
}

bool Ini::open(const char *filename) {
  __ENTER_FUNCTION
    buffer_ = new char[INI_VALUE_MAX];
    bufferlength_max_ += INI_VALUE_MAX; 
    strncpy(filename_, filename, FILENAME_MAX - 1);
    fstream_.open(filename, std::ios_base::in);
    if (!fstream_) {
      char msg[128] = {0};
      snprintf(msg, 128, "[file] open file failed! file: %s", filename);
      AssertEx(false, msg);
    }
    char buffer[512] = {0};
    while (fstream_.getline(buffer, 512)) {
      _trimstring(buffer);
      char *section;
      if(_parsesection(buffer, &section)) {
        current_section_ = new valueset_t;
        sectiondata_.insert(
            std::make_pair(std::string(section), current_section_));
        continue;
      }
      char *key = NULL, *value = NULL;
      if (_parsekey(buffer, &key, &value)) {
        if(current_section_ == NULL) {
          char msg[128] = {0};
          snprintf(msg, 
                   128, 
                   "[file] format invalid! file: %s, key: %s", 
                   filename,
                   key);
          AssertEx(false, msg);
        }
        int position = _add_bufferstring(value);
        current_section_->insert(std::make_pair(std::string(key), position));
        continue;
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Ini::close() {
  __ENTER_FUNCTION
    if (fstream_) fstream_.close();
    SAFE_DELETE_ARRAY(buffer_);
    sectionset_t::iterator iter = sectiondata_.begin();
    for (; iter != sectiondata_.end(); iter++) {
      valueset_t *_section = iter->second;
      SAFE_DELETE(_section);
    }
    sectiondata_.clear();
    bufferlength_ = 0;
    bufferlength_max_ = 0;
    memset(filename_, 0, FILENAME_MAX);
  __LEAVE_FUNCTION
}

Ini::sectionset_t *Ini::getdata() {
  return &sectiondata_;
}

int32_t Ini::getint32(const char *section, const char *key, int32_t _default) {
  __ENTER_FUNCTION
    int32_t result;
    if (_getint32(section, key, result)) return result;
    char msg[128] = {0};
    snprintf(msg, 
             128, 
             "[ini] invalid key! file: %s, section: %s, key: %s", 
             filename_,section,key);
    AssertEx(false, msg);
    return _default;
  __LEAVE_FUNCTION
    return _default;
}

bool Ini::getint32_ifexist(const char *section, 
                           const char *key, 
                           int32_t &result) {
  __ENTER_FUNCTION
    return _getint32(section, key, result);
  __LEAVE_FUNCTION
    return false;  
}

float Ini::getfloat(const char *section, const char *key, float _default) {
  __ENTER_FUNCTION
    float result;
    if (_getfloat(section, key, result)) return result;  
    char msg[128] = {0};
    snprintf(msg, 
             128, 
             "[ini] invalid key! file: %s, section: %s, key: %s", 
             filename_,
             section,
             key);
    AssertEx(false, msg);
    return _default;
  __LEAVE_FUNCTION
    return _default;
}

bool Ini::getfloat_ifexist(const char *section, 
                           const char *key, 
                           float &result) {
  __ENTER_FUNCTION
    return _getfloat(section, key, result);
  __LEAVE_FUNCTION
    return false;
}

bool Ini::getstring(const char *section, 
                    const char *key, 
                    char *str, 
                    int32_t size, 
                    const char *_default) {
  __ENTER_FUNCTION
    if (_getstring(section, key, str, size)) return true;
    strncpy(str, _default, size);
    int32_t _size = static_cast<int32_t>(strlen(_default)) > size - 1 ? 
                    size - 1 : 
                    static_cast<int32_t>(strlen(_default));
    str[_size] = 0;
    char msg[128] = {0};
    snprintf(msg, 
             sizeof(msg) - 1, 
             "[ini] invalid key! file: %s, section: %s, key: %s", 
             filename_,section,key);
    AssertEx(false, msg);
    return false;
  __LEAVE_FUNCTION
    return false;
}

void Ini::get(const char *section, 
              const char *key, 
              pf_base::variable_t &variable) {
  __ENTER_FUNCTION
    using namespace pf_base::string;
    sectionset_t::iterator it = sectiondata_.find(section);
    if(it == sectiondata_.end()) return;
    valueset_t *_section = it->second;
    if (_section != NULL) {
      valueset_t::iterator it2 = _section->find(std::string(key));
      if (it2 == _section->end()) return;
      int position = it2->second;
      const char *value = get_bufferstring(position);
      variable = value;
    }
  __LEAVE_FUNCTION
}

bool Ini::getstring_ifexist(const char *section, 
                            const char *key, 
                            char *str, 
                            int32_t size) {
  __ENTER_FUNCTION
    return _getstring(section, key, str, size);
  __LEAVE_FUNCTION
    return false;
}

bool Ini::_getint32(const char *section, const char *key, int32_t &result) {
  __ENTER_FUNCTION
    sectionset_t::iterator it = sectiondata_.find(section);
    if (it == sectiondata_.end()) return false;
    valueset_t *_section = it->second;
    if (_section != NULL) {
      valueset_t::iterator it2 = _section->find(std::string(key));
      if(it2 == _section->end())
        return false;
      int position = it2->second;
      char *value = get_bufferstring(position);
      result = atoi(value);
      return true;
    }
    return false;
  __LEAVE_FUNCTION
    return false;
}

bool Ini::_getfloat(const char *section, const char *key, float &result) {
  __ENTER_FUNCTION
    sectionset_t::iterator it = sectiondata_.find(section);
    if (it == sectiondata_.end()) return false;
    valueset_t *_section = it->second;
    if (_section != NULL) {
      valueset_t::iterator it2 = _section->find(std::string(key));
      if (it2 == _section->end()) return false;
      int position = it2->second;
      char *value = get_bufferstring(position);
      result = (float)atof(value);
      return true;
    }
    return false;
  __LEAVE_FUNCTION
    return false;
}
   
const char *Ini::getstring(int32_t position) {
  return get_bufferstring(position);
}

bool Ini::_getstring(const char *section, 
                     const char *key, 
                     char *str, 
                     int32_t size) {
  __ENTER_FUNCTION
    using namespace pf_base::string;
    sectionset_t::iterator it = sectiondata_.find(section);
    if(it == sectiondata_.end()) return false;
    valueset_t *_section = it->second;
    if (_section != NULL) {
      valueset_t::iterator it2 = _section->find(std::string(key));
      if (it2 == _section->end()) return false;
      int position = it2->second;
      char *value = get_bufferstring(position);
      size_t valuelength = strlen(value);
      safecopy(str, value, valuelength + 1);
      size_t _size = 
        valuelength > static_cast<size_t>(size - 1) ? size - 1 : valuelength;
      str[_size] = 0;
      return true;
    }
    return false;
  __LEAVE_FUNCTION
    return false;
}

void Ini::_trimstring(char *buffer) {
  __ENTER_FUNCTION
    if ('\r' == buffer[strlen(buffer) - 1] || 
        '\n' == buffer[strlen(buffer) - 1]) {
      buffer[strlen(buffer)-1] = 0;
    }
    size_t i;
    for (i = 0; i < strlen(buffer); ++i ){
      if(buffer[i] == FILE_INI_NOTE) {
        buffer[i] = '\0';
        break;
      }
    }
    for (i = strlen(buffer) - 1; i >= 0; --i) {
      if (' '== buffer[i] || '\t' == buffer[i]) {
        buffer[i] = 0;
      } else {
        break;
      }
    }
  __LEAVE_FUNCTION
}

bool Ini::_parsesection(char *buffer, char **_section) {
  __ENTER_FUNCTION
    if (buffer[0] != '[') return false;
    char *size = strchr(buffer, ']');
    if (size != NULL) {
      size[0] = 0;
      *_section = buffer + 1;
      return true;
    }
    return false;
  __LEAVE_FUNCTION
    return false;
}

bool Ini::_parsekey(char *buffer, char **key, char **value) {
  __ENTER_FUNCTION
    char *size = strchr(buffer, '=');
    if (size != NULL) {
      size[0] = 0;
      *key = buffer;
      *value = size + 1;
      return true;
    }
    return false;
  __LEAVE_FUNCTION
    return false;
}

void Ini::_buffer_resize() {
  __ENTER_FUNCTION
    bufferlength_max_ *= 2;
    buffer_ = (char *)realloc(buffer_, bufferlength_max_);
    if (is_null(buffer_)) Assert(false);
  __LEAVE_FUNCTION
}

char *Ini::get_bufferstring(int32_t position) {
  __ENTER_FUNCTION
    if (position >= bufferlength_) return NULL;
    return buffer_ + position;
  __LEAVE_FUNCTION
    return NULL;
}

int32_t Ini::_add_bufferstring(char *str) {
  __ENTER_FUNCTION
    int32_t position = 0;
    size_t size = strlen(str);
    if (bufferlength_ + static_cast<int32_t>(size + 1) >= bufferlength_max_) 
      _buffer_resize();
    memcpy(buffer_ + bufferlength_, str, size + 1);
    position = bufferlength_;
    bufferlength_ += static_cast<int32_t>(size) + 1;
    return position;
  __LEAVE_FUNCTION
    return 0;
}

} //namespace pf_file
