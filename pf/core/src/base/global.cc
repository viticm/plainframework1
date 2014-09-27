#include "pf/base/string.h"
#include "pf/base/global.h"

namespace pf_base {

namespace global {

char net_modulename[FILENAME_MAX] = {0};
char script_modulename[FILENAME_MAX] = {0};
char performance_modulename[FILENAME_MAX] = {0};
char db_modulename[FILENAME_MAX] =  {0};
char engine_modulename[FILENAME_MAX] = {0};
char applicationname[FILENAME_MAX] = {0};
uint8_t applicationtype = 0;

const char *get_net_modulename() {
  __ENTER_FUNCTION
    if (strlen(net_modulename) > 0) return net_modulename;
    snprintf(net_modulename, 
             sizeof(net_modulename) - 1, 
             "net");
    return net_modulename;
  __LEAVE_FUNCTION
    return NULL;
}

const char *get_script_modulename() {
  __ENTER_FUNCTION
    if (strlen(script_modulename) > 0) return script_modulename;
    snprintf(script_modulename, 
             sizeof(script_modulename) - 1, 
             "script");
    return script_modulename;
  __LEAVE_FUNCTION
    return NULL;
}

const char *get_performanace_modulename() {
  __ENTER_FUNCTION
    if (strlen(performance_modulename) > 0) return performance_modulename;
    snprintf(performance_modulename, 
             sizeof(performance_modulename) - 1, 
             "performance");
    return performance_modulename;
  __LEAVE_FUNCTION
    return NULL;
}

const char *get_db_modulename() {
  __ENTER_FUNCTION
    if (strlen(db_modulename) > 0) return db_modulename;
    snprintf(db_modulename, 
             sizeof(db_modulename) - 1, 
             "db");
    return db_modulename;
  __LEAVE_FUNCTION
    return NULL;
}

const char *get_engine_modulename() {
  __ENTER_FUNCTION
    if (strlen(engine_modulename) > 0) return engine_modulename;
    snprintf(engine_modulename, 
             sizeof(engine_modulename) - 1, 
             "engine");
    return engine_modulename;
  __LEAVE_FUNCTION
    return NULL;
}

const char *get_applicationname() {
  __ENTER_FUNCTION
    if (0 == strlen(applicationname)) {
      snprintf(applicationname,
               sizeof(applicationname) - 1,
               "%s",
               "unknown");
    }
    return applicationname;
  __LEAVE_FUNCTION
    return NULL;
}

void set_applicationname(const char *name) {
  string::safecopy(applicationname, name, sizeof(applicationname));  
}

uint8_t get_applicationtype() {
  return applicationtype;
}

void set_applicationtype(uint8_t type) {
  applicationtype = type;
}

}; //namespace global

} //namespace pf_base
