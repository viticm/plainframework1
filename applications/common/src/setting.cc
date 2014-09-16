#include "pf/base/log.h"
#include "pf/base/string.h"
#include "pf/file/ini.h"
#include "common/define/file.h"
#include "pf/net/connection/config.h"
#include "common/setting.h"

common::Setting *g_setting;

template <>
common::Setting *pf_base::Singleton<common::Setting>::singleton_ = NULL;

namespace common {

//--struct start

#ifdef __SERVER__ /* __SERVER__ { */

#ifdef _SERVER
config_info_t::config_info_t() {
  __ENTER_FUNCTION
    global.drop_param = 2.0;
    global.equipment_damage_point = 500;
    global.respawn_param = 1;
    global.pk_refix_of_damage = 100;
    global.disciple_proffer_exp = 2;
    global.god_relive_top_level = 20;
    global.audit_switch = true;
    global.auto_remove_bad_pet = true;
    localization.language = 1;
    zone.size = 10;
    time.recover_time = 10000;
    time.disconnect_time1 = 20000;
    time.disconnect_time2 = 20000;
    time.kick_user_time = 300000;
    time.smu_kick_user_time = 1800000;
    time.drop_box_recycle = 60000;
    time.time_change_interval = 150000;
    time.packet_audit_time = 0;
    monster.max_count = 20000;
    monster.default_respawn_time = 30000;
    monster.default_position_range = 10;
    monster.default_ai_type = 1;
    monster.default_body_time = 5000;
    monster.default_boss_body_time = 5000;
    monster.default_refuse_scan_time = 3000;
    monster.odds_of_change_target = 20;
    monster.odds_of_attack_assistant = 16;
    monster.change_target_cooldown = 3;
    monster.default_max_level = 150;
    portal.max_count = 1024;
    platform.max_count = 1024;
    skill_obj.max_count = 1024;
    special_obj.max_count = 1024;
    bus_obj.max_count = 1024;
    player_shop.max_count = 100;
    player_shop.payment_per_hour = 0;
    scene_timer.max_count = 64;
    human_timer.max_count = 3;
    pet.max_count = 1000;
    pet.body_time = 2000;
    pet.happiness_interval = 600000;
    pet.life_interval = 600000;
    pet.dec_life_step = 7;
    pet.call_up_happiness = 60;
    pet.placard_time = 1800000;
    pet.placard_need_level = 30;
    pet.placard_need_happiness = 100;
    pet.placard_need_life = 3000;
    pet.compound_gen_gu_float = 5;
    pet.compound_grow_float = 5;
    pet.compound_baby_per_wild_with_wild = 10;
    pet.compound_baby_per_baby_with_baby = 95;
    pet.compound_baby_per_wild_with_baby = 30;
    pet.compound_perception_per = 70;
    pet.default_max_level = 115;
    pet.die_lreduce_life = 0.4f;
    human.can_get_exp_range = 60;
    human.out_ghost_time = 300000;
    human.default_move_speed = 5000;
    human.default_attack_speed = 2000;
    human.default_refresh_rate = 60000;
    human.energy_vigor_recover_interval = 300000;
    human.default_max_level = 119;
    human.pk_value_refresh_rate = 3600;
    human.default_pilfer_lock_time = 6000;
    human.default_xinfa_max_level = 150;
    human.level_up_validate_min_level = 0;
    human.level_up_validate_min_level = 20;
    human.can_get_yuanbao_ticket_min_level = 30;
    team.available_follow_dist = 10;
    team.time_for_lose_follow = 30;
    guild.found_duration = 24;
    guild.default_max_member_count = 36;
    guild.response_user_count = 2;
    memset(guild.xianya_max_user, 0, sizeof(guild.xianya_max_user));
    memset(guild.xianya_standard_money, 0, sizeof(guild.xianya_standard_money));
    memset(guild.xianya_max_money, 0, sizeof(guild.xianya_max_money));
    memset(guild.xianya_max_trader, 0, sizeof(guild.xianya_max_trader));
    memset(guild.bank_standard_money, 0, sizeof(guild.bank_standard_money));
    memset(guild.wing_max_user, 0, sizeof(guild.wing_max_user));
    guild.create_city_money = 10000000;
    guild.leave_word_cost = 1000;
    guild.battle_time = 120;
    guild.left_guild_buff_enable = true;
    guild.boom_max_value = 999;
    guild.boom_scale_all = 1;
    guild.boom_scale_online_count = 0.6f;
    guild.boom_scale_attribute_increase = 0.15f;
    guild.boom_scale_trader_transactions_count = 1;
    guild.boom_scale_avg_boom = 0.06f;
    guild.boom_scale_dilation = 0.4f;
    guild.boom_scale_farming = 1.6f;
    guild.boom_scale_trading_mission = 1.6f;
    guild.boom_scale_defence = 0.8f;
    guild.boom_scale_industry = 1.6f;
    guild.boom_scale_technology = 0.8f;
    guild.trade_ticket_decrease_value = 136;
    guild.trading_mission_plug = true;
    guild.validate_xianya_level = true;
    guild_league.max_count = 255;
    guild_league.quit_league_wait_time = 72;
    guild_league.create_money = 1000000;
    relation.password_point = 500;
    relation.prompt_point = 0;
    relation.get_point_need_exp = 1;
    minor_password.unlock_delay_time = 24;
    minor_password.energy_set_cost = 20;
    minor_password.energy_modify_cost = 20;
    minor_password.energy_unlock_cost = 100;
    minor_password.input_error_times_per_day = 5;
    minor_password.password_unlock_time = 24;
    world.hash_online_user_count = 2000;
    world.hash_mail_user_count = 30000;
    world.max_offline_user_count = 50000;
    world.speaker_pool_max = 128;
    memset(temp.user_path,'\0', sizeof(temp.user_path));
    combat.default_damage_fluctuation = 8;
    combat.h0_of_hit_calculation = 10;
    combat.c0_of_crit_calculation = 100;
    combat.c1_of_crit_calculation = 10;
    combat.c2_of_crit_calculation = 25;
    good_and_evil.min_value = 0;
    good_and_evil.max_value = 1000000;
    good_and_evil.get_value_need_level = 20;
    good_and_evil.give_value_min_level = 10;
    good_and_evil.give_value_max_level = 40;
    good_and_evil.give_value_difference_level = 8;
    good_and_evil.give_value_radius = 15;
    good_and_evil.per_member_give_value = 5;
    good_and_evil.per_disciple_give_value = 3;
    good_and_evil.once_max_give_value = 2;
    good_and_evil.member_die_cost_value = 100;
    plot_point.min = 0;
    plot_point.max = 9999;
    economic.rate_a = 0.25f;
    economic.rate_b = 0.1f;
    economic.yuanbao_transaction_scene_id = 0;
    economic.yuanbao_ticket_max = 10000;
    economic.new_server_sale_rate = 1.0f;
    economic.min_exchange_code_level = 15;
    economic.max_exchange_code_level = 25;
    exp.revise_param = 1.0f;
    exp.team_spouse_add_rate = 0.04f;
    exp.team_brother_add_rate = 0.02f;
    exp.team_master_in_add_rate = 0.2f;
    exp.team_master_notin_add_rate = 0.1f;
    exp.team_disciple_add_rate = 0.05f;
    exp.hidden_or_magic_weapon_absorb_rate = 0.01f;
    duel.continue_time = 600;
    duel.need_min_level = 10;
    duel.need_energy = 100;
    warfare.continue_time = 600;
    pk.flag_switch_delay = 600;
    pk.hostile_player_warning = 30;
    memset(ability.assistant_demand_formula_param, 
           0, 
           sizeof(ability.assistant_demand_formula_param));
    fatigue.enable = false;
    fatigue.little_fatigue_time = 10800;
    fatigue.exceeding_fatigue_time = 18000;
    fatigue.reset_fatigue_state_offline_time = 18000;
    yuanbao.max_day_can_cost = 167772150;
    yuanbao.enable_exchage_yuanbao_ticket = true;
    monster_income.little_count = 7000;
    monster_income.nil_count = 10000;
    monster_income.little_percent = 0.5;
    commision_shop.max_count = 10; //max write in code, can't read in config file
    commision_shop.send_back_time = 360;
    commision_shop.yuanbao_fee = 2;
    commision_shop.gold_coin_fee = 2;
    memset(commision_shop.yuanbao_value, 
           0, 
           sizeof(commision_shop.yuanbao_value));
    memset(commision_shop.gold_coin_value, 
           0, 
           sizeof(commision_shop.gold_coin_fee));
  __LEAVE_FUNCTION
}

config_info_t::~config_info_t() {
  //do nothing
}
#endif

login_info_t::login_info_t() {
  __ENTER_FUNCTION
    id = ID_INVALID; 
    memset(db_ip, '\0', sizeof(db_ip));
    db_port = 3306; //default mysql port
    memset(db_connection_ordbname, '\0', sizeof(db_connection_ordbname));
    memset(db_user, '\0', sizeof(db_user));
    memset(db_password, '\0', sizeof(db_password));
    db_connectortype = kDBConnectorTypeODBC;
    encrypt_dbpassword = false;
    client_version = 0;
    db_connect_count = 1;
    turn_player_count = 100;
    proxy_connect = 0;
    enable_license = false;
    relogin_limit = false;
    relogin_stop = false;
    relogin_stop_time = 60000;
    notify_safe_sign = false;
  __LEAVE_FUNCTION
}

login_info_t::~login_info_t() {
  //do nothing
}

gateway_data_t::gateway_data_t() {
  __ENTER_FUNCTION
    memset(ip, '\0', sizeof(ip));
    port = 3306;
    container_postion = -1;
  __LEAVE_FUNCTION
}

gateway_data_t::~gateway_data_t() {
  //do nothing
}

center_info_t::center_info_t() {
  __ENTER_FUNCTION
    id = ID_INVALID;
    zone_id = ID_INVALID;
    share_memory_key.guild = 0;
    share_memory_key.mail = 0;
    share_memory_key.pet = 0;
    share_memory_key.city = 0;
    share_memory_key.global_data = 0;
    share_memory_key.league = 0;
    share_memory_key.find_friend = 0;
    enable_share_memory = true;
  __LEAVE_FUNCTION
}

center_info_t::~center_info_t() {
  //do nothing
}

share_memory_data_struct::share_memory_data_struct() {
  __ENTER_FUNCTION
    key = 0;
    type = 0;
  __LEAVE_FUNCTION
}

share_memory_data_struct::~share_memory_data_struct() {
  //do nothing
}

share_memory_info_t::share_memory_info_t() {
  __ENTER_FUNCTION
    data = NULL;
    obj_count = 0;
    memset(db_ip, '\0', sizeof(db_ip));
    db_port = 3306; //default mysql port
    memset(db_connection_ordbname, '\0', sizeof(db_connection_ordbname));
    memset(db_user, '\0', sizeof(db_user));
    memset(db_password, '\0', sizeof(db_password));
    db_connectortype = kDBConnectorTypeODBC;
    encrypt_dbpassword = false;
    center_data_save_interval = 1200000;
    player_data_save_interval = 900000;
    type = 0;
  __LEAVE_FUNCTION
}

share_memory_info_t::~share_memory_info_t() {
  __ENTER_FUNCTION
    SAFE_DELETE_ARRAY(data);
  __LEAVE_FUNCTION
}

machine_data_t::machine_data_t() {
  __ENTER_FUNCTION
    id = ID_INVALID;
  __LEAVE_FUNCTION
}

machine_data_t::~machine_data_t() {
  //do nothing
}

machine_info_t::machine_info_t() {
  __ENTER_FUNCTION
    data = NULL;
    count = 0;
  __LEAVE_FUNCTION
}

machine_info_t::~machine_info_t() {
  __ENTER_FUNCTION
    SAFE_DELETE_ARRAY(data);
    count = 0;
  __LEAVE_FUNCTION
}

proxy_data_t::proxy_data_t() {
  __ENTER_FUNCTION
    isp = kIspInvalid;
    memset(ip, '\0', sizeof(ip));
    port = 0;
    //enable = false;
  __LEAVE_FUNCTION
}

proxy_data_t::~proxy_data_t() {
  //do nothing
}

server_data_struct::server_data_struct() {
  __ENTER_FUNCTION
    id = ID_INVALID;
    machine_id = ID_INVALID;
    memset(ip, '\0', sizeof(ip));
    port = 0;
    type = -1;
    share_memory_key.human = 0;
    share_memory_key.player_shop = 0;
    share_memory_key.item_serial = 0;
    share_memory_key.commision_shop = 0;
    enable_share_memory = false;
  __LEAVE_FUNCTION
}

server_data_struct::~server_data_struct() {
  //do nothing
}

server_info_t::server_info_t() {
  __ENTER_FUNCTION
    data = NULL;
    count = 0;
    current_server_id = -1;
    net_connectionmax = NET_CONNECTION_MAX;
    memset(center_data.ip, '\0', sizeof(center_data.ip));
    for (int16_t i = 0; i < OVER_SERVER_MAX; ++i)
      hash_server[i] = ID_INVALID;
  __LEAVE_FUNCTION
}

server_info_t::~server_info_t() {
  //do nothing
}

#ifdef _SERVER
scene_data_t::scene_data_t() {
  __ENTER_FUNCTION
    thread_index = INDEX_INVALID;
    client_resource_index = INDEX_INVALID;
    id = ID_INVALID;
    active = false;
    memset(name, '\0', sizeof(name));
    memset(file, '\0', sizeof(file));
    server_id = ID_INVALID;
    type = -1;
    pvp_ruler = 0;
    begin_plus = 0;
    plus_client_resource_index = -1;
    end_plus = 0;
    relive = false;
  __LEAVE_FUNCTION
}

scene_data_t::~scene_data_t() {
  //do nothing
}

scene_info_t::scene_info_t() {
  __ENTER_FUNCTION
    data = NULL;
    count = 0;
    memset(scene_hash, -1, sizeof(scene_hash));
  __LEAVE_FUNCTION
}

scene_info_t::~scene_info_t() {
  //do nothing
}
#endif

internal_ip_of_proxy_t::internal_ip_of_proxy_t() {
  __ENTER_FUNCTION
    memset(proxy_for_cnc_user, '\0', sizeof(proxy_for_cnc_user));
    memset(proxy_for_ctc_user, '\0', sizeof(proxy_for_ctc_user));
    memset(proxy_for_edu_user, '\0', sizeof(proxy_for_edu_user));
  __LEAVE_FUNCTION
}

internal_ip_of_proxy_t::~internal_ip_of_proxy_t() {
  //do nothing
}

isp_enum internal_ip_of_proxy_t::ip_from(const char *ip) {
  __ENTER_FUNCTION
    uint32_t i;
    for (i = 0; kProxyForOneNetworkMax > i; ++i) {
      if (0 == strncmp(ip, proxy_for_cnc_user[i], IP_SIZE)) {
        return kIspChinaNetCom;
      }
      else if (0 == strncmp(ip, proxy_for_ctc_user[i], IP_SIZE)) {
        return kIspChinaTeleCom;
      }
      else if (0 == strncmp(ip, proxy_for_edu_user[i], IP_SIZE)) {
        return kIspChinaEdu;
      }
    }
  __LEAVE_FUNCTION
    return kIspInvalid;
}

//struct end--

//-- class start

GatewayInfo::GatewayInfo() {
  __ENTER_FUNCTION
    info_pool_ = NULL;
    number_ = 0;
    current_gateway_no_ = 0;
    used_ = false;
    memset(ip_, '\0', sizeof(ip_));
    port_ = 0;
#ifdef _GATEWAY
    memset(listenip_, 0, sizeof(listenip_));
    listenport_ = 0;
    net_connectionmax_ = NET_CONNECTION_MAX;
#endif
  __LEAVE_FUNCTION
}

GatewayInfo::~GatewayInfo() {
  __ENTER_FUNCTION
    clean_up();
  __LEAVE_FUNCTION
}

void GatewayInfo::clean_up() {
  __ENTER_FUNCTION
    if (info_pool_ && number_ > 0) {
      uint32_t i;
      for (i = 0; i < number_; ++i) {
        SAFE_DELETE(info_pool_[i]);
      }
    }
    SAFE_DELETE_ARRAY(info_pool_);
    memset(ip_, '\0', sizeof(ip_));
    number_ = 0;
    current_gateway_no_ = 0;
    used_ = false;
  __LEAVE_FUNCTION
}

bool GatewayInfo::init(uint16_t number) {
  __ENTER_FUNCTION
    Assert(number > 0);
    number_ = number;
    info_pool_ = new gateway_data_t*[number_];
    Assert(info_pool_);
    uint32_t i;
    for (i = 0; i < number_; ++i) {
      info_pool_[i] = new gateway_data_t();
      info_pool_[i]->container_postion = static_cast<int16_t>(i);
    }
  __LEAVE_FUNCTION
    return false;
}

uint16_t GatewayInfo::get_number() {
  __ENTER_FUNCTION
    return number_;
  __LEAVE_FUNCTION
    return 0;
}

gateway_data_t* GatewayInfo::next() {
  __ENTER_FUNCTION
    gateway_data_t* current = info_pool_[current_gateway_no_];
    current_gateway_no_ = 
      current_gateway_no_ + 1 >= number_ ? 0 : current_gateway_no_ + 1;
    return current;
  __LEAVE_FUNCTION
    return NULL;
}

void GatewayInfo::begin_use() {
  __ENTER_FUNCTION
    current_gateway_no_ = 0;
    used_ = true;
    if (info_pool_) {
      gateway_data_t* current = info_pool_[current_gateway_no_];
      port_ = current->port;
      snprintf(ip_, sizeof(ip_) - 1, "%s", current->ip);
    }
  __LEAVE_FUNCTION
}

bool GatewayInfo::is_use() {
  __ENTER_FUNCTION
    return used_;
  __LEAVE_FUNCTION
    return false;
}

#endif /* } __SERVER__ */

Setting *Setting::getsingleton_pointer() {
  return singleton_;
}

Setting &Setting::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

Setting::Setting() {
  //do nothing
}

Setting::~Setting() {
  //do noting
}

bool Setting::init() {
  __ENTER_FUNCTION
#ifdef __SERVER__ /* __SERVER__ { */
    load_config_info();
    load_login_info();
    load_center_info();
    load_gateway_info();
    load_share_memory_info();
    load_machine_info();
    load_server_info();
    load_scene_info();
    load_copy_scene_info();
#endif /* } __SERVER__ */
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Setting::reload() {
  __ENTER_FUNCTION
#ifdef __SERVER__ /* __SERVER__ { */
    load_config_info_reload();
    load_login_info_reload();
    load_center_info_reload();
    load_gateway_info_reload();
    load_share_memory_info_reload();
    load_machine_info_reload();
    load_server_info_reload();
    load_scene_info_reload();
    load_copy_scene_info_reload();
#endif /* } __SERVER__ */
  __LEAVE_FUNCTION
}

#ifdef __SERVER__ /* __SERVER__ { */

void Setting::load_config_info() {
  __ENTER_FUNCTION
    load_config_info_only();
    load_config_info_reload();
  __LEAVE_FUNCTION
}

void Setting::load_config_info_only() { //this params just read once
#ifdef _SERVER
  __ENTER_FUNCTION
    pf_file::Ini config_info_ini(CONFIG_INFO_FILE);
    config_info_.zone.size = 
      config_info_ini.read_uint8("Zone", "Size");
    config_info_.portal.max_count = 
      config_info_ini.read_uint16("Portal", "MaxCount");
    config_info_.platform.max_count = 
      config_info_ini.read_uint16("Platform", "MaxCount");
    config_info_.skill_obj.max_count = 
      config_info_ini.read_uint16("SkillObj", "MaxCount");
    config_info_.special_obj.max_count = 
      config_info_ini.read_uint16("SpecialObj", "MaxCount");
    config_info_.bus_obj.max_count = 
      config_info_ini.read_uint16("BusObj", "MaxCount");
    config_info_.player_shop.max_count = 
      config_info_ini.read_uint16("PlayerShop", "MaxCount");
    config_info_.scene_timer.max_count = 
      config_info_ini.read_uint16("SceneTimer","MaxCount");
    config_info_.human_timer.max_count = 
      config_info_ini.read_uint16("HumanTimer","MaxCount");
    config_info_.localization.language = 
      config_info_ini.read_uint8("Localization", "Language");
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s only ... ok!", 
             CONFIG_INFO_FILE);
  __LEAVE_FUNCTION
#endif
}

void Setting::load_config_info_reload() { //this params can reload again
#ifdef _SERVER
  __ENTER_FUNCTION
    pf_file::Ini config_info_ini(CONFIG_INFO_FILE);
    uint32_t i;
    config_info_.global.drop_param = 
      config_info_ini.read_float("Global", "DropParam");
    config_info_.global.equipment_damage_point = 
      config_info_ini.read_uint32("Global", "EquipmentDamagePoint");
    config_info_.global.respawn_param = 
      config_info_ini.read_uint8("Global", "RespawnParam");
    config_info_.global.pk_refix_of_damage = 
      config_info_ini.read_uint16("Global", "PkRefixOfDamage");
    config_info_.global.disciple_proffer_exp = 
      config_info_ini.read_uint8("Global", "DiscipleProfferExp");
    config_info_.global.god_relive_top_level = 
      config_info_ini.read_uint8("Global", "GodReliveTopLevel");
    config_info_.global.audit_switch = 
      config_info_ini.read_bool("Global", "AuditSwitch");
    config_info_.global.auto_remove_bad_pet = 
      config_info_ini.read_bool("Global", "AutoRemoveBadPet");
    config_info_.time.recover_time = 
      config_info_ini.read_uint32("Time", "RecoverTime");
    config_info_.time.disconnect_time1 = 
      config_info_ini.read_uint32("Time", "DisconnectTime1");
    config_info_.time.disconnect_time2 = 
      config_info_ini.read_uint32("Time", "DisconnectTime2");
    config_info_.time.kick_user_time = 
      config_info_ini.read_uint32("Time", "KickUserTime");
    config_info_.time.smu_kick_user_time = 
      config_info_ini.read_uint32("Time", "SMUKickUserTime");
    config_info_.time.drop_box_recycle = 
      config_info_ini.read_uint32("Time", "DropBoxRecycle");
    config_info_.time.time_change_interval = 
      config_info_ini.read_uint32("Time", "TimeChangeInterval");
    config_info_.time.packet_audit_time = 
      config_info_ini.read_uint32("Time", "PacketAuditTime");
    config_info_.monster.max_count = 
      config_info_ini.read_uint32("Monster", "MaxCount");
    config_info_.monster.default_respawn_time = 
      config_info_ini.read_uint32("Monster", "DefaultRespawnTime");
    config_info_.monster.default_position_range = 
      config_info_ini.read_uint16("Monster", "DefaultPositionRange");
    config_info_.monster.default_ai_type = 
      config_info_ini.read_uint8("Monster", "DefaultAIType");
    config_info_.monster.default_body_time = 
      config_info_ini.read_uint32("Monster", "DefaultBodyTime");
    config_info_.monster.default_boss_body_time = 
      config_info_ini.read_uint32("Monster", "DefaultBossBodyTime");
    config_info_.monster.default_refuse_scan_time = 
      config_info_ini.read_uint32("Monster", "DefaultRefuseScanTime");
    config_info_.monster.odds_of_change_target = 
      config_info_ini.read_uint16("Monster", "OddsOfChangeTarget");
    config_info_.monster.odds_of_attack_assistant = 
      config_info_ini.read_uint16("Monster", "OddsOfAttackAssistant");
    config_info_.monster.change_target_cooldown = 
      config_info_ini.read_uint16("Monster", "ChangeTargetCooldown");
    config_info_.monster.default_max_level = 
      config_info_ini.read_uint8("Monster", "DefaultMaxLevel");
    config_info_.player_shop.payment_per_hour = 
      config_info_ini.read_uint32("PlayerShop", "PaymentPerHour");
    config_info_.pet.max_count = 
      config_info_ini.read_uint32("Pet", "MaxCount");
    config_info_.pet.body_time = 
      config_info_ini.read_uint32("Pet", "BodyTime");
    config_info_.pet.happiness_interval = 
      config_info_ini.read_uint32("Pet", "HappinessInterval");
    config_info_.pet.life_interval = 
      config_info_ini.read_uint32("Pet", "LifeInterval");
    config_info_.pet.dec_life_step = 
      config_info_ini.read_uint8("Pet", "PetDecLifeStep");
    config_info_.pet.call_up_happiness = 
      config_info_ini.read_uint8("Pet", "CallUpHappiness");
    config_info_.pet.placard_time = 
      config_info_ini.read_uint32("Pet", "PlacardTime");
    config_info_.pet.placard_need_level = 
      config_info_ini.read_uint8("Pet", "PlacardNeedLevel");
    config_info_.pet.placard_need_happiness = 
      config_info_ini.read_uint16("Pet", "PlacardNeedHappiness");
    config_info_.pet.placard_need_life = 
      config_info_ini.read_uint32("Pet", "PlacardNeedLife");
    config_info_.pet.compound_gen_gu_float = 
      config_info_ini.read_uint8("Pet", "CompoundGenGuFloat");
    config_info_.pet.compound_grow_float = 
      config_info_ini.read_uint8("Pet", "CompoundGrowFloat");
    config_info_.pet.compound_baby_per_wild_with_wild = 
      config_info_ini.read_uint8("Pet", "CompoundBabyPerWildWithWild");
    config_info_.pet.compound_baby_per_baby_with_baby = 
      config_info_ini.read_uint8("Pet", "CompoundBabyPerBabyWithBaby");
    config_info_.pet.compound_baby_per_wild_with_baby = 
      config_info_ini.read_uint8("Pet", "CompoundBabyPerWildWithBaby");
    config_info_.pet.compound_perception_per = 
      config_info_ini.read_uint8("Pet", "CompoundPerceptionPer");
    config_info_.pet.default_max_level = 
      config_info_ini.read_uint8("Pet", "DefaultMaxLevel");
    config_info_.pet.die_lreduce_life = 
      config_info_ini.read_float("Pet", "DieLReduceLife");
    config_info_.human.can_get_exp_range = 
      config_info_ini.read_uint16("Human", "CanGetExpRange"); 
    config_info_.human.out_ghost_time = 
      config_info_ini.read_uint32("Human", "OutGhostTime");
    config_info_.human.default_move_speed = 
      config_info_ini.read_uint32("Human", "DefaultMoveSpeed");
    config_info_.human.default_attack_speed = 
      config_info_ini.read_uint32("Human", "DefaultAttackSpeed");
    config_info_.human.default_refresh_rate = 
      config_info_ini.read_uint32("Human", "DefaultRefreshRate");
    config_info_.human.energy_vigor_recover_interval = 
      config_info_ini.read_uint32("Human", "EnergyVigorRecoverInterval");
    config_info_.human.default_max_level = 
      config_info_ini.read_uint8("Human", "DefaultMaxLevel");
    config_info_.human.pk_value_refresh_rate = 
      config_info_ini.read_uint32("Human", "PKValueRefreshRate");
    config_info_.human.default_pilfer_lock_time = 
      config_info_ini.read_uint32("Human", "DefPilferLockTime");
    config_info_.human.default_xinfa_max_level = 
      config_info_ini.read_uint8("Human", "DefaultXinfaMaxLevel");
    config_info_.human.level_up_validate_min_level = 
      config_info_ini.read_uint8("Human", "LevelUpValidateMinLevel");
    config_info_.human.level_up_validate_max_level = 
      config_info_ini.read_uint8("Human", "LevelUpValidateMaxLevel");
    config_info_.human.can_get_yuanbao_ticket_min_level = 
      config_info_ini.read_uint8("Human", "CanGetYuanbaoTicketMinLevel");
    config_info_.team.available_follow_dist = 
      config_info_ini.read_uint16("Team", "VailableFollowDist");
    config_info_.team.time_for_lose_follow = 
      config_info_ini.read_uint32("Team", "TimeForLoseFollow");
    config_info_.guild.found_duration = 
      config_info_ini.read_uint16("Guild", "FoundDuration");
    config_info_.guild.default_max_member_count = 
      config_info_ini.read_uint16("Guild", "DefaultMaxMemberCount");
    config_info_.guild.response_user_count = 
      config_info_ini.read_uint8("Guild", "ResponseUserCount");
    //-- loop read
    char key_temp[33];
    for (i = 0; i < GUILDLEVEL_MAX; ++i) {
      memset(key_temp, '\0', sizeof(key_temp));
      snprintf(key_temp, sizeof(key_temp) - 1, "XianYaMaxUser%d", i);
      config_info_.guild.xianya_max_user[i] = 
        config_info_ini.read_uint16("Guild", 
                                    static_cast<const char*>(key_temp));
    }
    
    for (i = 0; i < GUILDLEVEL_MAX; ++i) {
      memset(key_temp, '\0', sizeof(key_temp));
      snprintf(key_temp, sizeof(key_temp) - 1, "XianYaStandardMoney%d", i);
      config_info_.guild.xianya_standard_money[i] = 
        config_info_ini.read_uint32("Guild", 
                                    static_cast<const char*>(key_temp));
    }

    for (i = 0; i < GUILDLEVEL_MAX; ++i) {
      memset(key_temp, '\0', sizeof(key_temp));
      snprintf(key_temp, sizeof(key_temp) - 1, "XianYaMaxMoney%d", i);
      config_info_.guild.xianya_max_money[i] = 
        config_info_ini.read_uint32("Guild", 
                                    static_cast<const char*>(key_temp));
    }
    
    for (i = 0; i < GUILDLEVEL_MAX; ++i) {
      memset(key_temp, '\0', sizeof(key_temp));
      snprintf(key_temp, sizeof(key_temp) - 1, "XianYaMaxTrader%d", i);
      config_info_.guild.xianya_max_trader[i] = 
        config_info_ini.read_uint8("Guild", 
                                   static_cast<const char*>(key_temp));
    }

    for (i = 0; i < GUILDLEVEL_MAX; ++i) {
      memset(key_temp, '\0', sizeof(key_temp));
      snprintf(key_temp, sizeof(key_temp) - 1, "BankStandardMoney%d", i);
      config_info_.guild.bank_standard_money[i] = 
        config_info_ini.read_uint8("Guild", 
                                   static_cast<const char*>(key_temp));
    }

    for (i = 0; i < GUILDLEVEL_MAX; ++i) {
      memset(key_temp, '\0', sizeof(key_temp));
      snprintf(key_temp, sizeof(key_temp) - 1, "WingMaxUser%d", i);
      config_info_.guild.wing_max_user[i] = 
        config_info_ini.read_uint16("Guild", 
                                    static_cast<const char*>(key_temp));
    }
    //loop read --

    config_info_.guild.create_city_money = 
      config_info_ini.read_uint32("Guild", "CreateCityMoney");
    config_info_.guild.leave_word_cost = 
      config_info_ini.read_uint32("Guild", "LeaveWordCost");
    config_info_.guild.battle_time = 
      config_info_ini.read_uint32("Guild", "BattleTime");
    config_info_.guild.left_guild_buff_enable = 
      config_info_ini.read_bool("Guild", "LeftGuildBuffEnable");
    config_info_.guild.boom_max_value = 
      config_info_ini.read_uint16("Guild", "BoomMaxValue");
    config_info_.guild.boom_scale_all = 
      config_info_ini.read_float("Guild", "BoomScaleAll");
    config_info_.guild.boom_scale_online_count = 
      config_info_ini.read_float("Guild", "BoomScaleOnlineCount");
    config_info_.guild.boom_scale_attribute_increase = 
      config_info_ini.read_float("Guild", "BoomScaleAttributeIncrease");
    config_info_.guild.boom_scale_trader_transactions_count = 
      config_info_ini.read_float("Guild", "BoomScaleTraderTransactionsCount");
    config_info_.guild.boom_scale_avg_boom = 
      config_info_ini.read_float("Guild", "BoomScaleAvgBoom");
    config_info_.guild.boom_scale_dilation = 
      config_info_ini.read_float("Guild", "BoomScaleDilation");
    config_info_.guild.boom_scale_farming = 
      config_info_ini.read_float("Guild", "BoomScaleFarming");
    config_info_.guild.boom_scale_trading_mission = 
      config_info_ini.read_float("Guild", "BoomScaleTradingMission");
    config_info_.guild.boom_scale_defence = 
      config_info_ini.read_float("Guild", "BoomScaleDefence");
    config_info_.guild.boom_scale_industry = 
      config_info_ini.read_float("Guild", "BoomScaleIndustry");
    config_info_.guild.boom_scale_technology = 
      config_info_ini.read_float("Guild", "BoomScaleTechnology");
    config_info_.guild.trade_ticket_decrease_value = 
      config_info_ini.read_uint16("Guild", "TradeTicketDecreaseValue");
    config_info_.guild.trading_mission_plug = 
      config_info_ini.read_bool("Guild", "TradingMissionPlug");
    config_info_.guild.validate_xianya_level = 
      config_info_ini.read_bool("Guild", "ValidateXianYaLevel");
    config_info_.guild_league.max_count = 
      config_info_ini.read_uint16("GuildLeague", "MaxCount");
    config_info_.guild_league.quit_league_wait_time = 
      config_info_ini.read_uint16("GuildLeague", "QuitLeagueWaitTime");
    config_info_.guild_league.create_money = 
      config_info_ini.read_uint32("GuildLeague", "CreateMoney");
    config_info_.relation.password_point = 
      config_info_ini.read_uint16("Relation", "PasswordPoint");
    config_info_.relation.prompt_point = 
      config_info_ini.read_uint16("Relation", "PromptPoint");
    config_info_.relation.get_point_need_exp = 
      config_info_ini.read_uint32("Relation", "GetPointNeedExp");
    config_info_.minor_password.unlock_delay_time = 
      config_info_ini.read_uint16("MinorPassword", "UnlockDelayTime");
    config_info_.minor_password.energy_set_cost = 
      config_info_ini.read_uint16("MinorPassword", "EnergySetCost");
    config_info_.minor_password.energy_modify_cost = 
      config_info_ini.read_uint16("MinorPassword", "EnergyModifyCost");
    config_info_.minor_password.energy_unlock_cost = 
      config_info_ini.read_uint16("MinorPassword", "EnergyUnlockCost");
    config_info_.minor_password.input_error_times_per_day = 
      config_info_ini.read_uint16("MinorPassword", "InputErrorTimesPerDay");
    config_info_.minor_password.password_unlock_time = 
      config_info_ini.read_uint16("MinorPassword", "PasswordUnlockTime");
    config_info_.world.hash_online_user_count = 
      config_info_ini.read_uint32("World", "HashOnlineUserCount");
    config_info_.world.hash_mail_user_count = 
      config_info_ini.read_uint32("World", "HashMailUserCount");
    config_info_.world.max_offline_user_count = 
      config_info_ini.read_uint32("World", "MaxOfflineUserCount");
    config_info_.world.speaker_pool_max = 
      config_info_ini.read_uint8("World", "SpeakerPoolMax");
    config_info_ini.readstring("Temp", 
                               "UserPath", 
                               config_info_.temp.user_path, 
                               sizeof(config_info_.temp.user_path) - 1);
    config_info_.combat.default_damage_fluctuation = 
      config_info_ini.read_uint32("Combat", "DefaultDamageFluctuation");
    config_info_.combat.h0_of_hit_calculation = 
      config_info_ini.read_uint16("Combat", "H0ofHitCalculation");
    config_info_.combat.c0_of_crit_calculation = 
      config_info_ini.read_uint16("Combat", "C0ofCritCalculation");
    config_info_.combat.c1_of_crit_calculation = 
      config_info_ini.read_uint16("Combat", "C1ofCritCalculation");
    config_info_.combat.c2_of_crit_calculation = 
      config_info_ini.read_uint16("Combat", "C2ofCritCalculation");
    config_info_.good_and_evil.min_value = 
      config_info_ini.read_uint32("GoodAndEvil", "MaxValue");
    config_info_.good_and_evil.max_value = 
      config_info_ini.read_uint32("GoodAndEvil", "MinValue");
    config_info_.good_and_evil.get_value_need_level = 
      config_info_ini.read_uint8("GoodAndEvil", "GetValueNeedLevel");
    config_info_.good_and_evil.give_value_min_level = 
      config_info_ini.read_uint8("GoodAndEvil", "GiveValueMinLevel");
    config_info_.good_and_evil.give_value_max_level = 
      config_info_ini.read_uint8("GoodAndEvil", "GiveValueMaxLevel");
    config_info_.good_and_evil.give_value_difference_level = 
      config_info_ini.read_uint8("GoodAndEvil", "GiveValueDifferenceLevel");
    config_info_.good_and_evil.give_value_radius = 
      config_info_ini.read_uint16("GoodAndEvil", "GiveValueRadius");
    config_info_.good_and_evil.per_member_give_value = 
      config_info_ini.read_uint32("GoodAndEvil", "PerMemberGiveValue");
    config_info_.good_and_evil.per_disciple_give_value = 
      config_info_ini.read_uint32("GoodAndEvil", "PerDiscipleGiveValue");
    config_info_.good_and_evil.once_max_give_value = 
      config_info_ini.read_uint32("GoodAndEvil", "OnceMaxGiveValue");
    config_info_.good_and_evil.member_die_cost_value = 
      config_info_ini.read_uint32("GoodAndEvil", "MemberDieCostValue");
    config_info_.plot_point.min = 
      config_info_ini.read_uint16("PlotPoint", "Min");
    config_info_.plot_point.max = 
      config_info_ini.read_uint16("PlotPoint", "Max");
    config_info_.economic.rate_a = 
      config_info_ini.read_float("Economic", "RateA");
    config_info_.economic.rate_b = 
      config_info_ini.read_float("Economic", "RateB");
    config_info_.economic.yuanbao_transaction_scene_id = 
      config_info_ini.read_uint16("Economic", "YuanBaoTransactionSceneId");
    config_info_.economic.yuanbao_ticket_max = 
      config_info_ini.read_uint32("Economic", "YuanBaoTicketMax");
    config_info_.economic.new_server_sale_rate = 
      config_info_ini.read_float("Economic", "NewServerSaleRate");
    config_info_.economic.min_exchange_code_level = 
      config_info_ini.read_uint8("Economic", "MinExchangeCodeLevel");
    config_info_.economic.max_exchange_code_level = 
      config_info_ini.read_uint8("Economic", "MaxExchangeCodeLevel");
    config_info_.exp.revise_param = 
      config_info_ini.read_float("Exp", "ReviseParam");
    config_info_.exp.team_spouse_add_rate = 
      config_info_ini.read_float("Exp", "TeamSpouseAddRate");
    config_info_.exp.team_brother_add_rate = 
      config_info_ini.read_float("Exp", "TeamBrotherAddRate");
    config_info_.exp.team_master_in_add_rate = 
      config_info_ini.read_float("Exp", "TeamMasterInTeamAddRate");
    config_info_.exp.team_master_notin_add_rate = 
      config_info_ini.read_float("Exp", "TeamMasterNotInTeamAddRate");
    config_info_.exp.team_disciple_add_rate = 
      config_info_ini.read_float("Exp", "TeamDiscipleAddRate");
    config_info_.exp.hidden_or_magic_weapon_absorb_rate = 
      config_info_ini.read_float("Exp", "HiddenOrMagicWeaponAbsorbRate");
    config_info_.duel.continue_time = 
      config_info_ini.read_uint32("Duel", "ContinueTime");
    config_info_.duel.need_min_level = 
      config_info_ini.read_uint8("Duel", "NeedMinLevel");
    config_info_.duel.need_energy = 
      config_info_ini.read_uint16("Duel", "NeedEnergy");
    config_info_.warfare.continue_time = 
      config_info_ini.read_uint32("Warfare", "ContinueTime");
    config_info_.pk.flag_switch_delay = 
      config_info_ini.read_uint32("PK", "FlagSwitchDelay");
    config_info_.pk.hostile_player_warning = 
      config_info_ini.read_uint32("PK", "HostilePlayerWarning");
    config_info_.fight_back.legal_fight_back_time = 
      config_info_ini.read_uint32("FightBack", "LegalFightBackTime");

    //-- loop read
    for (i = 0; 
        i < sizeof(config_info_.ability.assistant_demand_formula_param); 
        ++i) {
      memset(key_temp, '\0', sizeof(key_temp));
      snprintf(key_temp, 
               sizeof(key_temp) - 1, 
               "AssistantDemandFormulaParam%d", 
               i);
      config_info_.ability.assistant_demand_formula_param[i] = 
        config_info_ini.read_float("Ability", 
                                   static_cast<const char*>(key_temp));
    }
    //loop read --

    config_info_.fatigue.enable = 
      config_info_ini.read_bool("Fatigue", "Enable");
    config_info_.fatigue.little_fatigue_time = 
      config_info_ini.read_uint32("Fatigue", "LittleFatigueTime");
    config_info_.fatigue.exceeding_fatigue_time = 
      config_info_ini.read_uint32("Fatigue", "ExceedingFatigueTime");
    config_info_.fatigue.reset_fatigue_state_offline_time = 
      config_info_ini.read_uint32("Fatigue", "ResetFatigueStateOfflineTime");
    config_info_.yuanbao.max_day_can_cost = 
      config_info_ini.read_uint32("YuanBao", "MaxDayCanCost");
    config_info_.yuanbao.enable_exchage_yuanbao_ticket = 
      config_info_ini.read_bool("YuanBao", "EnableExchageYuanBaoTicket");
    config_info_.monster_income.little_count = 
      config_info_ini.read_uint32("MonsterIncome", "LittleCount");
    config_info_.monster_income.nil_count = 
      config_info_ini.read_uint32("MonsterIncome", "NilCount");
    config_info_.monster_income.little_percent = 
      config_info_ini.read_float("MonsterIncome", "LittlePercent");
    config_info_.commision_shop.send_back_time = 
      config_info_ini.read_uint32("CommisionShop", "SendBackTime");
    config_info_.commision_shop.yuanbao_fee = 
      config_info_ini.read_uint8("CommisionShop", "YuanBaoFee");
    config_info_.commision_shop.gold_coin_fee = 
      config_info_ini.read_uint8("CommisionShop", "GoldCoinFee");

    //-- loop read
    for (i = 0; i < sizeof(config_info_.commision_shop.yuanbao_value); ++i) {
      memset(key_temp, '\0', sizeof(key_temp));
      snprintf(key_temp, sizeof(key_temp) - 1, "YuanBaoValue%d", i);
      config_info_.commision_shop.yuanbao_value[i] = 
        config_info_ini.read_uint32("CommisionShop", 
                                    static_cast<const char*>(key_temp));
    }

    for (i = 0; i < sizeof(config_info_.commision_shop.gold_coin_value); ++i) {
      memset(key_temp, '\0', sizeof(key_temp));
      snprintf(key_temp, sizeof(key_temp) - 1, "GoldCoinValueValue%d", i);
      config_info_.commision_shop.gold_coin_value[i] = 
        config_info_ini.read_uint32("CommisionShop", 
                                    static_cast<const char*>(key_temp));
    }
    //loop read --

    config_info_.commision_shop.close_yuanbao_sell = 
      config_info_ini.read_bool("CommisionShop", "CloseYuanBaoSell");
    config_info_.commision_shop.close_gold_coin_sell = 
      config_info_ini.read_bool("CommisionShop", "CloseGoldCoinSell");
    config_info_.cache_log_time.login_cache_time = 
      config_info_ini.read_uint16("CacheLogTime", "LogInCacheTime");
    config_info_.system_notice.join_guild = 
      config_info_ini.read_bool("SystemNotice", "JoinGuild");
    
    //-- loop read
    for (i = 0; 
         i < sizeof(config_info_.player_count_of_scene.hour_point); 
         ++i) {
      memset(key_temp, '\0', sizeof(key_temp));
      snprintf(key_temp, sizeof(key_temp) - 1, "HourPoint%d", i);
      config_info_.player_count_of_scene.hour_point[i] = 
        config_info_ini.read_uint8("PlayerCountOfScene", 
                                   static_cast<const char*>(key_temp));
    }
    //loop read --

    //-- loop read
    for (i = 0; i < 2; ++i) {
      memset(key_temp, '\0', sizeof(key_temp));
      snprintf(key_temp, sizeof(key_temp) - 1, "Card%d", i);
      config_info_.active_riches_card.state[i] = 
        config_info_ini.read_bool("ActiveRichesCard", 
                                  static_cast<const char*>(key_temp));
    }
    //loop read --
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s reload ... ok!", 
             CONFIG_INFO_FILE);
  __LEAVE_FUNCTION
#endif
}

