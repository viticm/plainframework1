/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id util.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/18 13:46
 * @uses base util module
 */
#ifndef PF_BASE_UTIL_H_
#define PF_BASE_UTIL_H_

#include "pf/base/config.h"
#include "pf/base/md5.h"

namespace pf_base {

namespace util {

PF_API char value_toascii(char in);
PF_API char ascii_tovalue(char in);
PF_API bool binary_tostring(const char *in, uint32_t in_length, char *out);
PF_API bool string_tobinary(const char *in, 
                            uint32_t in_length, 
                            char *out, 
                            uint32_t out_limit, 
                            uint32_t &out_length);
PF_API void sleep(uint32_t million_seconds);
/**
 * 由byte类型获得单位大小字符串如：32bytes,32kb,32mb,32gb,32t
 * type: -1 获得大于1最大单位字符串，如1023byte则返回1023bytes而非kb
 * type: 0 - 4 分别对应单位bytes、kb、mb、gb、t，
 *       不过规则是返回最至少大于0.01单位的字符串
 */ 
PF_API void get_sizestr(uint64_t size, 
                        char *buffer, 
                        uint32_t length, 
                        int8_t type = -1);

PF_API void path_tounix(char *buffer, uint16_t length);
PF_API void path_towindows(char *buffer, uint16_t length); 

//获取当前执行文件名
PF_API void get_module_filename(char *buffer, size_t size);

PF_API void disable_windowclose();

PF_API bool makedir(const char *path, uint16_t mode = 755);

PF_API uint64_t touint64(uint32_t high, uint32_t low);

PF_API uint32_t get_highsection(uint64_t value);

PF_API uint32_t get_lowsection(uint64_t value);

PF_API void dirname(const char *filepath, char *save);
PF_API void complementpath(char *filepath, size_t size, char delimiter = '/');

} //namespace util

} //namespace pf_base

#endif //PF_BASE_UTIL_H_
