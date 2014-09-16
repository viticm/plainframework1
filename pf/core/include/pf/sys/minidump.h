#ifndef PAP_COMMON_SYS_MINIDUMP_H_
#define PAP_COMMON_SYS_MINIDUMP_H_

#include "pf/sys/config.h"

extern const char *g_dump_exename;

namespace pap_common_sys {

namespace minidump {

#if __WINDOWS__
#include <windows.h>
PF_API LONG WINAPI unhandled_exceptionfilter(
  struct _EXCEPTION_POINTERS* exceptioninfo);
#endif

}; //namespace minidump

}; //namespace pap_common_sys

#endif //PAP_COMMON_SYS_MINIDUMP_H_