void Setting::load_login_info() {
  __ENTER_FUNCTION
    load_login_info_only();
    load_login_info_reload();
  __LEAVE_FUNCTION
}

void Setting::load_login_info_only() {
#ifdef _LOGIN
  __ENTER_FUNCTION
    pf_file::Ini login_info_ini(LOGIN_INFO_FILE);
    login_info_.id = login_info_ini.read_int16("System", "ID");
    login_info_ini.readstring("System", 
                              "DBIP", 
                              login_info_.db_ip, 
                              sizeof(login_info_.db_ip) - 1);
    login_info_.db_port = login_info_ini.read_uint16("System", "DBPort");
    login_info_ini.readstring("System", 
                              "DBConnectionOrDBName", 
                              login_info_.db_connection_ordbname,
                              sizeof(login_info_.db_connection_ordbname) - 1);
    login_info_ini.readstring("System", 
                              "DBUser", 
                              login_info_.db_user, 
                              sizeof(login_info_.db_user) - 1);
    login_info_ini.readstring("System", 
                              "DBPassword", 
                              login_info_.db_password, 
                              sizeof(login_info_.db_password) - 1);
    login_info_.db_connectortype = static_cast<dbconnector_type_t>( 
      login_info_ini.read_int8("System", "DBConnectorType"));
    login_info_.encrypt_dbpassword = 
      login_info_ini.read_bool("System", "EncryptDBPassword");
    login_info_.client_version = 
      login_info_ini.read_uint16("System", "ClientVersion");
    login_info_.db_connect_count = 
      login_info_ini.read_uint8("System", "DBConnectCount");
    login_info_.turn_player_count = 
      login_info_ini.read_uint16("System", "TurnPlayerCount");
    login_info_.proxy_connect = 
      login_info_ini.read_uint8("System", "ProxyConnect");
    login_info_.enable_license = 
      login_info_ini.read_bool("System", "EnableLicense");
    login_info_.relogin_limit = 
      login_info_ini.read_bool("System", "ReLoginLimit");
    login_info_.relogin_stop = 
      login_info_ini.read_bool("System", "ReLoginStop");
    login_info_.relogin_stop_time = 
      login_info_ini.read_uint32("System", "ReLoginStopTime");
    login_info_.notify_safe_sign = 
      login_info_ini.read_bool("System", "NotifySafeSign");
    login_info_.clientin_sameip_max =
      login_info_ini.read_int8("System", "ClientInSameIpMax");
    login_info_.net_connectionmax =
      login_info_ini.read_uint16("System", "NetConnectionMax");
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s only ... ok!", 
             LOGIN_INFO_FILE);
  __LEAVE_FUNCTION
