#include "pf/base/log.h"
#include "pf/file/database.h"
//现在只用到lua脚本，设计中已经预留可以使用其他脚本，这里先写死
#include "pf/script/lua/system.h"
#include "pf/script/cache/manager.h"

template <> 
pf_script::cache::Manager 
  *pf_base::Singleton<pf_script::cache::Manager>::singleton_ = NULL;

namespace pf_script {

namespace cache {

Manager *Manager::getsingleton_pointer() {
  return singleton_;
}

Manager &Manager::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

Manager::Manager() {
  __ENTER_FUNCTION
    isinit_ = false;
    count_ = 0;
    filedata_set_ = NULL;
  __LEAVE_FUNCTION
}

Manager::~Manager() {
  __ENTER_FUNCTION
    clear();
  __LEAVE_FUNCTION
}

const char *Manager::get_rootpath() {
  __ENTER_FUNCTION
    if (SCRIPT_LUASYSTEM_POINTER) {
      return SCRIPT_LUASYSTEM_POINTER->get_rootpath();
    }
    return SCRIPT_ROOT_PATH_DEFAULT;
  __LEAVE_FUNCTION
    return NULL;
}

void Manager::clear() {
  __ENTER_FUNCTION
    isinit_ = false;
    count_ = 0;
    SAFE_DELETE_ARRAY(filedata_set_);
  __LEAVE_FUNCTION
}

bool Manager::init(const char *filename, bool force_arraymode) {
  __ENTER_FUNCTION
    using namespace pf_file;
    USE_PARAM(force_arraymode); //扩展后去掉
    enum {kFileDataIndex = 0, kFileDataName};
    Database db(0); 
    bool result = db.open_from_txt(filename);
    if (!result) {
      Assert(false && "[script][cache] (Manager::init) error open file");
      return false;
    }
    int32_t tablecount = db.get_record_number();
    if (!read_tablesize_check(tablecount)) return false;
    count_ = tablecount;
    filedata_set_ = new Base[count_];
    Assert(filedata_set_);
    for (int32_t i = 0; i < tablecount; ++i) {
      char _filename[FILENAME_MAX] = {0};
      int32_t id = db.search_position(i, kFileDataIndex)->int_value;
      const char *kFileName = 
        db.search_position(i, kFileDataName)->string_value;
      snprintf(_filename, 
               sizeof(_filename) - 1, 
               "%s%s", 
               get_rootpath(), 
               kFileName);
      FILE *fp = fopen(_filename, "r");
      if (!fp) {
        SLOW_ERRORLOG(SCRIPT_MODULENAME,
                      "[script.cache] (Manager::init) script file [%d:%s]"
                      " not exist!",
                      id,
                      _filename);
      }
      else {
        fclose(fp);
      }
      filedata_set_[i].init(id, kFileName, NULL);
    }
    sort_filedata_array();
    isinit_ = true;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Manager::read_tablesize_check(int32_t tablecount) {
  __ENTER_FUNCTION
    if (tablecount <= 0) {
      Assert(false && 
          "[script.cache] (Manager::read_tablesize_check)"
          " error, tablecount <= 0");
      return false;
    }
    if (tablecount > SCRIPT_CACHE_TABLE_DATA_MAX) {
      Assert(false && 
          "[script.cache] (Manager::read_tablesize_check)"
          " error, tablecount > SCRIPT_CACHE_TABLE_DATA_MAX");
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Manager::sort_filedata_array() {
  __ENTER_FUNCTION
    qsort(filedata_set_, 
          count_, 
          sizeof(Base), 
          (int32_t(*) (const void *, const void *))compare_with_filedata_id);
  __LEAVE_FUNCTION
}

int32_t Manager::compare_with_filedata_id(const void *arg1, const void *arg2) {
  __ENTER_FUNCTION
    int32_t serial1, serial2;
    serial1 = ((Base *)arg1)->getid();
    serial2 = ((Base *)arg2)->getid();
    if (serial1 > serial2) {
      return 1;
    }
    else if (serial1 < serial2) {
      return -1;
    }
    return 0;
  __LEAVE_FUNCTION
    return 0;
}

Base *Manager::get_filedata(uint32_t id) {
  __ENTER_FUNCTION
    Base cache_data;
    cache_data.init(id, "for bsearch", NULL);
    Base *result = reinterpret_cast<Base *>(
        bsearch(
          &cache_data, 
          filedata_set_, 
          count_, 
          sizeof(Base), 
          (int32_t(*) (const void *, const void *))compare_with_filedata_id));
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

const Base *Manager::get_const_filedata(uint32_t id) const {
  __ENTER_FUNCTION
    const Base *result = NULL;
    result = const_cast<Manager *>(this)->get_filedata(id);
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

Base *Manager::get_filedata_byindex(int32_t index) {
  __ENTER_FUNCTION
    Assert(index >= 0 && index <= count_);
    return &filedata_set_[index];
  __LEAVE_FUNCTION
    return NULL;
}

int32_t Manager::getcount() const {
  return count_;
}

bool Manager::isinit() const {
  return isinit_;
}

} //namespace cache

} //namespace pf_script
