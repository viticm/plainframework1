#include "pf/engine/kernel.h"
#include "main.h"

pf_engine::Kernel g_engine_kernel;
int32_t main(int32_t argc, char * argv[]) {
#if __WINDOWS__
  USE_PARAM(argc);
  USE_PARAM(argv);
  _CrtSetDbgFlag(_CrtSetDbgFlag(0) | _CRTDBG_LEAK_CHECK_DF);
  system("color 02"); //color green
  system("mode con cols=120"); //cmd size
  WORD versionrequested;
  WSADATA data;
  int32_t error;
  versionrequested = MAKEWORD(2, 2);
  error = WSAStartup(versionrequested, &data);
#endif
  //g_engine_kernel.setconfig(ENGINE_CONFIG_NET_ISACTIVE, true);
  g_engine_kernel.init();
  g_engine_kernel.run();
  g_engine_kernel.stop();
  return 0;
}
