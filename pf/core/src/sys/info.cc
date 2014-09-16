#include "pf/base/string.h"
#if __LINUX__
#include <sys/times.h>
#include <sys/sysinfo.h>
#elif __WINDOWS__
#include <winsock.h>
#include <io.h>
#endif
#include "pf/sys/info.h"

namespace pf_sys {

namespace info {

#if __WINDOWS__
static uint64_t file_time_2_utc(const FILETIME *ftime) {
  LARGE_INTEGER li;
  Assert(ftime);
  li.LowPart = ftime->dwLowDateTime;
  li.HighPart = ftime->dwHighDateTime;
  return li.QuadPart;
}
#endif

bool get_sys_info(sys_info_t &sys_info) {
  __ENTER_FUNCTION
#if __LINUX__
    struct sysinfo info;
    if (-1 == sysinfo(&info)) return false;
    sys_info.uptime_second = info.uptime;
    sys_info.average_load[0] = info.loads[0];
    sys_info.average_load[1] = info.loads[1];
    sys_info.average_load[2] = info.loads[2];
    sys_info.ram_total = info.totalram;
    sys_info.ram_free = info.freeram;
    sys_info.ram_shared = info.sharedram;
    sys_info.ram_buffer = info.bufferram;
    sys_info.swap_total = info.totalswap;
    sys_info.swap_free = info.freeswap;
    sys_info.process_number = info.procs;
#elif __WINDOWS__
    MEMORYSTATUS mem_info;
    SYSTEM_INFO info;
    GlobalMemoryStatus(&mem_info); //memory
    GetSystemInfo(&info); //cpu
    sys_info.uptime_second = 0;
    sys_info.average_load[0] = 0;
    sys_info.average_load[1] = 0;
    sys_info.average_load[2] = 0;
    sys_info.ram_total = mem_info.dwTotalPhys;
    sys_info.ram_free = mem_info.dwAvailPhys;
    sys_info.ram_shared = mem_info.dwTotalVirtual;
    sys_info.ram_buffer = mem_info.dwAvailPageFile;
    sys_info.swap_total = mem_info.dwTotalVirtual;
    sys_info.swap_free = mem_info.dwAvailVirtual;
    sys_info.process_number = static_cast<uint16_t>(info.dwNumberOfProcessors);
#endif
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool get_mem_info(memory_info_t &mem_info) {
  __ENTER_FUNCTION
#if __LINUX__
    FILE* fp = fopen("/proc/meminfo", "r");
    if (NULL == fp) return false;
    CloseHelper ch(fp);
    int i = 0;
    int value;
    char name[LINE_MAX];
    char line[LINE_MAX];
    int filed_number = 2;
    int member_number = 7;

    while (fgets(line, sizeof(line)-1, fp)) {
      if (sscanf(line, "%s%u", name, &value) != filed_number)
        continue;

      if (0 == strcmp(name, "MemTotal:")) {
        ++i;
        mem_info.mem_total = value;
      }
      else if (0 == strcmp(name, "MemFree:")) {
        ++i;
        mem_info.mem_free = value;
      }
      else if (0 == strcmp(name, "Buffers:")) {
        ++i;
        mem_info.buffers = value;
      }
      else if (0 == strcmp(name, "Cached:")) {
        ++i;
        mem_info.cached = value;
      }
      else if (0 == strcmp(name, "SwapCached:")) {
        ++i;
        mem_info.swap_cached = value;
      }
      else if (0 == strcmp(name, "SwapTotal:")) {
        ++i;
        mem_info.swap_total = value;
      }
      else if (0 == strcmp(name, "SwapFree:")) {
        ++i;
        mem_info.swap_free = value;
      }
      if (i == member_number)
        break;
    }

    return (i == member_number);
#elif __WINDOWS__
    MEMORYSTATUS memory_status;
    GlobalMemoryStatus(&memory_status); //memory
    mem_info.mem_total = static_cast<uint32_t>(memory_status.dwTotalPhys);
    mem_info.mem_free = static_cast<uint32_t>(memory_status.dwAvailPhys);
    mem_info.buffers = static_cast<uint32_t>(memory_status.dwTotalPageFile);
    mem_info.cached = static_cast<uint32_t>(memory_status.dwAvailPageFile);
    mem_info.swap_total = static_cast<uint32_t>(memory_status.dwTotalVirtual);
    mem_info.swap_free = static_cast<uint32_t>(memory_status.dwAvailVirtual);
#endif
  __LEAVE_FUNCTION
    return false;
}

bool get_cpu_info(cpu_info_t &cpu_info) {
  __ENTER_FUNCTION
    using namespace pf_base;
#if __LINUX__
    FILE* fp = fopen("/proc/stat", "r");
    if (NULL == fp) return false;
    CloseHelper ch(fp);

    char name[LINE_MAX];
    char line[LINE_MAX];
    int filed_number = 8;

    while (fgets(line, sizeof(line) - 1, fp)) {
      if (sscanf(line, 
                 "%s%u%u%u%u%u%u%u", 
                 name, 
                 &cpu_info.user, 
                 &cpu_info.nice, 
                 &cpu_info.system, 
                 &cpu_info.idle, 
                 &cpu_info.iowait, 
                 &cpu_info.irq, 
                 &cpu_info.softirq) != filed_number)
        continue;
      
      if (0 == strcmp(name, "cpu")) {
        cpu_info.total = cpu_info.user + 
                         cpu_info.nice + 
                         cpu_info.system + 
                         cpu_info.idle + 
                         cpu_info.iowait + 
                         cpu_info.irq + 
                         cpu_info.softirq;
        break;
      }
      name[0] = '\0';    
    }
    memset(cpu_info.cpu_name, 0, sizeof(cpu_info.cpu_name)); //init
    FILE *fp1 = fopen("/proc/cpuinfo", "r");
    if (fp1) {
      CloseHelper ch1(fp1);
      while (fgets(line, sizeof(line) - 1, fp1)) {
        const char *kCpuModelName = "model name      : %s";
        int ret = sscanf(line, kCpuModelName, &cpu_info.cpu_name);
        if (ret > 0) {
          std::string full_model_name = line;
          string::replace_all(full_model_name, "model name      : ", "");
          string::safecopy(cpu_info.cpu_name,  
                           full_model_name.c_str(),
                           sizeof(cpu_info.cpu_name));
          break;
        }
      }
    }
    return (name[0] != '\0');
#elif __WINDOWS__
    FILETIME idle_time;
    FILETIME system_time;
    FILETIME user_time;
    GetSystemTimes(&idle_time, &system_time, &user_time);
    cpu_info.idle = static_cast<uint32_t>(file_time_2_utc(&idle_time));
    cpu_info.system = static_cast<uint32_t>(file_time_2_utc(&system_time));
    cpu_info.user = static_cast<uint32_t>(file_time_2_utc(&user_time));
    cpu_info.nice = 0;
    cpu_info.iowait = 0;
    cpu_info.irq = 0;
    cpu_info.softirq = 0;
    cpu_info.total = cpu_info.idle + cpu_info.system + cpu_info.user;

    /**
    char cpu_name[CPU_NAME_MAX];
    int result;
    HKEY  h_key = NULL;
    DWORD type = 0;
    DWORD size = 0;

    result = RegOpenKeyEx(CPU_REG_KEY, CPU_REG_SUBKEY, 0, KEY_READ, &key);
    Assert(S_OK == result);
    result = RegQueryValueEx(key, CPU_NAME, NULL, &type, NULL, &size);
    Assert(size > 0);
    result = RegQueryValueEx(key, 
                             CPU_NAME, 
                             NULL, 
                             &type, 
                             static_cast<LPBYTE>(cpu_name), 
                             &size);
    Assert(S_OK == result);
    **/

    return true;
#endif
  __LEAVE_FUNCTION
    return false;
}
/**
int get_cpu_info_array(std::vector<cpu_info_t>& cpu_info_array) {
  __ENTER_FUNCTION
#if __LINUX__
    cpu_info_array.clear();
    FILE* fp = fopen("/proc/stat", "r");
    if (NULL == fp) return 0;
    sys::CloseHelper<FILE*> ch(fp);

    char name[LINE_MAX];
    char line[LINE_MAX];
    int filed_number = 8;
    
    while (fgets(line, sizeof(line)-1, fp)) {
      cpu_info_t cpu_info;
      if (sscanf(line, 
                "%s%u%u%u%u%u%u%u", 
                name, 
                &cpu_info.user, 
                &cpu_info.nice, 
                &cpu_info.system, 
                &cpu_info.idle, 
                &cpu_info.iowait, 
                &cpu_info.irq, 
                &cpu_info.softirq) != filed_number)
        continue;

      if (strncmp(name, "cpu", 3) != 0)
        break;
      cpu_info.total = cpu_info.user + 
                       cpu_info.nice + 
                       cpu_info.system + 
                       cpu_info.idle + 
                       cpu_info.iowait + 
                       cpu_info.irq + 
                       cpu_info.softirq;
      //cpu name
      cpu_info.cpu_name = {0}; //init
      FILE* fp = fopen("/proc/cpuinfo", "r");
      if (fp) {
        CloseHelper<FILE*>(fp);
        while (fgets(line, sizeof(line) - 1, fp)) {
          const char *kCpuModelName = "model name      : %s";
          int ret = sscanf(line, kCpuModelName, &cpu_info.cpu_name);
          if (ret > 0) {
            std::string full_model_name = line;
            pap_common_base::string::replace_all(full_model_name, "model name      : ", "");
            cpu_info.cpu_name = full_model_name.c_str();
            break;
          }
        }
      }
      cpu_info_array.push_back(cpu_info);
    }
    return cpu_info_array.size();
#elif __WINDOWS__
    GetSystemTimes(&cpu_info.idle, &cpu_info.system, &cpu_info.user);
    cpu_info.nice = 0;
    cpu_info.iowait = 0;
    cpu_info.irq = 0;
    cpu_info.softirq = 0;
    cpu_info.total = cpu_info.idle + cpu_info.system + cpu_info.user;

    char cpu_name[CPU_NAME_MAX];
    int result;
    HKEY  h_key = NULL;
    DWORD type = 0;
    DWORD size = 0;

    result = RegOpenKeyEx(CPU_REG_KEY, CPU_REG_SUBKEY, 0, KEY_READ, &key);
    Assert(S_OK == result);
    result = RegQueryValueEx(key, CPU_NAME, NULL, &type, NULL, &size);
    Assert(size > 0);
    result = RegQueryValueEx(key, CPU_NAME, NULL, &type, static_cast<LPBYTE>(cpu_name), &size);
    Assert(S_OK == result);
    cpu_info_array.push_back(cpu_info);
    return cpu_info_array.size();
#endif
  __LEAVE_FUNCTION
    return false;
}
**/

bool get_kernel_version(kernel_version_t &kernel_version) {
  __ENTER_FUNCTION
    using namespace pf_base;
#if __LINUX__
    FILE* fp = fopen("/proc/version", "r");
    if (NULL == fp) return false;
    CloseHelper ch(fp);
    char line[LINE_MAX];
    char *linep = fgets(line, sizeof(line)-1, fp);

    if (NULL == linep) return false;

    char f1[LINE_MAX];
    char f2[LINE_MAX];
    char version[LINE_MAX];
    // Linux version 2.6.24-22-generic (buildd@crested)
    if (sscanf(line, "%s%s%s", f1, f2, version) != 3)
      return false;
    // system name
    string::safecopy(kernel_version.system_name, 
                     f1, 
                     sizeof(kernel_version.system_name));

    char *bar = strchr(version, '-');
    if (bar != NULL) *bar = '\0';

    char *dot1 = strchr(version, '.');
    if (NULL == dot1) return false;
    *dot1++ = '\0';
    if (!string::toint16(version, kernel_version.major)) return false;

    char *dot2 = strchr(dot1, '.');
    if (NULL == dot2) return false;
    *dot2++ = '\0';
    if (!string::toint16(dot1, kernel_version.minor)) return false;

    return string::toint16(dot2, kernel_version.revision);
#elif __WINDOWS__
    int result = false;
    OSVERSIONINFOEX os_vision;
    BOOL os_version_info_ex = false;
    std::string version_str;

    ZeroMemory(&os_vision, sizeof(OSVERSIONINFOEX));
    os_vision.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    os_version_info_ex = GetVersionEx((OSVERSIONINFO *)&os_vision);
    if (!os_version_info_ex) {
      os_vision.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      result = GetVersionEx((OSVERSIONINFO*) &os_vision);
      if (!result) return false;
    }
    switch (os_vision.dwPlatformId) {
      case VER_PLATFORM_WIN32_NT: {
        if (os_vision.dwMajorVersion <= 4)
          version_str = "Microsoft Windows NT ";
        if (5 == os_vision.dwMajorVersion && 0 ==  os_vision.dwMinorVersion)
          version_str = "Microsoft Windows 2000 ";
        if (5 == os_vision.dwMajorVersion && 1 == os_vision.dwMinorVersion)
          version_str = "Microsoft Windows XP ";
        if (5 == os_vision.dwMajorVersion && 2 == os_vision.dwMinorVersion)
          if (::GetSystemMetrics(SM_SERVERR2) != 0)
            version_str = "Microsoft Windows 2003 RC2 ";
          else
            version_str = "Microsoft Windows 2003 ";
        if (6 == os_vision.dwMajorVersion) {
          if(0 == os_vision.dwMinorVersion) {
            if (os_vision.wProductType == VER_NT_WORKSTATION)
              version_str = "Microsoft Windows Vista";
            else if (os_vision.wProductType != VER_NT_WORKSTATION)
              version_str = "Microsoft Windows 2008";
          }
          else if (1 == os_vision.dwMinorVersion) {
              version_str = "Windows 7";
          }
        }
        if (os_version_info_ex) {
          if (VER_NT_SERVER == os_vision.wProductType) {
            if( os_vision.wSuiteMask & VER_SUITE_DATACENTER )
              version_str += "DataCenter Server ";
            else if( os_vision.wSuiteMask & VER_SUITE_ENTERPRISE )
              version_str += "Advanced Server ";
            else
              version_str += "Server ";
          }
        }
        else {
          HKEY key = NULL;
          char product_type[80];
          DWORD buf_len = 0;
          ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                         "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
                         0, 
                         KEY_QUERY_VALUE, 
                         &key);
          ::RegQueryValueEx(key, 
                            "ProductType", 
                            NULL, 
                            NULL,
                            (LPBYTE)product_type, 
                            &buf_len);

          RegCloseKey(key);
          key = NULL;
          if (0 == lstrcmpi( "WINNT", product_type))
            version_str += "Professional ";
          if (0 == lstrcmpi( "LANMANNT", product_type))
            version_str += "Server ";
          if (0 == lstrcmpi( "SERVERNT", product_type))
            version_str += "Advanced Server ";
        }
        if (os_vision.dwMajorVersion <= 4) {
          char temp[FILENAME_MAX] = {0};
          snprintf(temp,
                   sizeof(temp) - 1,
                   "%s version %d.%d %s (Build %u)\n",
                   version_str.c_str(), 
                   os_vision.dwMajorVersion,
                   os_vision.dwMinorVersion,
                   os_vision.szCSDVersion,
                   os_vision.dwBuildNumber & 0xFFFF);
          version_str = temp;
        } else { 
          char temp[FILENAME_MAX] = {0};
          snprintf(temp,
                   sizeof(temp) - 1,
                   "%s %s (Build %u)\n",
                   version_str.c_str(), 
                   os_vision.szCSDVersion,
                   os_vision.dwBuildNumber & 0xFFFF);
          version_str = temp;
        }
        break;
      } case VER_PLATFORM_WIN32_WINDOWS: {

        if (4 == os_vision.dwMajorVersion && 0 == os_vision.dwMinorVersion) {
          version_str = "Microsoft Windows 95 ";
          if (os_vision.szCSDVersion[1] == 'C' || 
              os_vision.szCSDVersion[1] == 'B')
            version_str += "OSR2 ";
        } 

        if (4 == os_vision.dwMajorVersion && 10 == os_vision.dwMinorVersion) {
          version_str = "Microsoft Windows 98 ";
          if ( os_vision.szCSDVersion[1] == 'A' )
            version_str += "SE ";
        } 

        if (4 == os_vision.dwMajorVersion && 90 == os_vision.dwMinorVersion) {
          version_str = "Microsoft Windows Me ";
        } 
        break;
      } case VER_PLATFORM_WIN32s: {
        version_str = "Microsoft Win32s ";
        break;
      }
      default: {
        version_str = "UnKnow OS";
        break;
      }
    }
    kernel_version.minor = static_cast<int16_t>(os_vision.dwMinorVersion);
    kernel_version.major = static_cast<int16_t>(os_vision.dwMajorVersion);
    kernel_version.revision = 0;
    string::safecopy(kernel_version.system_name, 
                     version_str.c_str(), 
                     sizeof(kernel_version.system_name));
    return true;
#endif
  __LEAVE_FUNCTION
    return false;
}

bool get_process_info(process_info_t &process_info) {
  USE_PARAMEX(process_info);
  /**
  __ENTER_FUNCTION
    char filename[FILENAME_MAX];
    snprintf(filename, sizeof(filename)-1, "/proc/%u/stat", getpid());

    FILE* fp = fopen(filename, "r");
    if (NULL == fp) return false;
    CloseHelper<FILE*> ch(fp);

    char line[LINE_MAX];
    int filed_number = 38;
      char *linep = fgets(line, sizeof(line)-1, fp);

    if (NULL == linep) return false;
    
    return (sscanf(line, "%d%s%s%d%d"
               "%d%d%d%u%lu"
               "%lu%lu%lu%lu%lu"
               "%ld%ld%ld%ld%ld"
               "%ld%lld%lu%ld%lu"
               "%lu%lu%lu%lu%lu"
               "%lu%lu%lu%lu%lu"
               "%lu%d%d",
               &process_info.pid, 
               process_info.comm,
               &process_info.state,
               &process_info.ppid,
               &process_info.pgrp,
               &process_info.session,
               &process_info.tty_nr,
               &process_info.tpgid,
               &process_info.flags,
               &process_info.minflt,
               &process_info.cminflt,
               &process_info.majflt,
               &process_info.cmajflt,
               &process_info.utime,
               &process_info.stime,
               &process_info.cutime,
               &process_info.cstime,
               &process_info.priority,
               &process_info.nice,
               &process_info.num_threads,
               &process_info.itrealvalue,
               &process_info.starttime,
               &process_info.vsize,
               &process_info.rss,
               &process_info.rlim,
               &process_info.startcode,
               &process_info.endcode,
               &process_info.startstack,
               &process_info.kstkesp,
               &process_info.kstkeip,
               &process_info.signal,
               &process_info.blocked,
               &process_info.sigignore,
               &process_info.sigcatch,
               &process_info.nswap,
               &process_info.cnswap,
               &process_info.exit_signal,
               &process_info.processor) == filed_number);
  __LEAVE_FUNCTION
    return false;
  **/
    return true;
}

bool get_process_page_info(process_page_info_t &process_page_info) {
  __ENTER_FUNCTION
#if __LINUX__
    char filename[FILENAME_MAX];
    snprintf(filename, sizeof(filename)-1, "/proc/%u/statm", getpid());

    FILE* fp = fopen(filename, "r");
    if (NULL == fp) return false;
    CloseHelper ch(fp);

    char line[LINE_MAX];
    int filed_number = 6;
    char *linep = fgets(line, sizeof(line)-1, fp);

    if (NULL == linep) return false;

    return (sscanf(line,
                   "%ld%ld%ld%ld%ld%ld",
                   &process_page_info.size,
                   &process_page_info.resident,
                   &process_page_info.share,
                   &process_page_info.text,
                   &process_page_info.lib,
                   &process_page_info.data) == filed_number);
#elif __WINDOWS__
    USE_PARAMEX(process_page_info);
    return true;
#endif
  __LEAVE_FUNCTION
    return false;
}

// getrusage

bool get_process_times(process_time_t &process_time) {
  __ENTER_FUNCTION
#if __LINUX__
    struct tms buf;
    if (-1 == times(&buf)) return false;
    
    process_time.user_time = buf.tms_utime;
    process_time.system_time = buf.tms_stime;
    process_time.user_time_child = buf.tms_cutime;
    process_time.system_time_child = buf.tms_cstime;
    
    return true;
#elif __WINDOWS__
    USE_PARAMEX(process_time);
    return true;
#endif
  __LEAVE_FUNCTION
    return false;
}

bool do_get_net_info_array(const char *interface_name, 
                           std::vector<net_info_t> &net_info_array) {
  __ENTER_FUNCTION
#if __LINUX__
    net_info_array.clear();
    FILE* fp = fopen("/proc/net/dev", "r");
    if (NULL == fp) return false;
    CloseHelper ch(fp);

    char line[LINE_MAX];
    int filed_number = 17;

    // 跳过头两行
    if (NULL == fgets(line, sizeof(line)-1, fp)) return false;
    if (NULL == fgets(line, sizeof(line)-1, fp)) return false;

    while (fgets(line, sizeof(line)-1, fp)) {
      char *line_p = line;
      // 去掉前导空格
      while ((' ' == *line_p) || ('\t' == *line_p))
        ++line_p;
      
      char *colon = strchr(line_p, ':');
      if (NULL == colon) break;
      *colon = '\t';

      if ((interface_name != NULL)
       && (strncmp(line_p, interface_name, strlen(interface_name)) != 0))
        continue;
      
      net_info_t net_info;
      if (sscanf(line_p,
              "%s"
               "%lu%lu%lu%lu%lu"
               "%lu%lu%lu%lu%lu"
               "%lu%lu%lu%lu%lu"
               "%lu", 
               net_info.interface_name,
               &net_info.receive_bytes,
               &net_info.receive_packets,
               &net_info.receive_errors,
               &net_info.receive_dropped,
               &net_info.receive_fifo_errors,
               &net_info.receive_frame,
               &net_info.receive_compressed,
               &net_info.receive_multicast,
               &net_info.transmit_bytes,
               &net_info.transmit_packets,
               &net_info.transmit_errors,
               &net_info.transmit_dropped,
               &net_info.transmit_fifo_errors,
               &net_info.transmit_collisions,
               &net_info.transmit_carrier,
               &net_info.transmit_compressed) != filed_number)
        break;

      net_info_array.push_back(net_info);
      return true;
    }
    return false;
#elif __WINDOWS__
    USE_PARAM(interface_name);
    USE_PARAMEX(net_info_array);
    return true;
#endif
  __LEAVE_FUNCTION
    return false;
}

bool get_net_info(const char *interface_name, net_info_t &net_info) {
  __ENTER_FUNCTION
#if __LINUX__
    std::vector<net_info_t> net_info_array;
    if (do_get_net_info_array(interface_name, net_info_array)) {
      memcpy(&net_info, &net_info_array[0], sizeof(net_info));
      return true;
    }
    return false;
#elif (__WINDOWS__)
    USE_PARAM(interface_name);
    USE_PARAMEX(net_info);
    return true;
#endif
  __LEAVE_FUNCTION
    return false;
}

bool get_net_info_array(std::vector<net_info_t> &net_info_array) {
  __ENTER_FUNCTION
#if __LINUX__
    return do_get_net_info_array(NULL, net_info_array);
#elif __WINDOWS__
    USE_PARAMEX(net_info_array);
    return true;
#endif
  __LEAVE_FUNCTION
    return false;
}

bool get_ip(char *&ip, const char *interface_name) {
  __ENTER_FUNCTION
    using namespace pf_base;
    bool result = false;
#if __LINUX__
    char line[LINE_MAX] = {0};
    const char *kNetworkScriptFileNameStr 
      = "/etc/sysconfig/network-scripts/ifcfg-%s";
    char network_script_file_name[FILENAME_MAX] = {0};
    snprintf(network_script_file_name, 
             sizeof(network_script_file_name), 
             kNetworkScriptFileNameStr, 
             interface_name);
    FILE* fp = fopen(network_script_file_name, "r");
    if (fp) {
      CloseHelper ch(fp);
      while (fgets(line, sizeof(line) - 1, fp)) {
        const char *kIpAddr = "IPADDR=%s";
        int ret = sscanf(line, kIpAddr, &ip);
        if (ret > 0) {
          std::string full_ipaddr = line;
          string::replace_all(full_ipaddr, "IPADDR=", "");
          string::safecopy(ip, full_ipaddr.c_str(), IP_SIZE);
          break;
        }
      }
    }
#elif __WINDOWS__
    USE_PARAM(interface_name);
    WSADATA wsa_data;
    char name[FILENAME_MAX];
    PHOSTENT hostinfo = 0; 
    if (0 == WSAStartup(MAKEWORD(2,2), &wsa_data)) { 
      if (0 == gethostname(name, sizeof(name))) { 
        if((hostinfo = gethostbyname(name)) != NULL) { 
           ip = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list); 
        }
      }
    }
    WSACleanup();
#endif
    result = strlen(ip) > 0;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool get_loadaverage(loadaverage_t &loadaverage) {
  __ENTER_FUNCTION
#if __LINUX__
    char line[LINE_MAX] = {0};
    const char *kFileName = "/proc/loadavg";
    FILE *fp = fopen(kFileName, "r");
    if (!fp) return false;
    char *linep = fgets(line, sizeof(line) - 1, fp);
    if (NULL == linep) return false;
    if (fp) fclose(fp);
    fp = NULL;
    if (sscanf(line,
               "%f %f %f", 
               &loadaverage.oneminutes, 
               &loadaverage.fiveminutes, 
               &loadaverage.fifteenminutes) != 3) {
      DEBUGPRINTF("sscanf error");
      return false;
    }
#elif __WINDOWS__
    USE_PARAMEX(loadaverage);
#endif
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace info

} //namespace pf_sys
