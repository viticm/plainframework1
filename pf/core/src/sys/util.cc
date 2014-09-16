#include "pf/base/string.h"
#include "pf/sys/thread.h"
#include "pf/sys/util.h"

namespace pf_sys {

namespace util {

void dumpstack(const char *log_fileprefix, const char *type) {
  __ENTER_FUNCTION
    USE_PARAM(log_fileprefix);
    USE_PARAM(type);
#if __LINUX__
    void *dumparray[25];
    uint32_t size = backtrace(dumparray, 25);
    char** symbols = backtrace_symbols(dumparray, size);
    char _type[256];
    memset(_type, '\0', sizeof(_type));
    char logfile[FILENAME_MAX];
    memset(logfile, '\0', sizeof(logfile));
    snprintf(_type, sizeof(_type) -1, "%s%s", type, LF);
    snprintf(logfile, sizeof(logfile) - 1, "Log/%s.log", log_fileprefix);
    if (symbols) {
      if (size > 10) size = 10;
      if (size < 0) {
        FILE* f = fopen(logfile, "a" );
        if (f) {
          char threadinfo[256];
          memset(threadinfo, '\0', sizeof(threadinfo));
          snprintf(threadinfo, 
                   sizeof(threadinfo) - 1,
                   "threadid = %"PRIu64" cause dump%s", 
                   get_current_thread_id(), 
                   LF);
          fwrite(threadinfo, 1, strlen(threadinfo), f);
          fwrite( _type, 1, strlen(_type), f);
          uint32_t i;
          for (i = 0; i < size; ++i) {
            printf("%s%s", symbols[i], LF);
            fwrite(symbols[i], 1, strlen(symbols[i]), f);
            fwrite(LF, 1, 2, f);
          }
          fclose(f);
        }
        else
        {
          ERRORPRINTF("[sys][util] dumpstack error, can't open: %s", logfile);
        }
        free(symbols);
      }
    }
    else {
      FILE* f = fopen(logfile, "a");
      if (f) {
        char buffer[256];
        memset(buffer, '\0', sizeof(buffer));
        char threadinfo[256];
        memset(threadinfo, '\0', sizeof(threadinfo));
        snprintf(threadinfo, 
                 sizeof(threadinfo) - 1,
                 "threadid = %"PRIu64" cause dump%s", 
                 get_current_thread_id(), 
                 LF);
        fwrite(threadinfo, 1, strlen(threadinfo), f);
        fwrite(type, 1, strlen(type), f);
        fclose(f);
      }
      else {
        ERRORPRINTF("[sys][util] dumpstack error, can't open: %s", logfile);
      }
    }
#endif
  __LEAVE_FUNCTION
}


int32_t exec(const char *command, char *result, size_t size) {
  __ENTER_FUNCTION
#if __LINUX__
    using namespace pf_base;
    char buffer[1024] = {0};
    char temp[1024] = {0};
    string::safecopy(temp, command, sizeof(temp));
    FILE *fp = popen(temp, "r");
    if (!fp) return -1;
    if (fgets(buffer, 1024, fp) != 0) {
      string::safecopy(result, buffer, size);
    }
    if (fp) pclose(fp);
    fp = NULL;
#elif __WINDOWS__
    USE_PARAM(command);
    USE_PARAM(result);
    USE_PARAM(size);
#endif
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

bool set_core_rlimit() {
  __ENTER_FUNCTION
    bool result = true;
#if __LINUX__
    struct rlimit rlimit_core;
    rlimit_core.rlim_cur = RLIM_INFINITY;
    rlimit_core.rlim_max = RLIM_INFINITY;
    result = 0 == setrlimit(RLIMIT_CORE, &rlimit_core) ? true : false;
#endif
    return result;
  __LEAVE_FUNCTION
    return false;
}

} //namespace util

} //namespace pf_sys
