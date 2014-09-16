#ifdef __LINUX__
#include <execinfo.h>
#endif
#include <time.h>
#include "pf/sys/assert.h"
int g_command_assert = 0;
bool g_command_ignore_message_box = false; //控制参数，跳过MyMessageBox的中断
int g_need_manager_do_pos_init = 1; //控制参数，是否需要初始化管理器数据

namespace pf_sys {

/**
 * g_command_assert 控制参数
 * 0:会通过弹出对话框让用户选择(缺省值)
 * 1:忽略
 * 2:继续抛出异常用于获取运行堆栈
 **/

void __show__(const char *temp) {
#if __LINUX__
  printf("Assert:%s",temp);
#endif
  //保存日志
#ifndef GAME_CLIENT
  FILE* f = fopen( "./Log/assert.log", "a" ) ;
  fwrite( temp, 1, strlen(temp), f ) ;
  fwrite( "\n", 1, 2, f ) ;
  fclose(f) ;
#endif

#if __WINDOWS__
/**
  static MyLock lock;
  if (1 != g_command_assert) {
    lock.Lock() ;
    int ret = ::MessageBoxA( NULL, temp, "异常", MB_OK ) ;
    lock.Unlock() ;
  }
**/
#elif __LINUX__
#endif

#ifdef GAME_CLIENT
  throw(std::string(temp));
#else
  throw(1);
#endif
}

void __messagebox__(const char *msg) {
  if (g_command_ignore_message_box)
    return;
#if __WINDOWS__
  ::MessageBoxA(NULL, msg, "信息", MB_OK);
#elif __LINUX__
#endif
}

void __assert__ (const char *file, 
                 unsigned int line, 
                 const char *func , 
                 const char *expr) {
  char temp[1024] = {0};
#if __LINUX__ //换个格式
  sprintf(temp, "[%s][%d][%s][%s]\n", file, line, func, expr);
#else
  sprintf(temp, "[%s][%d][%s][%s]", file, line, func, expr);
#endif
  __show__(temp);
}

void __assertex__ (const char *file, 
                   unsigned int line, 
                   const char *func, 
                   const char *expr,
                   const char *msg) {
  char temp[1024] = {0};
#if __LINUX__
  sprintf(temp, "[%s][%d][%s][%s]\n[%s]\n", file, line, func, expr, msg);
#else
  sprintf(temp, "[%s][%d][%s][%s]\n[%s]", file, line, func, expr, msg);
#endif
  __show__(temp);
}

void __assertspecial__ (const char *file, 
                        unsigned int line, 
                        const char *func, 
                        const char *expr,
                        const char *msg) {
  char temp[1024] = {0};
#if __LINUX__
  sprintf(temp, "S[%s][%d][%s][%s]\n[%s]\n", file, line, func, expr, msg) ;
#else
  sprintf(temp, "S[%s][%d][%s][%s]\n[%s]", file, line, func, expr, msg ) ;
#endif
  __show__(temp) ;
}

void __protocol_assert__(const char *file, 
                         unsigned int line, 
                         const char *func, 
                         const char *expr) {
  printf("[%s][%d][%s][%s]", file, line, func, expr);
}

bool RangeCheckForIndex_Assert(int index, 
                               int from, 
                               int to, 
                               char const* code_location) {
  int up_border = 0;
  int low_border = 0;
  char buff[1024];
  if (from > to) {
    up_border = from;
    low_border = to;
  }
  else {
    up_border = to;
    low_border = from;
  }
  Assert(0 <= up_border);
  Assert(0 <= low_border);
  Assert(0 <= index);
  if (index >= low_border && index <= up_border) {
    return true;
  }
  snprintf(buff,
          sizeof(buff),
          "[%s] index out of range!! index=%d, but legal range is [%d, %d]."
          " check it now, please!!",
          code_location,
          index,
          low_border,
          up_border);
  buff[sizeof(buff)-1] = '\0';
  AssertEx(false, buff);
  return false;
}

} //namespace pf_sys