#endif
}

void Setting::load_login_info_reload() {
#ifdef _LOGIN
  __ENTER_FUNCTION
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s reload ... ok!", 
             LOGIN_INFO_FILE);
  __LEAVE_FUNCTION
#endif
}

void Setting::load_center_info() {
  __ENTER_FUNCTION
    load_center_info_only();
    load_center_info_reload();
  __LEAVE_FUNCTION
}

void Setting::load_center_info_only() {
#ifndef _GATEWAY
  __ENTER_FUNCTION
    pf_file::Ini center_info_ini(CENTER_INFO_FILE);
    center_info_.id = center_info_ini.read_int16("System", "ID");
    center_info_.zone_id = center_info_ini.read_int16("System", "ZoneID");
    center_info_.share_memory_key.guild = 
      center_info_ini.read_uint32("System", "GuildShareMemoryKey");
    center_info_.share_memory_key.mail = 
      center_info_ini.read_uint32("System", "MailShareMemoryKey");
    center_info_.share_memory_key.pet = 
      center_info_ini.read_uint32("System", "PetShareMemoryKey");
    center_info_.share_memory_key.city = 
      center_info_ini.read_uint32("System", "CityShareMemoryKey");
    center_info_.share_memory_key.global_data = 
      center_info_ini.read_uint32("System", "GlobalDataShareMemoryKey");
    center_info_.share_memory_key.league = 
      center_info_ini.read_uint32("System", "LeagueShareMemoryKey");
    center_info_.share_memory_key.find_friend = 
      center_info_ini.read_uint32("System", "FindFriendShareMemoryKey");
    center_info_.enable_share_memory = 
      center_info_ini.read_bool("System", "EnableShareMemory");
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s only ... ok!", 
             CENTER_INFO_FILE);
  __LEAVE_FUNCTION
