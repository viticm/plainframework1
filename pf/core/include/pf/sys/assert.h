/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id assert.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/18 16:57
 * @uses assert functions
 */
#ifndef PF_SYS_ASSERT_H_
#define PF_SYS_ASSERT_H_

#include "pf/sys/config.h"

extern int g_command_assert; //控制参数，不提示Assert的对话框，直接忽略
extern bool g_command_ignore_message_box;//控制参数，跳过MyMessageBox的中断
extern uint32_t g_file_name_fix_last;
extern int32_t g_file_name_fix;
extern int g_need_manager_do_pos_init;//控制参数，是否需要初始化管理器数据

namespace pf_sys {

PF_API void __assert__(const char *file, 
                       unsigned int line, 
                       const char *func, 
                       const char *expr);
PF_API void __assertex__(const char *file, 
                         unsigned int line, 
                         const char *func, 
                         const char *expr, 
                         const char *msg);
PF_API void __assertspecial__(const char *file, 
                              unsigned int line, 
                              const char *func, 
                              const char *expr, 
                              const char *msg);
PF_API void __messagebox__(const char *msg);
PF_API void __protocol_assert__(const char *file, 
                                unsigned int line, 
                                const char *func, 
                                const char *expr);

//--------------------------------------------------------------------------------
// if nIndex was in [nFrom, nTo], this function will return TRUE. that means legal.
// If any illegal index found, this function will throw a except !!
// the szCodeLocation should be class_name::function_name, or something like this.
// Example: RangeCheckForIndex(i, 0, 10, "testclass::testfunction");
// Legal: 0<=i<=10;
// Illegal: i<0 or i>10;
//--------------------------------------------------------------------------------
PF_API bool RangeCheckForIndex_Assert(int index, 
                                      int from, 
                                      int to, 
                                      char const *code_location);

} //namespace pap_common_sys

#if defined(NDEBUG)
    #define Assert(expr) ((void)0)
    #define AssertEx(expr,msg) ((void)0)
    #define AssertSpecial(expr,msg) ((void)0)
    #define MyMessageBox(msg) ((void)0)
#elif __LINUX__
    #define Assert(expr) {if(!(expr)){pf_sys::__assert__(__FILE__,__LINE__,__PRETTY_FUNCTION__,#expr);}}
    #define ProtocolAssert(expr) ((void)((expr)?0:(pf_sys::__protocol_assert__(__FILE__,__LINE__,__PRETTY_FUNCTION__,#expr),0)))
    #define AssertEx(expr,msg) {if(!(expr)){pf_sys::__assertex__(__FILE__,__LINE__,__PRETTY_FUNCTION__,#expr,msg);}}
    #define AssertSpecial(expr,msg) {if(!(expr)){pf_sys::__assertspecial__(__FILE__,__LINE__,__PRETTY_FUNCTION__,#expr,msg);}}
    #define AssertExPass(expr,msg) {if(!(expr)){pap_common_sys::__assertex__(__FILE__,__LINE__,__PRETTY_FUNCTION__,#expr,msg);}}
    #define MyMessageBox(msg) ((void)0)
#elif defined(__WIN_CONSOLE__) || defined(__WIN32__) || __WINDOWS__
    #define Assert(expr) ((void)((expr)?0:(pf_sys::__assert__(__FILE__,__LINE__,__FUNCTION__,#expr),0)))
    #define AssertEx(expr,msg) ((void)((expr)?0:(pf_sys::__assertex__(__FILE__,__LINE__,__FUNCTION__,#expr,msg),0)))
    #define AssertSpecial(expr,msg) ((void)((expr)?0:(pf_sys::__assertspecial__(__FILE__,__LINE__,__FUNCTION__,#expr,msg),0)))
    #define MyMessageBox(msg) __messagebox__(msg)
#elif defined(__MFC__)
    #define Assert(expr) ASSERT(expr)
    #define AssertEx(expr,msg) ((void)0)
    #define AssertSpecial(expr,msg) ((void)0)
    #define MyMessageBox(msg) ((void)0)
#endif

#endif //PF_SYS_ASSERT_H_
