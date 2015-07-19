#include "pf/base/util.h"
#include "pf/sys/util.h"
#include "pf/sys/process.h"
#if __WINDOWS__
#include <process.h>
#include <psapi.h>
#elif __LINUX__
#include <unistd.h>
#include <sys/stat.h>
#endif

namespace pf_sys {

namespace process {

int32_t getid() {
  __ENTER_FUNCTION
    int32_t id = ID_INVALID;
#if __WINDOWS__
    id = _getpid();
#elif __LINUX__
    id = getpid();
#endif
    return id;
  __LEAVE_FUNCTION
    return ID_INVALID;
}

void get_filename(char *filename, size_t size) {
  __ENTER_FUNCTION
    using namespace pf_base::util;
    get_module_filename(filename, size);
    int32_t havelength = static_cast<int32_t>(strlen(filename));
    int32_t _size = static_cast<int32_t>(size) - havelength;
    if (_size > 0) snprintf(filename + havelength, _size, "%s", ".pid");
  __LEAVE_FUNCTION
}

int32_t getid(const char *filename) {
  __ENTER_FUNCTION
    FILE *fp = NULL;
    fp = fopen(filename, "r");
    if (NULL == fp) return ID_INVALID;
    int32_t id;
    fscanf(fp, "%d", &id);
    fclose(fp);
    fp = NULL;
    return id;
  __LEAVE_FUNCTION
    return ID_INVALID;
}

bool writeid(const char *filename) {
  __ENTER_FUNCTION
    int32_t id = getid();
    FILE *fp = fopen(filename, "w");
    if (NULL == fp) return false;
    fprintf(fp, "%d", id);
    fflush(fp);
    fclose(fp);
    fp = NULL;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool waitexit(const char *filename) {
  __ENTER_FUNCTION
    if (NULL == filename || strlen(filename) < 0) {
      ERRORPRINTF("[sys] (process::waitexit) error, can't find pid file");
      return false;
    }
    int32_t id = getid(filename);
    if (ID_INVALID == id) {
      ERRORPRINTF("[sys] (process::waitexit) error, can't get id from file: %s",
                  filename);
      return false;
    }
#if __LINUX__
    kill(id, 10);
    pf_base::util::sleep(2000);
    kill(id, 10);
    int32_t result = kill(id, 0);
    while (result >= 0) {
      pf_base::util::sleep(1000);
      result = kill(id, 0);
    }
    DEBUGPRINTF("[sys] (process::waitexit) success, pid(%d), result(%d)",
                id,
                result);
#endif
    return true;
  __LEAVE_FUNCTION
    return false;
}

void getinfo(int32_t id, info_t &info) {
  __ENTER_FUNCTION
    info.id = id;
    info.cpu_percent = get_cpu_usage(id);
    info.VSZ = get_virtualmemory_usage(id);
    info.RSS = get_physicalmemory_usage(id);
  __LEAVE_FUNCTION
}

#if __WINDOWS__
static uint64_t file_time_2_utc(const FILETIME* ftime) {
  LARGE_INTEGER li;
  Assert(ftime);
  li.LowPart = ftime->dwLowDateTime;
  li.HighPart = ftime->dwHighDateTime;
  return li.QuadPart;
}

static int32_t get_processor_number() {
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  int32_t result = static_cast<int32_t>(info.dwNumberOfProcessors);
  return result;
}
#endif 

float get_cpu_usage(int32_t id) {
  __ENTER_FUNCTION
  float cpu = -1.0f;
#if __WINDOWS__ /* { */
  static int processor_count = -1;
  static int64_t last_time = 0;
  static int64_t last_system_time = 0;
  FILETIME now;
  FILETIME creation_time;
  FILETIME exit_time;
  FILETIME kernel_time;
  FILETIME user_time;
  int64_t system_time;
  int64_t time;
  int64_t system_time_delta;
  int64_t time_delta;
  if (-1 == processor_count) {
    processor_count = get_processor_number();
  }
  GetSystemTimeAsFileTime(&now);
  HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
  if (!::GetProcessTimes(hProcess, 
                         &creation_time, 
                         &exit_time,
                         &kernel_time, 
                         &user_time)) {
    return -1.0f;
  }
  system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) / 
    processor_count;
  time = file_time_2_utc(&now);
  if ((0 == last_system_time) || (0 == last_time)) {
    last_system_time = system_time;
    last_time = time;
    return -1.0f;
  }
  system_time_delta = system_time - last_system_time;
  time_delta = time - last_time;
  if (time_delta == 0) return -1.0f;
  cpu = static_cast<float>(
    (system_time_delta * 100 + time_delta / 2) / time_delta);
  last_system_time = system_time;
  last_time = time;
#elif __LINUX__ /* } { */
  char temp[32] = {0};
  char command[128] = {0};
  snprintf(command, 
           sizeof(command) - 1, 
           "ps aux | awk '{if ($2 == %d) print $3}'",
           id);
  if (0 == util::exec(command, temp, sizeof(temp))) {
    cpu = atof(temp);
  }
#endif /* } */
    return cpu;
  __LEAVE_FUNCTION
    return -1.0f;
}

uint64_t get_virtualmemory_usage(int32_t id) {
  __ENTER_FUNCTION
    uint64_t result = 0;
#if __WINDOWS__ /* { */
    PROCESS_MEMORY_COUNTERS pmc;
    HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
    if (::GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
      result = pmc.PagefileUsage;
    }
#elif __LINUX__ /* }{ */
  char temp[128] = {0};
  char command[128] = {0};
  snprintf(command, 
           sizeof(command) - 1, 
           "ps aux | awk '{if ($2 == %d) print $5}'",
           id);
  if (0 == util::exec(command, temp, sizeof(temp))) {
    char *endpointer = NULL;
    result = strtouint64(temp, &endpointer, 10);
    result *= 1024;
  }
#endif /* } */
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t get_physicalmemory_usage(int32_t id) {
  __ENTER_FUNCTION
    uint64_t result = 0;
#if __WINDOWS__ /* { */
    PROCESS_MEMORY_COUNTERS pmc;
    HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
    if (::GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
      result = pmc.WorkingSetSize;
    }
#elif __LINUX__ /* }{ */
  char temp[128] = {0};
  char command[128] = {0};
  snprintf(command, 
           sizeof(command) - 1, 
           "ps aux | awk '{if ($2 == %d) print $6}'",
           id);
  if (0 == util::exec(command, temp, sizeof(temp))) {
    char *endpointer = NULL;
    result = strtouint64(temp, &endpointer, 10);
    result *= 1024;
  }
#endif /* } */
    return result;
  __LEAVE_FUNCTION
    return 0;
}

bool daemon() {
  __ENTER_FUNCTION
#if __LINUX__
    pid_t pid;
    if ((pid = fork()) != 0) exit(0);
    setsid();
    signal(SIGHUP, SIG_IGN);
    if ((pid = fork()) != 0) exit(0);
    umask(0);
    for (uint8_t i = 0; i < 3; i++) close(i);
    return true;
#endif
  __LEAVE_FUNCTION
    return false;
}

} //namespace process

} //namespace pf_sys