#endif
}

void Setting::load_center_info_reload() {
#ifndef _GATEWAY
  __ENTER_FUNCTION
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s reload ... ok!", 
             CENTER_INFO_FILE);
  __LEAVE_FUNCTION
#endif
}

void Setting::load_gateway_info() {
  __ENTER_FUNCTION
    load_gateway_info_only();
    load_gateway_info_reload();
  __LEAVE_FUNCTION
}

void Setting::load_gateway_info_only() {
  __ENTER_FUNCTION
    pf_file::Ini gateway_info_ini(GATEWAY_INFO_FILE);
    pf_file::Ini server_info_ini(SERVER_INFO_FILE);
    uint16_t number;
    if (false == server_info_ini.read_exist_uint16("Gateway", 
                                                   "Number", 
                                                   number)) {
      AssertEx(false, 
               "Setting::load_gateway_info_only is failed," 
               "can't find Gateway Number");
    }
    gateway_info_.clean_up();
    gateway_info_.init(number);
    //db info by viticm
#ifdef _GATEWAY
    gateway_info_ini.readstring("System", 
                                "DBIP", 
                                gateway_info_.db_ip_, 
                                sizeof(gateway_info_.db_ip_) - 1);
    gateway_info_.db_port_ = gateway_info_ini.read_uint16("System", "DBPort");
    gateway_info_ini.readstring(
        "System", 
        "DBConnectionOrDBName", 
        gateway_info_.db_connection_ordbname_, 
        sizeof(gateway_info_.db_connection_ordbname_) - 1);
    gateway_info_ini.readstring("System", 
                                "DBUser", 
                                gateway_info_.db_user_, 
                                sizeof(gateway_info_.db_user_) - 1);
    gateway_info_ini.readstring("System", 
                                "DBPassword", 
                                gateway_info_.db_password_, 
                                sizeof(gateway_info_.db_password_) - 1);
    gateway_info_.db_connectortype_ = static_cast<dbconnector_type_t>(
      gateway_info_ini.read_int8("System", "DBConnectorType"));
    gateway_info_.encrypt_dbpassword_ = 
      gateway_info_ini.read_bool("System", "EncryptDBPassword");
    if (gateway_info_.encrypt_dbpassword_) {
      char temp[DB_PASSWORD_LENGTH] = {0};
      pf_base::string::safecopy(temp, gateway_info_.db_password_, sizeof(temp));
      memset(gateway_info_.db_password_, 0, sizeof(gateway_info_.db_password_));
      pf_base::string::decrypt(temp, 
                               gateway_info_.db_password_, 
                               sizeof(gateway_info_.db_password_) - 1);
    }
    gateway_info_ini.readstring("System",
                                "NetListenIP",
                                gateway_info_.listenip_,
                                sizeof(gateway_info_.listenip_) - 1);
    gateway_info_.listenport_ = 
      gateway_info_ini.read_uint16("System", "NetListenPort");
    gateway_info_.net_connectionmax_ = 
      gateway_info_ini.read_uint16("System", "NetConnectionMax");
#endif
    int32_t i;
    for (i = 0; i < gateway_info_.get_number(); ++i) {
      char key[65];
      char message[256];
      memset(key, '\0', sizeof(key));
      memset(message, '\0', sizeof(message));
      gateway_data_t* gateway_data = gateway_info_.next();
      snprintf(key, sizeof(key) - 1, "IP%d", i);
      if (false == server_info_ini.read_existstring(
            "Gateway", 
            static_cast<const char*>(key), gateway_data->ip, 
            sizeof(gateway_data->ip) - 1)) {
      snprintf(message, 
               sizeof(message) - 1, 
               "Setting::load_gateway_info_only is failed, can't find key: %s", 
               key);
        AssertEx(false, message);
      }
      memset(key, '\0', sizeof(key));
      memset(message, '\0', sizeof(message));
      snprintf(key, sizeof(key) - 1, "Port%d", i);
      if (false == server_info_ini.read_exist_uint16(
            "Gateway",
            static_cast<const char*>(key),
            gateway_data->port)) {
        snprintf(message, 
                 sizeof(message) - 1, 
                 "Setting::load_gateway_info_only is failed, can't find key: %s", 
                 key);
        AssertEx(false, message);
      }
      if (0 == i) {
        gateway_info_.port_ = gateway_data->port;
        memcpy(gateway_info_.ip_, 
               gateway_data->ip, 
               sizeof(gateway_info_.ip_) - 1);
      }
    }
    gateway_info_.begin_use();
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s only ... ok!", 
             GATEWAY_INFO_FILE);
  __LEAVE_FUNCTION
}

