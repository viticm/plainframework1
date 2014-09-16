#include "pf/sys/minidump.h"
#include "pf/sys/thread.h"

#if __WINDOWS__
#include <stdio.h>
#include <stdlib.h>
#include <Dbghelp.h>
#pragma comment( lib, "DbgHelp" )
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

namespace pf_sys {

namespace minidump {

#if __WINDOWS__
LONG WINAPI unhandled_exceptionfilter(
    struct _EXCEPTION_POINTERS* exceptioninfo)
{
	char hostname[FILENAME_MAX] = {0};
	gethostname(hostname, sizeof(hostname) - 1);
	char crashfile[FILENAME_MAX] = {0};
	SYSTEMTIME nowtime;
	::GetLocalTime(&nowtime);
	snprintf(crashfile, 
           sizeof(crashfile) - 1, 
           "%s_crash_%s_%02d%02d%02d%02d.dmp", 
		       "filename", 
           hostname, 
           nowtime.wMonth, 
           nowtime.wDay, 
           nowtime.wHour, 
           nowtime.wMinute,
           nowtime.wSecond);

	HANDLE createfile_heandle = CreateFile(crashfile, 
                                         GENERIC_WRITE, 
                                         0, 
                                         NULL, 
                                         CREATE_ALWAYS,
                                         FILE_ATTRIBUTE_NORMAL,
                                         NULL);
	if (createfile_heandle)
	{
		MINIDUMP_EXCEPTION_INFORMATION _exceptioninfo;
		_exceptioninfo.ExceptionPointers = exceptioninfo;
		_exceptioninfo.ThreadId = static_cast<DWORD>(get_current_thread_id());
		_exceptioninfo.ClientPointers = true;
		MiniDumpWriteDump(GetCurrentProcess(), 
                      GetCurrentProcessId(),
                      createfile_heandle, 
                      MiniDumpNormal, 
                      &_exceptioninfo, 
                      NULL, 
                      NULL);
		CloseHandle(createfile_heandle);

		char command[FILENAME_MAX] = {0};
		PathAppend(command, "upload_dump.bat ");
		strcat(command, crashfile);
		system(command);	
	}
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif 

} //namespace minidump

} //namespace pf_sys
