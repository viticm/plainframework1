#ifndef PF_SYS_UTIL_H_
#define PF_SYS_UTIL_H_

#include "pf/sys/config.h"

namespace pf_sys {

namespace util {

PF_API void dumpstack(const char *log_fileprefix, const char *type);
//执行系统命令，并将结果输出返回到result
PF_API int32_t exec(const char *command, char *result, size_t size);
PF_API bool set_core_rlimit();

}; //namespace util

}; //namespace pf_sys

#endif //PF_SYS_UTIL_H_