void Setting::load_gateway_info_reload() {
  __ENTER_FUNCTION
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s reload ... ok!", 
             GATEWAY_INFO_FILE);
  __LEAVE_FUNCTION
}

void Setting::load_share_memory_info() {
  __ENTER_FUNCTION
    load_share_memory_info_only();
    load_share_memory_info_reload();
  __LEAVE_FUNCTION
}

void Setting::load_share_memory_info_only() {
#if defined(_SHAREMEMORY)
  __ENTER_FUNCTION
    pf_file::Ini share_memory_info_ini(SHARE_MEMORY_INFO_FILE);
    share_memory_info_.obj_count = 
      share_memory_info_ini.read_uint16("Key", "KeyCount");
    share_memory_info_.data = 
      new share_memory_data_t[share_memory_info_.obj_count];
    uint32_t i;
    for (i = 0; i < share_memory_info_.obj_count; ++i) {
      char key[256];
      char type[256];
      memset(key, '\0', sizeof(key));
      memset(type, '\0', sizeof(type));
      snprintf(key, sizeof(key) - 1, "Key%d", i);
      snprintf(type, sizeof(type) - 1, "Type%d", i);
      share_memory_info_.data[i].key = 
        share_memory_info_ini.read_uint32("Key", key);
      share_memory_info_.data[i].type = 
        share_memory_info_ini.read_uint8("Key", type);
    }
    share_memory_info_ini.readstring("System", 
                                     "DBIP", 
                                     share_memory_info_.db_ip, 
                                     sizeof(share_memory_info_.db_ip) - 1);
    share_memory_info_.db_port = 
      share_memory_info_ini.read_uint16("System", "DBPort");
    share_memory_info_ini.readstring(
        "System", 
        "DBConnectionOrDBName", 
        share_memory_info_.db_connection_ordbname, 
        sizeof(share_memory_info_.db_connection_ordbname) - 1);
    share_memory_info_ini.readstring("System",
                                     "DBUser", 
                                     share_memory_info_.db_user, 
                                     sizeof(share_memory_info_.db_user) - 1);
    share_memory_info_ini.readstring("System",
                                     "DBPassword",
                                     share_memory_info_.db_password,
                                     sizeof(share_memory_info_.db_password) - 1);
    share_memory_info_.db_connectortype = static_cast<dbconnector_type_t>(
      share_memory_info_ini.read_int8("System", "DBConnectorType"));
    share_memory_info_.center_data_save_interval = 
      share_memory_info_ini.read_uint32("System", "WorldDataSaveInterval");
    share_memory_info_.player_data_save_interval = 
      share_memory_info_ini.read_uint32("System", "HumanDataSaveInterval");
    share_memory_info_.type = 
      share_memory_info_ini.read_uint8("System", "Type");
    share_memory_info_.encrypt_dbpassword = 
      share_memory_info_ini.read_bool("System", "EncryptDBPassword");
    if (share_memory_info_.encrypt_dbpassword) {
      char temp[DB_PASSWORD_LENGTH] = {0};
      pf_base::string::safecopy(temp, share_memory_info_.db_password, sizeof(temp));
      memset(share_memory_info_.db_password, 0, sizeof(share_memory_info_.db_password));
      pf_base::string::decrypt(temp, 
                               share_memory_info_.db_password, 
                               sizeof(share_memory_info_.db_password) - 1);
    }
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s only ... ok!", 
             SHARE_MEMORY_INFO_FILE);
  __LEAVE_FUNCTION
