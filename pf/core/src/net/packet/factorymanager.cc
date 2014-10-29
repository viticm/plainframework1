#include "pf/base/log.h"
#include "pf/net/packet/base.h"
#include "pf/net/packet/factorymanager.h"

pf_net::packet::FactoryManager* g_packetfactory_manager = NULL;

template<> 
pf_net::packet::FactoryManager *pf_base::Singleton<
  pf_net::packet::FactoryManager>::singleton_ = NULL;

namespace pf_net {

namespace packet {

FactoryManager* FactoryManager::getsingleton_pointer() {
  return singleton_;
}

FactoryManager& FactoryManager::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

FactoryManager::FactoryManager() {
  __ENTER_FUNCTION
    isinit_ = false;
    factories_ = NULL;
    factorycount_ = 0;
    size_ = 0;
    function_registerfactories_ = NULL;
    function_isvalid_packetid_ = NULL;
  __LEAVE_FUNCTION
}

FactoryManager::~FactoryManager() {
  __ENTER_FUNCTION
    Assert(factories_ != NULL);
    uint16_t i;
    for (i = 0; i < size_; ++i) {
      SAFE_DELETE(factories_[i]);
    }
    SAFE_DELETE(packet_alloccount_);
  __LEAVE_FUNCTION
}

bool FactoryManager::isinit() const {
  return isinit_;
}

bool FactoryManager::init() {
  __ENTER_FUNCTION
    if (isinit()) return true;
    Assert(size_ > 0);
    if (!function_isvalid_packetid_ || !function_registerfactories_) {
      SLOW_ERRORLOG(
          NET_MODULENAME, 
          "[net.packet] (FactoryManager::init) error,"
          " the register factories and is valid packet id"
          " function pointer is NULL");
      return false;
    }
    factories_ = new Factory * [size_];
    Assert(factories_);
    packet_alloccount_ = new uint32_t[size_];
    Assert(packet_alloccount_);
    idindexs_.init(size_); //ID索引数组初始化
    uint16_t i;
    for (i = 0; i < size_; ++i) {
      factories_[i] = NULL;
      packet_alloccount_[i] = 0;
    }
    if (!(*function_registerfactories_)()) return false;
    isinit_ = true;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void FactoryManager::setsize(uint16_t size) {
  size_ = size;
}

uint16_t FactoryManager::getsize() const {
  return size_;
}

void FactoryManager::set_function_registerfactories(
    function_registerfactories function) {
  function_registerfactories_ = function;
}

void FactoryManager::set_function_isvalid_packetid(
    function_isvalid_packetid function) {
  function_isvalid_packetid_ = function;
}

Base *FactoryManager::createpacket(uint16_t packetid) {
  __ENTER_FUNCTION
    bool isfind = idindexs_.isfind(packetid);
    uint16_t index = idindexs_.get(packetid);
    if (!isfind || NULL == factories_[index]) {
      Assert(false);
      return NULL;
    }
    Base* packet = NULL;
    lock();
    try {
      packet = factories_[index]->createpacket();
      ++(packet_alloccount_[index]);
    } catch(...) {
      packet = NULL;
    }
    unlock();
    return packet;
  __LEAVE_FUNCTION
    return NULL;
}

uint32_t FactoryManager::getpacket_maxsize(uint16_t packetid) {
  __ENTER_FUNCTION
    uint32_t result = 0;
    bool isfind = idindexs_.isfind(packetid);
    uint16_t index = idindexs_.get(packetid);
    if (!isfind || NULL == factories_[index]) {
      char temp[FILENAME_MAX] = {0};
      snprintf(temp, 
               sizeof(temp) - 1, 
               "packetid: %d not register in factory!", 
               packetid);
      AssertEx(false, temp);
      return result;
    }
    lock();
    result = factories_[index]->get_packet_maxsize();
    unlock();
    return result;
  __LEAVE_FUNCTION
    return 0;
}

void FactoryManager::removepacket(Base* packet) {
  __ENTER_FUNCTION
    if (NULL == packet) {
      Assert(false);
      return;
    }
    uint16_t packetid = packet->getid();
    lock();
    try {
      SAFE_DELETE(packet);
      --(packet_alloccount_[packetid]);
    } catch(...) {
      unlock();
    }
    unlock();
  __LEAVE_FUNCTION
}

void FactoryManager::lock() {
  lock_.lock();
}

void FactoryManager::unlock() {
  lock_.unlock();
}

void FactoryManager::addfactory(Factory *factory) {
  __ENTER_FUNCTION
    bool isfind = idindexs_.isfind(factory->get_packetid());
    uint16_t index = 
      isfind ? idindexs_.get(factory->get_packetid()) : factorycount_;
    if (factories_[index] != NULL) {
      Assert(false);
      return;
    }
    if (!isfind) {
      ++factorycount_;
      idindexs_.add(factory->get_packetid(), index);
    }
    factories_[index] = factory;
  __LEAVE_FUNCTION
}

bool FactoryManager::isvalid_packetid(uint16_t id) {
  bool result = false;
  __ENTER_FUNCTION
    if (!function_isvalid_packetid_) return false;
    result = (*function_isvalid_packetid_)(id);
    return result;
  __LEAVE_FUNCTION
    return result;
}

} //namespace packet

} //namespace pf_net
