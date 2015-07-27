/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/pap )
 * $Id define.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/09 14:30
 * @uses base defines file
 *       cn: 基础定义，结构扩展
 */
#ifndef PF_BASE_DEFINE_H_
#define PF_BASE_DEFINE_H_

#include "pf/base/config.h"

namespace pf_base {

enum {
  kVariableTypeInvalid = -1,
  kVariableTypeBool,
  kVariableTypeInt32,
  kVariableTypeUint32,
  kVariableTypeInt16,
  kVariableTypeUint16,
  kVariableTypeInt8,
  kVariableTypeUint8,
  kVariableTypeInt64,
  kVariableTypeUint64,
  kVariableTypeFloat,
  kVariableTypeDouble,
  kVariableTypeString,
  kVariableTypeNumber,
}; //变量的类型

typedef struct PF_API variable_struct variable_t;
struct variable_struct { //该变量禁止频繁使用，只做方便查询和缓存
  std::string data;
  int8_t type;
  variable_struct();
  variable_struct(const variable_t &object);
  variable_struct(const variable_t *object);
  variable_struct(bool value);
  variable_struct(int32_t value);
  variable_struct(uint32_t value);
  variable_struct(int16_t value);
  variable_struct(uint16_t value);
  variable_struct(int8_t value);
  variable_struct(uint8_t value);
  variable_struct(int64_t value);
  variable_struct(uint64_t value);
  variable_struct(float value);
  variable_struct(double value);
  variable_struct(std::string value);
  variable_struct(const char *value);
  bool _bool() const;
  int32_t int32() const;
  uint32_t uint32() const;
  int16_t int16() const;
  uint16_t uint16() const;
  int8_t int8() const;
  uint8_t uint8() const;
  int64_t int64() const;
  uint64_t uint64() const;
  float _float() const;
  double _double() const;
  const char *string() const;
  variable_t &operator = (const variable_t &object);
  variable_t *operator = (const variable_t *object);
  variable_t &operator = (bool value);
  variable_t &operator = (int32_t value);
  variable_t &operator = (uint32_t value);
  variable_t &operator = (int16_t value);
  variable_t &operator = (uint16_t value);
  variable_t &operator = (int8_t value);
  variable_t &operator = (uint8_t value);
  variable_t &operator = (int64_t value);
  variable_t &operator = (uint64_t value);
  variable_t &operator = (float value);
  variable_t &operator = (double value);
  variable_t &operator = (std::string value);
  variable_t &operator = (const char *value);
  variable_t &operator += (const variable_t &object);
  variable_t *operator += (const variable_t *object);
  variable_t &operator += (int32_t value);
  variable_t &operator += (uint32_t value);
  variable_t &operator += (int16_t value);
  variable_t &operator += (uint16_t value);
  variable_t &operator += (int8_t value);
  variable_t &operator += (uint8_t value);
  variable_t &operator += (int64_t value);
  variable_t &operator += (uint64_t value);
  variable_t &operator += (float value);
  variable_t &operator += (double value);
  variable_t &operator += (std::string value);
  variable_t &operator += (const char *value);
  variable_t &operator -= (const variable_t &object);
  variable_t *operator -= (const variable_t *object);
  variable_t &operator -= (int32_t value);
  variable_t &operator -= (uint32_t value);
  variable_t &operator -= (int16_t value);
  variable_t &operator -= (uint16_t value);
  variable_t &operator -= (int8_t value);
  variable_t &operator -= (uint8_t value);
  variable_t &operator -= (int64_t value);
  variable_t &operator -= (uint64_t value);
  variable_t &operator -= (float value);
  variable_t &operator -= (double value);
  variable_t &operator *= (const variable_t &object);
  variable_t *operator *= (const variable_t *object);
  variable_t &operator *= (int32_t value);
  variable_t &operator *= (uint32_t value);
  variable_t &operator *= (int16_t value);
  variable_t &operator *= (uint16_t value);
  variable_t &operator *= (int8_t value);
  variable_t &operator *= (uint8_t value);
  variable_t &operator *= (int64_t value);
  variable_t &operator *= (uint64_t value);
  variable_t &operator *= (float value);
  variable_t &operator *= (double value);
  variable_t &operator /= (const variable_t &object);
  variable_t *operator /= (const variable_t *object);
  variable_t &operator /= (int32_t value);
  variable_t &operator /= (uint32_t value);
  variable_t &operator /= (int16_t value);
  variable_t &operator /= (uint16_t value);
  variable_t &operator /= (int8_t value);
  variable_t &operator /= (uint8_t value);
  variable_t &operator /= (int64_t value);
  variable_t &operator /= (uint64_t value);
  variable_t &operator /= (float value);
  variable_t &operator /= (double value);
  variable_t &operator ++ ();
  variable_t &operator -- ();
  variable_t &operator ++ (int32_t);
  variable_t &operator -- (int32_t);
  bool operator == (const variable_t &object) const;
  bool operator == (const variable_t *object) const;
  bool operator == (const bool value) const;
  bool operator == (const int32_t value) const;
  bool operator == (const uint32_t value) const;
  bool operator == (const int16_t value) const;
  bool operator == (const uint16_t value) const;
  bool operator == (const int8_t value) const;
  bool operator == (const uint8_t value) const;
  bool operator == (const int64_t value) const;
  bool operator == (const uint64_t value) const;
  bool operator == (const float value) const;
  bool operator == (const double value) const;
  bool operator == (const std::string value) const;
  bool operator == (const char *value) const;
  bool operator != (const variable_t &object) const;
  bool operator != (const variable_t *object) const;
  bool operator != (const int32_t value) const;
  bool operator != (const uint32_t value) const;
  bool operator != (const int16_t value) const;
  bool operator != (const uint16_t value) const;
  bool operator != (const int8_t value) const;
  bool operator != (const uint8_t value) const;
  bool operator != (const int64_t value) const;
  bool operator != (const uint64_t value) const;
  bool operator != (const float value) const;
  bool operator != (const double value) const;
  bool operator != (const std::string value) const;
  bool operator != (const char *value) const;
  bool operator < (const variable_t &object) const; //for map
  bool operator < (const variable_t *object) const;
  bool operator < (const int32_t value) const;
  bool operator < (const uint32_t value) const;
  bool operator < (const int16_t value) const;
  bool operator < (const uint16_t value) const;
  bool operator < (const int8_t value) const;
  bool operator < (const uint8_t value) const;
  bool operator < (const int64_t value) const;
  bool operator < (const uint64_t value) const;
  bool operator < (const float value) const;
  bool operator < (const double value) const;
  bool operator < (const std::string value) const;
  bool operator < (const char *value) const;
  bool operator > (const variable_t &object) const; //for map
  bool operator > (const variable_t *object) const;
  bool operator > (const int32_t value) const;
  bool operator > (const uint32_t value) const;
  bool operator > (const int16_t value) const;
  bool operator > (const uint16_t value) const;
  bool operator > (const int8_t value) const;
  bool operator > (const uint8_t value) const;
  bool operator > (const int64_t value) const;
  bool operator > (const uint64_t value) const;
  bool operator > (const float value) const;
  bool operator > (const double value) const;
  bool operator > (const std::string value) const;
  bool operator > (const char *value) const;


}; //综合变量结构，类似脚本的所有变量，基础数据为字符串 只使用在数据库和缓存

typedef std::vector<variable_t> variable_array_t; //禁止使用索引赋值
typedef std::map<std::string, variable_t> variable_set_t; //

}; //namespace pf_base

#endif //PF_BASE_DEFINE_H_
