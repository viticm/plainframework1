#include "pf/base/log.h"
#include "pf/net/packet/factorymanager.h"
#include "common/net/packetfactory.h"
#include "common/setting.h"
#include "engine/system.h"

engine::System *g_engine_system = NULL;

template <> 
engine::System *pf_base::Singleton<engine::System>::singleton_ = NULL;

namespace engine {

System *System::getsingleton_pointer() {
  return singleton_;
}

System &System::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

System::System() {
  //do nothing
}

System::~System() {
  SAFE_DELETE(g_packetfactory_manager);
  SAFE_DELETE(g_setting);
}

pf_db::Manager *System::get_dbmanager() {
  __ENTER_FUNCTION
    pf_db::Manager *dbmanager = NULL;
    bool is_usethread = getconfig_boolvalue(ENGINE_CONFIG_DB_RUN_ASTHREAD);
    if (is_usethread) {
      dbmanager = dynamic_cast<pf_db::Manager *>(db_thread_);
    } else {
      dbmanager = db_manager_;
    }
    return dbmanager;
  __LEAVE_FUNCTION
    return NULL;
}

bool System::init() {
  __ENTER_FUNCTION
    pf_base::global::set_applicationname(_APPLICATION_NAME);
    DEBUGPRINTF("(###) engine for (%s) start...", APPLICATION_NAME);
    if (!Kernel::init_base()) {
      SLOW_ERRORLOG(ENGINE_MODULENAME, 
                    "[engine] (System::init) base module failed");
      return false;
    }
    SLOW_LOG(ENGINE_MODULENAME, "[engine] (System::init) base module success");
    SLOW_LOG(ENGINE_MODULENAME, "[engine] (System::init) start setting module");
    
    if (!init_setting()) {
      SLOW_ERRORLOG(ENGINE_MODULENAME, 
                    "[engine] (System::init) setting module failed");
      return false;
    }
    setconfig(ENGINE_CONFIG_DB_ISACTIVE, true);
    //setconfig(ENGINE_CONFIG_SCRIPT_ISACTIVE, true);
    //setconfig(ENGINE_CONFIG_NET_RUN_ASTHREAD, true);
    setconfig(
        ENGINE_CONFIG_DB_CONNECTION_OR_DBNAME, 
        SETTING_POINTER->gateway_info_.db_connection_ordbname_);
    setconfig(
        ENGINE_CONFIG_DB_USERNAME,
        SETTING_POINTER->gateway_info_.db_user_);
    setconfig(
        ENGINE_CONFIG_DB_PASSWORD,
        SETTING_POINTER->gateway_info_.db_password_);
    setconfig(
        ENGINE_CONFIG_DB_CONNECTOR_TYPE,
        SETTING_POINTER->gateway_info_.db_connectortype_);
    setconfig(ENGINE_CONFIG_NET_ISACTIVE, true);
    setconfig(ENGINE_CONFIG_NET_LISTEN_IP, 
              SETTING_POINTER->gateway_info_.listenip_);
    setconfig(ENGINE_CONFIG_NET_LISTEN_PORT,
              SETTING_POINTER->gateway_info_.listenport_);
    setconfig(ENGINE_CONFIG_NET_CONNECTION_MAX,
              SETTING_POINTER->gateway_info_.net_connectionmax_);
    SLOW_LOG(ENGINE_MODULENAME, 
             "[engine] (System::init) setting module success");
    if (!NET_PACKET_FACTORYMANAGER_POINTER)
      g_packetfactory_manager = new pf_net::packet::FactoryManager();
    if (!NET_PACKET_FACTORYMANAGER_POINTER) return false;
    NET_PACKET_FACTORYMANAGER_POINTER->set_function_registerfactories(
        common::net::registerfactories);
    NET_PACKET_FACTORYMANAGER_POINTER->set_function_isvalid_packetid(
        common::net::isvalid_packetid);
    uint16_t factorysize = common::net::get_facctorysize();
    NET_PACKET_FACTORYMANAGER_POINTER->setsize(factorysize);
    bool result = Kernel::init();
    NET_PACKET_FACTORYMANAGER_POINTER->init();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool System::init_setting() {
  __ENTER_FUNCTION
    if (!SETTING_POINTER)
      g_setting = new common::Setting();
    if (!SETTING_POINTER) return false;
    bool result = SETTING_POINTER->init();
    return result;
  __LEAVE_FUNCTION
    return false;
}

} //namespace engine
