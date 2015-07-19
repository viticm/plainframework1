#include "pf/base/string.h"
#include "pf/base/define.h"

namespace pf_base {

variable_struct::variable_struct() {
  type = kVariableTypeInvalid;
}

variable_struct::variable_struct(const variable_t &object) {
  __ENTER_FUNCTION
    data = object.data;
    type = object.type;
  __LEAVE_FUNCTION
}

variable_struct::variable_struct(const variable_t *object) {
  __ENTER_FUNCTION
    if (object) {
      data = object->data;
      type = object->type;
    }
  __LEAVE_FUNCTION
}
  
variable_struct::variable_struct(bool value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    data = value ? 1 : 0;
  __LEAVE_FUNCTION
}
 
variable_struct::variable_struct(int32_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRIu32"", value);
    data = temp;
  __LEAVE_FUNCTION
}
 
variable_struct::variable_struct(uint32_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRIu32"", value);
    data = temp;
  __LEAVE_FUNCTION
}
  
variable_struct::variable_struct(int16_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRId16"", value);
    data = temp;
  __LEAVE_FUNCTION
}

variable_struct::variable_struct(uint16_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRIu16"", value);
    data = temp;
  __LEAVE_FUNCTION
}
  
variable_struct::variable_struct(int8_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRId8"", value);
    data = temp;
  __LEAVE_FUNCTION
}

variable_struct::variable_struct(uint8_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRIu8"", value);
    data = temp;
  __LEAVE_FUNCTION
}

variable_struct::variable_struct(int64_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRId64"", value);
    data = temp;
  __LEAVE_FUNCTION
}
 
variable_struct::variable_struct(uint64_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRId64"", value);
    data = temp;
  __LEAVE_FUNCTION
}

variable_struct::variable_struct(float value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%f", value);
    data = temp;
  __LEAVE_FUNCTION
}

variable_struct::variable_struct(double value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%f", value);
    data = temp;
  __LEAVE_FUNCTION
}
  
variable_struct::variable_struct(std::string value) {
  __ENTER_FUNCTION
    type = kVariableTypeString;
    data = value;
  __LEAVE_FUNCTION
}
  
variable_struct::variable_struct(const char *value) {
  __ENTER_FUNCTION
    type = kVariableTypeString;
    data = value;
  __LEAVE_FUNCTION
}
 
bool variable_struct::_bool() const {
  __ENTER_FUNCTION
    bool result = int64() != 0;
    return result;
  __LEAVE_FUNCTION
    return 0;
}
 
