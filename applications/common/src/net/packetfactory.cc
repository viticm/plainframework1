#include "pf/base/log.h"
#include "pf/net/packet/factorymanager.h"
#include "common/define/net/packet/id/all.h"

/* packets { */
#include "common/net/packet/serverserver/connect.h"
#if defined(_GATEWAY) || defined(_LOGIN)
#include "common/net/packet/gateway_tologin/resultauth.h"
#include "common/net/packet/login_togateway/askauth.h"
#include "common/net/packet/login_togateway/playeronline.h"
#endif
/* } packets */

#include "common/net/packetfactory.h"

namespace common {

namespace net {

uint16_t get_facctorysize() {
    using namespace common::define::net::packet::id;
    uint16_t size = 0;
    //between server and server
#if defined(_GATEWAY) || \
    defined(_LOGIN) || \
    defined(_CENTER) || \
    defined(_SERVER)
    size = serverserver::kLast - serverserver::kFirst; //common for server
#endif

    //between gateway and login
#if defined(_GATEWAY) || defined(_LOGIN)
    size += gatewaylogin::kLast - gatewaylogin::kFirst - 1;
    size += gateway_tologin::kLast - gateway_tologin::kFirst - 1;
    size += login_togateway::kLast - login_togateway::kFirst - 1;
#endif
    //between login and client
#if defined(_LOGIN) || defined(_CLIENT)
    size += clientlogin::kLast - clientlogin::kFirst - 1;
    size += client_tologin::kLast - client_tologin::kFirst - 1;
    size += login_toclient::kLast - login_toclient::kFirst - 1;
#endif
    //between login and center
#if defined(_LOGIN) || defined(_CENTER)
    size += logincenter::kLast - logincenter::kFirst - 1;
    size += login_tocenter::kLast - login_tocenter::kFirst - 1;
    size += center_tologin::kLast - center_tologin::kFirst - 1;
#endif
    //between server and center
#if defined(_SERVER) || defined(_CENTER)
    size += servercenter::kLast - servercenter::kFirst - 1;
    size += server_tocenter::kLast - server_tocenter::kFirst - 1;
    size += center_toserver::kLast - center_toserver::kFirst - 1;
#endif
    //between client and server
#if defined(_CLIENT) || defined(_SERVER)
    size += clientserver::kLast - clientserver::kFirst - 1;
    size += client_toserver::kLast - client_toserver::kFirst - 1;
    size += server_tocenter::kLast - server_tocenter::kFirst - 1;
#endif
    return size;
}

bool __stdcall registerfactories() {
  __ENTER_FUNCTION
    using namespace common::net::packet;
    SLOW_LOG(NET_MODULENAME, "[common.net] (registerfactories) start");
    if (!NET_PACKET_FACTORYMANAGER_POINTER) return false;
#if defined(_GATEWAY) || \
  defined(_LOGIN) || \
  defined(_CENTER) || \
  defined(_SERVER)
    NET_PACKET_FACTORYMANAGER_POINTER->addfactory(
        new serverserver::ConnectFactory());
#endif

#if defined(_GATEWAY) || defined(_LOGIN) /* { */
    NET_PACKET_FACTORYMANAGER_POINTER->addfactory(
        new gateway_tologin::ResultAuthFactory());
    NET_PACKET_FACTORYMANAGER_POINTER->addfactory(
        new login_togateway::AskAuthFactory());
    NET_PACKET_FACTORYMANAGER_POINTER->addfactory(
        new login_togateway::PlayerOnlineFactory());    
#endif /* } */

#if defined(_LOGIN) || defined(_CLIENT) /* { */

#endif /* } */

#if defined(_LOGIN) || defined(_PAP_NET_CENTER) /* { */

#endif /* } */

#if defined(_SERVER) || defined(_CENTER) /* { */

#endif /* } */

#if defined(_CLIENT) || defined(_SERVER) /* { */

#endif /* } */
    SLOW_LOG(NET_MODULENAME, "[common.net] (registerfactories) end");
    return true; //没有返回，这是我以为内存泄露的根源，小错误造成的后果真的难以想象
  __LEAVE_FUNCTION
    return false;
}

bool __stdcall isvalid_packetid(uint16_t id) {
  __ENTER_FUNCTION
    bool result = true;
    using namespace common::define::net::packet::id;
#if defined(_GATEWAY) /* { */
    result = (serverserver::kFirst < id && id < serverserver::kLast) ||  
             (gatewaylogin::kFirst < id && id < gatewaylogin::kLast) ||
             (gateway_tologin::kFirst < id && id < gateway_tologin::kLast) || 
             (login_togateway::kFirst < id && id < login_togateway::kLast);
#elif defined(_LOGIN) /* }{ */
    result = (serverserver::kFirst < id && id < serverserver::kLast) ||
             (gatewaylogin::kFirst < id && id < gatewaylogin::kLast) ||
             (gateway_tologin::kFirst < id && id < gateway_tologin::kLast) ||
             (login_togateway::kFirst < id && id < login_togateway::kLast) ||
             (clientlogin::kFirst < id && id < clientlogin::kLast) ||
             (client_tologin::kFirst < id && id < client_tologin::kLast) ||
             (login_tocenter::kFirst < id && id < login_toclient::kLast) ||
             (logincenter::kFirst < id && id < logincenter::kLast) ||
             (login_tocenter::kFirst < id && id < login_tocenter::kLast) || 
             (center_tologin::kFirst < id && id < center_tologin::kLast);
#elif defined(_CENTER) /* }{ */
    result = (serverserver::kFirst < id && id < serverserver::kLast) ||
             (logincenter::kFirst < id && id < logincenter::kLast) ||
             (login_tocenter::kFirst < id && id < login_tocenter::kLast) ||
             (center_tologin::kFirst < id && id < center_tologin::kLast) ||
             (servercenter::kFirst < id && id < servercenter::kLast) ||
             (server_tocenter::kFirst < id && id < server_tocenter::kLast) ||
             (center_toserver::kFirst < id && id < center_toserver::kLast);
#elif defined(_SERVER) /* }{ */
    result = (serverserver::kFirst < id && id < serverserver::kLast) ||
             (clientserver::kFirst < id && id < clientserver::kLast) ||
             (client_toserver::kFirst < id && id < client_toserver::kLast) ||
             (server_toclient::kFirst < id && id < server_toclient::kLast) ||
             (servercenter::kFirst < id && id < servercenter::kLast) ||
             (server_tocenter::kFirst < id && id < server_tocenter::kLast) ||
             (center_toserver::kFirst < id && id < center_toserver::kLast);
#elif defined(_CLIENT) /* }{ */
    result = (clientlogin::kFirst < id && id < clientlogin::kLast) ||
             (login_tocenter::kFirst < id && id < login_toclient::kLast) ||
             (login_tocenter::kFirst < id && id < login_toclient::kLast) ||
             (clientserver::kFirst < id && id < clientserver::kLast) || 
             (client_toserver::kFirst < id && id < client_toserver::kLast) ||
             (server_toclient::kFirst < id && id < server_toclient::kLast);
#endif /* } */
    return result;
  __LEAVE_FUNCTION
    return false;
}

} //namespace net

} //namespace common
