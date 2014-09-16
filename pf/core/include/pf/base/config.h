/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/18 12:05
 * @uses PLAIN FRAMEWORK base defines
 */
#ifndef PF_BASE_CONFIG_H_
#define PF_BASE_CONFIG_H_

#define DATE_LENGTH_MAX 20

/* base log { */
enum {
  kDebugLogFile = 0,
  kErrorLogFile = 1,
  kNetLogFile = 2,
  kScriptLogFile = 3,
  kEngineLogFile = 4,
  kApplicationLogFile, //应用的日志记录ID
  kLogFileCount,
};
/* } base log */

/* plain framework exports { */
/*
@@ PF_API is a mark for all core API functions.
@@ PFLIB_API is a mark for all auxiliary library functions.
@@ PFMOD_API is a mark for all standard library opening functions.
** CHANGE them if you need to define those functions in some special way.
** For instance, if you want to create one Windows DLL with the core and
** the libraries, you may want to use the following definition (define
** PF_BUILD_AS_DLL to get it).
*/

#if defined(PF_BUILD_AS_DLL) /* { */ 

#if defined(PF_CORE) || defined(PF_LIB) /* { */
#define PF_API __declspec(dllexport)
#else /* }{ */
#define PF_API __declspec(dllimport)
#endif

#else  /* }{ */ 

#define PF_API

#endif /* } */

/* more often than not the libs go together with the core */
#define PFLIB_API  PF_API
#define PFMOD_API  PFLIB_API

/*
@@ PFI_FUNC is a mark for all extern functions that are not to be
@* exported to outside modules.
@@ PFI_DDEF and PFI_DDEC are marks for all extern (const) variables
@* that are not to be exported to outside modules (PFI_DDEF for
@* definitions and PFI_DDEC for declarations).
** CHANGE them if you need to mark them in some special way. Elf/gcc
** (versions 3.2 and later) mark them as "hidden" to optimize access
** when Lua is compiled as a shared library. Not all elf targets support
** this attribute. Unfortunately, gcc does not offer a way to check
** whether the target offers that support, and those without support
** give a warning about it. To avoid these warnings, change to the
** default definition.
*/
#if defined(__GNUC__) && ((__GNUC__*100 + __GNUC_MINOR__) >= 302) && \
  defined(__ELF__)		/* { */
#define PFI_FUNC	__attribute__((visibility("hidden"))) extern
#define PFI_DDEC	PFI_FUNC
#define PFI_DDEF	/* empty */

#else				/* }{ */
#define PFI_FUNC
#define PFI_DDEC
#define PFI_DDEF	/* empty */
#endif				/* } */
/* } plain framework exports */

//platform define
#ifndef __WINDOWS__ /* { */ //windows上的宏处理与gnu有区别，所以这样定义
#if defined(_MSC_VER) || defined(__ICL) || defined(WIN32) || defined(WIN64)
#define __WINDOWS__ 1
#else
#define __WINDOWS__ 0
#endif
#endif /* } */

#ifndef __LINUX__
#define __LINUX__ !(__WINDOWS__)
#endif

//network size define
#ifndef FD_SETSIZE 
#define FD_SETSIZE 4906
#endif

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif //use c99

//system include
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#if __WINDOWS__ //diffrent system include
#include <windows.h>
#include <crtdbg.h>
#include <tchar.h>
#include <direct.h>
#include <io.h>
#elif __LINUX__
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <execinfo.h>
#include <signal.h>
#include <exception>
#include <setjmp.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/resource.h>
#endif
#include "pf/sys/assert.h"
#include "pf/base/io.h"
//warning the namespace can't use like this, remember it
//using namespace std;

//基本数据类型定义
//typedef unsigned char ubyte; //0~255 --use uint8_t
//typedef char byte; //-128~127 --use int8_t

#include "pf/base/atomic.h"
#include "pf/base/global.h" //全局定义

#define IP_SIZE 24 //max ip size
#if __LINUX__
#define HANDLE_INVALID (-1)
#elif __WINDOWS__
#define HANDLE_INVALID ((VOID*)0)
#endif
#define ID_INVALID (-1)
#define INDEX_INVALID (-1)
#define TAB_PARAM_ID_INVALID (-9999) //invalid id in excel param

#ifndef UCHAR_MAX
#define UCHAR_MIN (0)
#define UCHAR_MAX (0xFF)
#endif

#ifndef BYTE_MAX
#define BYTE_MIN UCHAR_MIN
#define BYTE_MAX UCHAR_MAX
#endif