#endif
}

void Setting::load_share_memory_info_reload() {
#if defined(_SHAREMEMORY)
  __ENTER_FUNCTION
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s reload ... ok!", 
             SHARE_MEMORY_INFO_FILE);
  __LEAVE_FUNCTION
#endif
}

void Setting::load_machine_info() {
  __ENTER_FUNCTION
#ifndef _GATEWAY
    load_machine_info_only();
    load_machine_info_reload();
#endif
  __LEAVE_FUNCTION
}

void Setting::load_machine_info_only() {
  __ENTER_FUNCTION
    pf_file::Ini machine_info_ini(MACHINE_INFO_FILE);
    machine_info_.count = 
      machine_info_ini.read_uint16("System", "MachineNumber");
    machine_info_.data = new machine_data_t[machine_info_.count];
    memset(machine_info_.data, 0, sizeof(machine_info_.data));
    uint32_t i;
    for (i = 0; i < machine_info_.count; ++i) {
      char section[256];
      memset(section, '\0', sizeof(section));
      snprintf(section, sizeof(section) - 1, "Machine%d", i);
      machine_info_.data[i].id = 
        machine_info_ini.read_int16(static_cast<const char*>(section), 
                                    "MachineID");
    }
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s only ... ok!", 
             MACHINE_INFO_FILE);
  __LEAVE_FUNCTION
}