int32_t variable_struct::int32() const {
  __ENTER_FUNCTION
    int32_t result = static_cast<int32_t>(int64());
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint32_t variable_struct::uint32() const {
  __ENTER_FUNCTION
    uint32_t result = static_cast<uint32_t>(int64());
    return result;
  __LEAVE_FUNCTION
    return 0;
}

int16_t variable_struct::int16() const {
  __ENTER_FUNCTION
    int16_t result = static_cast<int16_t>(int64());
    return result;
  __LEAVE_FUNCTION
    return 0;
}
  
uint16_t variable_struct::uint16() const {
  __ENTER_FUNCTION
    uint16_t result = static_cast<uint16_t>(int64());
    return result;
  __LEAVE_FUNCTION
    return 0;
}

int8_t variable_struct::int8() const {
  __ENTER_FUNCTION
    int8_t result = static_cast<int8_t>(int64());
    return result;
  __LEAVE_FUNCTION
    return 0;
}
  
uint8_t variable_struct::uint8() const {
  __ENTER_FUNCTION
    uint8_t result = static_cast<uint8_t>(int64());
    return result;
  __LEAVE_FUNCTION
    return 0;
}
  
int64_t variable_struct::int64() const {
  __ENTER_FUNCTION
    int64_t result = pf_base::string::toint64(data.c_str());
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t variable_struct::uint64() const {
  __ENTER_FUNCTION
    uint64_t result = static_cast<uint64_t>(int64());
    return result;
  __LEAVE_FUNCTION
    return 0;
}
  
float variable_struct::_float() const {
  __ENTER_FUNCTION
    float result = static_cast<float>(_double());
    return result;
  __LEAVE_FUNCTION
    return .0f;
}
  
double variable_struct::_double() const {
  __ENTER_FUNCTION
    double result = atof(data.c_str());
    return result;
  __LEAVE_FUNCTION
    return .0;
}
  
const char *variable_struct::string() const {
  __ENTER_FUNCTION
    return data.c_str();
  __LEAVE_FUNCTION
    return NULL;
}

variable_t &variable_struct::operator = (const variable_t &object) {
  __ENTER_FUNCTION
    data = object.data;
    type = object.type;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t *variable_struct::operator = (const variable_t *object) {
  __ENTER_FUNCTION
    if (object) {
      data = object->data;
      type = object->type;
    }
    return this;
  __LEAVE_FUNCTION
    return this;
}
  
variable_t &variable_struct::operator = (int32_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRId32"", value);
    data = temp;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator = (uint32_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRIu32"", value);
    data = temp;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator = (int16_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRId16"", value);
    data = temp;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator = (uint16_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRIu16"", value);
    data = temp;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator = (int8_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRId8"", value);
    data = temp;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator = (uint8_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRIu8"", value);
    data = temp;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator = (int64_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRId64"", value);
    data = temp;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator = (uint64_t value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%"PRIu64"", value);
    data = temp;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

variable_t &variable_struct::operator = (float value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%f", value);
    data = temp;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

variable_t &variable_struct::operator = (double value) {
  __ENTER_FUNCTION
    type = kVariableTypeInt32;
    char temp[128] = {0};
    snprintf(temp, sizeof(temp) - 1, "%f", value);
    data = temp;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator = (std::string value) {
  __ENTER_FUNCTION
    type = kVariableTypeString;
    data = value;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator = (const char *value) {
  __ENTER_FUNCTION
    type = kVariableTypeString;
    data = value;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

variable_t &variable_struct::operator += (const variable_t &object) {
  __ENTER_FUNCTION
    switch (type) {
      case kVariableTypeInt32:
        *this += object.int32();
        break;
      case kVariableTypeUint32:
        *this += object.uint32();
        break;
      case kVariableTypeInt16:
        *this += object.int16();
        break;
      case kVariableTypeUint16:
        *this += object.uint16();
        break;
      case kVariableTypeInt8:
        *this += object.int8();
        break;
      case kVariableTypeUint8:
        *this += object.uint8();
        break;
      case kVariableTypeInt64:
        *this += object.int64();
        break;
      case kVariableTypeUint64:
        *this += object.uint64();
        break;
      case kVariableTypeFloat:
        *this += object._float();
        break;
      case kVariableTypeDouble:
        *this += object._double();
        break;
      default:
        *this += object.data;
        break;
    }
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t *variable_struct::operator += (const variable_t *object) {
  __ENTER_FUNCTION
    if (object) *this += *object;
    return this;
  __LEAVE_FUNCTION
    return this;
}

variable_t &variable_struct::operator += (int32_t value) {
  __ENTER_FUNCTION
    int32_t finalvalue = int32() + value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator += (uint32_t value) {
  __ENTER_FUNCTION
    uint32_t finalvalue = uint32() + value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
 
variable_t &variable_struct::operator += (int16_t value) {
  __ENTER_FUNCTION
    int16_t finalvalue = int16() + value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator += (uint16_t value) {
  __ENTER_FUNCTION
    uint16_t finalvalue = uint16() + value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator += (int8_t value) {
  __ENTER_FUNCTION
    int8_t finalvalue = int8() + value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator += (uint8_t value) {
  __ENTER_FUNCTION
    uint8_t finalvalue = uint8() + value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator += (int64_t value) {
  __ENTER_FUNCTION
    int64_t finalvalue = int64() + value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator += (uint64_t value) {
  __ENTER_FUNCTION
    uint64_t finalvalue = uint64() + value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

variable_t &variable_struct::operator += (float value) {
  __ENTER_FUNCTION
    float finalvalue = value + value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator += (double value) {
  __ENTER_FUNCTION
    double finalvalue = _double() + value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator += (std::string value) {
  __ENTER_FUNCTION
    data += value;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator += (const char *value) {
  __ENTER_FUNCTION
    data += value;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

variable_t &variable_struct::operator -= (const variable_t &object) {
  __ENTER_FUNCTION
    switch (type) {
      case kVariableTypeInt32:
        *this -= object.int32();
        break;
      case kVariableTypeUint32:
        *this -= object.uint32();
        break;
      case kVariableTypeInt16:
        *this -= object.int16();
        break;
      case kVariableTypeUint16:
        *this -= object.uint16();
        break;
      case kVariableTypeInt8:
        *this -= object.int8();
        break;
      case kVariableTypeUint8:
        *this -= object.uint8();
        break;
      case kVariableTypeInt64:
        *this -= object.int64();
        break;
      case kVariableTypeUint64:
        *this -= object.uint64();
        break;
      case kVariableTypeFloat:
        *this -= object._float();
        break;
      case kVariableTypeDouble:
        *this -= object._double();
        break;
      default:
        *this -= object.data;
        break;
    }
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t *variable_struct::operator -= (const variable_t *object) {
  __ENTER_FUNCTION
    if (object) *this -= *object;
    return this;
  __LEAVE_FUNCTION
    return this;
}

variable_t &variable_struct::operator -= (int32_t value) {
  __ENTER_FUNCTION
    int32_t finalvalue = int32() - value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator -= (uint32_t value) {
  __ENTER_FUNCTION
    uint32_t finalvalue = uint32() - value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
 
variable_t &variable_struct::operator -= (int16_t value) {
  __ENTER_FUNCTION
    int16_t finalvalue = int16() - value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator -= (uint16_t value) {
  __ENTER_FUNCTION
    uint16_t finalvalue = uint16() - value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator -= (int8_t value) {
  __ENTER_FUNCTION
    int8_t finalvalue = int8() - value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator -= (uint8_t value) {
  __ENTER_FUNCTION
    uint8_t finalvalue = uint8() - value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator -= (int64_t value) {
  __ENTER_FUNCTION
    int64_t finalvalue = int64() - value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator -= (uint64_t value) {
  __ENTER_FUNCTION
    uint64_t finalvalue = uint64() - value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

variable_t &variable_struct::operator -= (float value) {
  __ENTER_FUNCTION
    float finalvalue = _float() - value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator -= (double value) {
  __ENTER_FUNCTION
    double finalvalue = _double() - value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator *= (const variable_t &object) {
  __ENTER_FUNCTION
    switch (type) {
      case kVariableTypeInt32:
        *this *= object.int32();
        break;
      case kVariableTypeUint32:
        *this *= object.uint32();
        break;
      case kVariableTypeInt16:
        *this *= object.int16();
        break;
      case kVariableTypeUint16:
        *this *= object.uint16();
        break;
      case kVariableTypeInt8:
        *this *= object.int8();
        break;
      case kVariableTypeUint8:
        *this *= object.uint8();
        break;
      case kVariableTypeInt64:
        *this *= object.int64();
        break;
      case kVariableTypeUint64:
        *this *= object.uint64();
        break;
      case kVariableTypeFloat:
        *this *= object._float();
        break;
      case kVariableTypeDouble:
        *this *= object._double();
        break;
      default:
        break;
    }
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t *variable_struct::operator *= (const variable_t *object) {
  __ENTER_FUNCTION
    if (object) *this *= *object;
    return this;
  __LEAVE_FUNCTION
    return this;
}

variable_t &variable_struct::operator *= (int32_t value) {
  __ENTER_FUNCTION
    int32_t finalvalue = int32() * value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator *= (uint32_t value) {
  __ENTER_FUNCTION
    uint32_t finalvalue = uint32() * value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
 
variable_t &variable_struct::operator *= (int16_t value) {
  __ENTER_FUNCTION
    int16_t finalvalue = int16() * value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator *= (uint16_t value) {
  __ENTER_FUNCTION
    uint16_t finalvalue = uint16() * value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator *= (int8_t value) {
  __ENTER_FUNCTION
    int8_t finalvalue = int8() * value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator *= (uint8_t value) {
  __ENTER_FUNCTION
    uint8_t finalvalue = uint8() * value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator *= (int64_t value) {
  __ENTER_FUNCTION
    int64_t finalvalue = int64() * value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator *= (uint64_t value) {
  __ENTER_FUNCTION
    uint64_t finalvalue = uint64() * value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

variable_t &variable_struct::operator *= (float value) {
  __ENTER_FUNCTION
    float finalvalue = _float() * value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator *= (double value) {
  __ENTER_FUNCTION
    double finalvalue = _double() * value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

variable_t &variable_struct::operator /= (const variable_t &object) {
  __ENTER_FUNCTION
    switch (type) {
      case kVariableTypeInt32:
        *this /= object.int32();
        break;
      case kVariableTypeUint32:
        *this /= object.uint32();
        break;
      case kVariableTypeInt16:
        *this /= object.int16();
        break;
      case kVariableTypeUint16:
        *this /= object.uint16();
        break;
      case kVariableTypeInt8:
        *this /= object.int8();
        break;
      case kVariableTypeUint8:
        *this /= object.uint8();
        break;
      case kVariableTypeInt64:
        *this /= object.int64();
        break;
      case kVariableTypeUint64:
        *this /= object.uint64();
        break;
      case kVariableTypeFloat:
        *this /= object._float();
        break;
      case kVariableTypeDouble:
        *this /= object._double();
        break;
      default:
        break;
    }
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

variable_t *variable_struct::operator /= (const variable_t *object) {
  __ENTER_FUNCTION
    if (object) *this /= *object;
    return this;
  __LEAVE_FUNCTION
    return this;
}

variable_t &variable_struct::operator /= (int32_t value) {
  __ENTER_FUNCTION
    int32_t finalvalue = int32() / value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator /= (uint32_t value) {
  __ENTER_FUNCTION
    uint32_t finalvalue = uint32() / value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
 
variable_t &variable_struct::operator /= (int16_t value) {
  __ENTER_FUNCTION
    int16_t finalvalue = int16() / value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator /= (uint16_t value) {
  __ENTER_FUNCTION
    uint16_t finalvalue = uint16() / value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator /= (int8_t value) {
  __ENTER_FUNCTION
    int8_t finalvalue = int8() / value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator /= (uint8_t value) {
  __ENTER_FUNCTION
    uint8_t finalvalue = uint8() / value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator /= (int64_t value) {
  __ENTER_FUNCTION
    int64_t finalvalue = int64() / value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator /= (uint64_t value) {
  __ENTER_FUNCTION
    uint64_t finalvalue = uint64() / value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

variable_t &variable_struct::operator /= (float value) {
  __ENTER_FUNCTION
    float finalvalue = _float() / value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator /= (double value) {
  __ENTER_FUNCTION
    double finalvalue = _double() / value;
    *this = finalvalue;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}  

variable_t &variable_struct::operator ++ () {
  __ENTER_FUNCTION
    *this += 1;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator -- () {
  __ENTER_FUNCTION
    *this -= 1;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

variable_t &variable_struct::operator ++ (int32_t) {
  __ENTER_FUNCTION
    *this += 1;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}
  
variable_t &variable_struct::operator -- (int32_t) {
  __ENTER_FUNCTION
    *this -= 1;
    return *this;
  __LEAVE_FUNCTION
    return *this;
}

bool variable_struct::operator == (const variable_t &object) const {
  return data == object.data;
}
  
bool variable_struct::operator == (const variable_t *object) const {
  __ENTER_FUNCTION
    if (is_null(object)) return false;
    return data == object->data;
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator == (const int32_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this == variable;
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator == (const uint32_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this == variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator == (const int16_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this == variable;
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator == (const uint16_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this == variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator == (const int8_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this == variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator == (const uint8_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this == variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator == (const int64_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this == variable;
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator == (const uint64_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this == variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator == (const float value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this == variable;
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator == (const double value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this == variable;
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator == (const std::string value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this == variable;
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator == (const char *value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this == variable;
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator != (const variable_t &object) const {
  return !(data == object.data);
}
  
bool variable_struct::operator != (const variable_t *object) const {
  __ENTER_FUNCTION
    if (is_null(object)) return false;
    return !(data == object->data);
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator != (const int32_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return !(*this == variable);
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator != (const uint32_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return !(*this == variable);
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator != (const int16_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return !(*this == variable);
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator != (const uint16_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return !(*this == variable);
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator != (const int8_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return !(*this == variable);
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator != (const uint8_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return !(*this == variable);
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator != (const int64_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return !(*this == variable);
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator != (const uint64_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return !(*this == variable);
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator != (const float value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return !(*this == variable);
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator != (const double value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return !(*this == variable);
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator != (const std::string value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return !(*this == variable);
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator != (const char *value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return !(*this == variable);
  __LEAVE_FUNCTION
    return false;
}
bool variable_struct::operator < (const variable_t &object) const {
  __ENTER_FUNCTION
    bool result = data < object.data;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator < (const variable_t *object) const {
  __ENTER_FUNCTION
    bool result = false;
    if (object) result = data < object->data;
    return result;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator < (const int32_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this < variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator < (const uint32_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this < variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator < (const int16_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this < variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator < (const uint16_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this < variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator < (const int8_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this < variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator < (const uint8_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this < variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator < (const int64_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this < variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator < (const uint64_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this < variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator < (const float value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this < variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator < (const double value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this < variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator < (const std::string value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this < variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator < (const char *value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this < variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator > (const variable_t &object) const {
  __ENTER_FUNCTION
    return data > object.data;
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator > (const variable_t *object) const {
  __ENTER_FUNCTION
    if (is_null(object)) return true;
    return data > object->data;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator > (const int32_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this > variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator > (const uint32_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this > variable;
  __LEAVE_FUNCTION
    return false;
}

bool variable_struct::operator > (const int16_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this > variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator > (const uint16_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this > variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator > (const int8_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this > variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator > (const uint8_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this > variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator > (const int64_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this > variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator > (const uint64_t value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this > variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator > (const float value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this > variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator > (const double value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this > variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator > (const std::string value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this > variable;
  __LEAVE_FUNCTION
    return false;
}
  
bool variable_struct::operator > (const char *value) const {
  __ENTER_FUNCTION
    variable_t variable = value;
    return *this > variable;
  __LEAVE_FUNCTION
    return false;
}

} //namespace pf_base
