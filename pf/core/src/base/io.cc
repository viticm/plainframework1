#include <iostream>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "pf/base/config.h"
#include "pf/base/io.h"

#if __WINDOWS__
uint16_t set_consolecolor(uint16_t forecolor = 0, 
                          uint16_t background_color = 0) {
  CONSOLE_SCREEN_BUFFER_INFO consolescreen_bufferinfo;
  if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), 
                                  &consolescreen_bufferinfo)) {
    return 0;
  }
  if (!SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                               forecolor | background_color)) {
    return 0;
  }
  return consolescreen_bufferinfo.wAttributes;
}
void reset_consolecolor(uint16_t color) {
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
#endif

void baseio_perror(const char *format, ...) {
  char buffer[2048] = {0};
#if __LINUX__ /* { */
  const char *head = "\e[0;31;1m";
  const char *end = "\e[0m";
#elif __WINDOWS__ /* }{ */
  uint16_t lastcolor = set_consolecolor(FOREGROUND_INTENSITY | FOREGROUND_RED);
  const char *head = "";
  const char *end = "";
#endif /* } */
  va_list argptr;
  va_start(argptr, format);
  vsnprintf(buffer, sizeof(buffer) - 1, format, argptr);
  va_end(argptr);
  std::cout<<head<<buffer<<end<<std::endl;
#if __WINDOWS__
  reset_consolecolor(lastcolor);
#endif
}

void baseio_pwarn(const char *format, ...) {
  char buffer[2048] = {0};
#if __LINUX__ /* { */
  const char *head = "\e[0;33;1m";
  const char *end = "\e[0m";
#elif __WINDOWS__ /* }{ */
  uint16_t lastcolor = 
    set_consolecolor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
  const char *head = "";
  const char *end = "";
#endif /* } */
  va_list argptr;
  va_start(argptr, format);
  vsnprintf(buffer, sizeof(buffer) - 1, format, argptr);
  va_end(argptr);
  std::cout<<head<<buffer<<end<<std::endl;
#if __WINDOWS__
  reset_consolecolor(lastcolor);
#endif
}

void baseio_pdebug(const char *format, ...) {
  char buffer[2048] = {0};
#if __LINUX__ /* { */
  const char *head = "\e[0;32;1m";
  const char *end = "\e[0m";
#elif __WINDOWS__ /* }{ */
  uint16_t lastcolor = 
    set_consolecolor(FOREGROUND_INTENSITY | FOREGROUND_GREEN);
  const char *head = "";
  const char *end = "";
#endif /* } */
  va_list argptr;
  va_start(argptr, format);
  vsnprintf(buffer, sizeof(buffer) - 1, format, argptr);
  va_end(argptr);
  std::cout<<head<<buffer<<end<<std::endl;
#if __WINDOWS__
  reset_consolecolor(lastcolor);
#endif
}