void Setting::load_machine_info_reload() {
  __ENTER_FUNCTION
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s reload ... ok!", 
             MACHINE_INFO_FILE);
  __LEAVE_FUNCTION
}

void Setting::load_server_info() {
  __ENTER_FUNCTION
#ifndef _GATEWAY
    load_server_info_only();
    load_server_info_reload();
#endif
  __LEAVE_FUNCTION
}

void Setting::load_server_info_only() {
  __ENTER_FUNCTION
    pf_file::Ini server_info_ini(SERVER_INFO_FILE);
    server_info_.count = server_info_ini.read_uint16("System", "ServerNumber");
    server_info_.net_connectionmax = 
      server_info_ini.read_uint16("System", "NetConnectionMax");
    server_info_.data = new server_data_t[server_info_.count];
    memset(server_info_.data, 0, sizeof(server_info_.data));
    uint32_t i;
    for (i = 0; i < server_info_.count; ++i) {
      char section[256];
      memset(section, '\0', sizeof(section));
      snprintf(section, sizeof(section) - 1, "Server%d", i);
      const char *kSection = static_cast<const char*>(section);
      server_info_.data[i].id = 
        server_info_ini.read_int16(kSection, "ServerID");
      server_info_.data[i].machine_id = 
        server_info_ini.read_int16(kSection, "MachineID");
      server_info_ini.readstring(kSection, 
                                 "IP", 
                                 server_info_.data[i].ip, 
                                 sizeof(server_info_.data[i].ip) - 1);
      server_info_.data[i].port = 
        server_info_ini.read_uint16(kSection, "Port");
      server_info_.data[i].type = server_info_ini.read_int8(kSection, "Type");
      //not active proxy
      server_info_.data[i].share_memory_key.human =  
        server_info_ini.read_uint32(kSection, "HumanShareMemoryKey");
      server_info_.data[i].share_memory_key.player_shop = 
        server_info_ini.read_uint32(kSection, "PlayShopShareMemoryKey");
      server_info_.data[i].share_memory_key.item_serial = 
        server_info_ini.read_uint32(kSection, "ItemSerialShareMemoryKey");
      server_info_.data[i].share_memory_key.commision_shop = 
        server_info_ini.read_uint32(kSection, "CommisionShopShareMemoryKey");
      server_info_.data[i].enable_share_memory = 
        server_info_ini.read_bool(kSection, "EnableShareMemory");
    }
    server_info_ini.readstring("Center", 
                               "IP", 
                               server_info_.center_data.ip, 
                               sizeof(server_info_.center_data.ip) - 1);
    server_info_.center_data.port = 
      server_info_ini.read_uint16("Center", "Port");
    for (i = 0; i < server_info_.count; ++i) {
      int16_t server_id = server_info_.data[i].id;
      Assert(server_id != ID_INVALID && server_id <= OVER_SERVER_MAX);
      Assert(-1 == server_info_.hash_server[server_id]);
      server_info_.hash_server[server_id] = static_cast<int16_t>(i);
    }
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s only ... ok!", 
             SERVER_INFO_FILE);
  __LEAVE_FUNCTION
}