// common define
#if __LINUX__
#ifndef LF
#define LF "\n"
#endif
#elif __WINDOWS__
#ifndef LF
#define LF "\r\n"
#endif
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

//c output/string functions
#if __WINDOWS__
#ifndef snprintf
#define snprintf _snprintf
#endif
#ifndef stricmp
#define stricmp _stricmp
#endif
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#define strtoint64(pointer,endpointer,base) \
  _strtoi64(pointer,endpointer,base)
#define strtouint64(pointer,endpointer,base) \
  _strtoui64(pointer,endpointer,base)
#elif __LINUX__
#ifndef stricmp
#define stricmp strcasecmp
#define strtoint64(pointer,endpointer,base) strtoll(pointer,endpointer,base)
#define strtouint64(pointer,endpointer,base) strtoull(pointer,endpointer,base) 
#endif
#endif

#if __WINDOWS__
#define access _access
#define mkdir(dir,mode) _mkdir(dir)
#endif

//根据指针值删除内存
#ifndef SAFE_DELETE
#if __WINDOWS__
#define SAFE_DELETE(x)	if ((x) != NULL) { \
  Assert(_CrtIsValidHeapPointer(x)); \
  delete (x); (x) = NULL; \
}

#elif __LINUX__
#define SAFE_DELETE(x)	if ((x) != NULL) { delete (x); (x) = NULL; }
#endif
#endif
//根据指针值删除数组类型内存 
//其中注意_CrtIsValidHeapPointer前的定义，在release需要定义NDEBUG
#ifndef SAFE_DELETE_ARRAY
#if __WINDOWS__
#define SAFE_DELETE_ARRAY(x) if ((x) != NULL) { \
  Assert(_CrtIsValidHeapPointer(x)); \
  delete[] (x); (x) = NULL; \
}
#elif __LINUX__
#define SAFE_DELETE_ARRAY(x) if ((x) != NULL) { delete[] (x); (x) = NULL; }
#endif
#endif

//删除指针型数据(应尽量使用SAFE_DELETE_ARRAY)
#ifndef DELETE_ARRAY
#if __WINDOWS__
#define DELETE_ARRAY(x)	if ((x) != NULL) { delete[] (x); (x) = NULL; }
#elif __LINUX__
#define DELETE_ARRAY(x)	if ((x) != NULL) { delete[] (x); (x) = NULL; }
#endif
#endif

#if __LINUX__
#define __stdcall
#endif

#ifndef USE_PARAM
#define USE_PARAM(x) if (!x) {}
#endif
#define USE_PARAMEX(x) if (!(&x)) {}
//根据指针调用free接口
#ifndef SAFE_FREE
#define SAFE_FREE(x) if ((x) != NULL) { free(x); (x) = NULL; }
#endif
//根据指针调用Release接口
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)	if ((x) != NULL) { (x)->release(); (x) = NULL; }
#endif

/* io { */
#ifndef ERRORPRINTF
#define ERRORPRINTF baseio_perror
#endif

#ifndef WARNINGPRINTF
#define WARNINGPRINTF baseio_pwarn
#endif

#ifndef DEBUGPRINTF
#define DEBUGPRINTF baseio_pdebug
#endif
/* } io */

#if __WINDOWS__ //normal functions
#if defined(NDEBUG)
#define __ENTER_FUNCTION { try {
#define __LEAVE_FUNCTION } catch(...){ \
  if (0 == APPLICATION_TYPE) AssertSpecial(false,__FUNCTION__); } }
#else
#define __ENTER_FUNCTION { try {
#define __LEAVE_FUNCTION } catch(...){ \
  if (0 == APPLICATION_TYPE) AssertSpecial(false,__FUNCTION__); } }
#endif

#elif __LINUX__    //linux
#define __ENTER_FUNCTION { try {
#define __LEAVE_FUNCTION } catch(...) \
  { if (0 == APPLICATION_TYPE) AssertSpecial(false,__PRETTY_FUNCTION__); } }
#endif

//headers include order: pf/base/config.h -> sys include -> module include

#if __WINDOWS__ //header fix

//disable: "no suitable definition provided for explicit template
//instantiation request" Occurs in VC7 for no justifiable reason on all
//#includes of Singleton
#pragma warning(disable: 4661)

//why use it? for FD_* functions
#pragma warning(disable: 4127)

//for template classes
#pragma warning(disable: 4786)

//disable: "<type> needs to have dll-interface to be used by clients'
//Happens on STL member variables which are not public therefore is ok
#pragma warning (disable: 4251)

//unreachable code
#pragma warning (disable: 4702)

#endif

#endif //PF_BASE_CONFIG_H_