void Setting::load_server_info_reload() {
  __ENTER_FUNCTION
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s reload ... ok!", 
             SERVER_INFO_FILE);
  __LEAVE_FUNCTION
}

void Setting::load_scene_info() {
  __ENTER_FUNCTION
    load_scene_info_only();
    load_scene_info_reload();
  __LEAVE_FUNCTION
}

void Setting::load_scene_info_only() {
#ifdef _SERVER
  __ENTER_FUNCTION
    pf_file::Ini scene_info_ini(SCENE_INFO_FILE);
    scene_info_.count = scene_info_ini.read_uint16("System", "SceneNumber");
    scene_info_.data = new scene_data_t[scene_info_.count];
    memset(scene_info_.data, 0, sizeof(scene_info_.data));
    uint32_t i;
    for (i = 0; i < scene_info_.count; ++i) {
      char section[256];
      memset(section, '\0', sizeof(section));
      snprintf(section, sizeof(section) - 1, "Scene%d", i);
      const char *kSection = static_cast<const char*>(section);
      scene_info_.data[i].id = static_cast<int16_t>(i);
      scene_info_.data[i].thread_index = 
        scene_info_ini.read_int16(kSection, "ThreadIndex");
      scene_info_.data[i].client_resource_index = 
        scene_info_ini.read_int16(kSection, "ClientResourceIndex");
      scene_info_ini.readstring(kSection, 
                                "Name", 
                                scene_info_.data[i].name, 
                                sizeof(scene_info_.data[i].name) - 1);
      scene_info_ini.readstring(kSection, 
                                "File", 
                                scene_info_.data[i].file, 
                                sizeof(scene_info_.data[i].file) - 1);
      scene_info_.data[i].server_id = 
        scene_info_ini.read_int16(kSection, "ServerID");
      scene_info_.data[i].type = scene_info_ini.read_int8(kSection, "Type");
      scene_info_.data[i].pvp_ruler = 
        scene_info_ini.read_uint16(kSection, "PvpRuler");
      scene_info_.data[i].begin_plus = 
        scene_info_ini.read_uint32(kSection, "BeginPlus");
      scene_info_.data[i].plus_client_resource_index = 
        scene_info_ini.read_int16(kSection, "PlusClientResourceIndex");
      scene_info_.data[i].end_plus = 
        scene_info_ini.read_uint32(kSection, "EndPlus");
      scene_info_.data[i].relive = scene_info_ini.read_bool(kSection, "Relive");
    }
    for (i = 0; i < scene_info_.count; ++i) {
      int16_t scene_id = scene_info_.data[i].id;
      Assert(scene_id != ID_INVALID && scene_id < SCENE_MAX);
      Assert(-1 == scene_info_.scene_hash[i]);
      scene_info_.scene_hash[scene_id] = static_cast<int16_t>(i);
    }
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s only ... ok!", 
             SCENE_INFO_FILE);
  __LEAVE_FUNCTION
#endif
}

void Setting::load_scene_info_reload() {
#if _SERVER
  __ENTER_FUNCTION
    SLOW_LOG("setting", 
             "[common] (Setting::load) %s reload ... ok!", 
             SCENE_INFO_FILE);
  __LEAVE_FUNCTION
#endif
}

void Setting::load_copy_scene_info() {
  __ENTER_FUNCTION
    load_copy_scene_info_only();
    load_copy_scene_info_reload();
  __LEAVE_FUNCTION
}

void Setting::load_copy_scene_info_only() {
#ifdef _SERVER
  //do nothing
#endif
}

void Setting::load_copy_scene_info_reload() {
#ifdef _SERVER
  //do nothing
#endif
}

int16_t Setting::get_server_id_by_scene_id(int16_t id) const {
#ifdef _SERVER
  __ENTER_FUNCTION
    Assert(id >= 0);
    Assert(id < scene_info_.count);
    return scene_info_.data[scene_info_.scene_hash[id]].server_id;
  __LEAVE_FUNCTION
    return -1;
#else
  USE_PARAM(id);
  return -1;
#endif
}

int16_t Setting::get_server_id_by_share_memory_key(uint32_t key) const {
  __ENTER_FUNCTION
    int16_t result = -1;
    Assert(key > 0);
    uint32_t i;
    for (i = 0; i < server_info_.count; ++i) {
      if (server_info_.data[i].enable_share_memory) {
        if (server_info_.data[i].share_memory_key.human == key ||
            server_info_.data[i].share_memory_key.player_shop == key ||
            server_info_.data[i].share_memory_key.item_serial == key ||
            server_info_.data[i].share_memory_key.commision_shop == key) {
          result = server_info_.data[i].id;
          break;
        }
      }
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

#endif /* } __SERVER__ */

//class end --

} //namespace common
