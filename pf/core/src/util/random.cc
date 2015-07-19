#include "pf/base/log.h"
#include "pf/util/random.h"

pf_util::RandomGenerator *g_random_generator = NULL;

template<> pf_util::RandomGenerator 
  *pf_base::Singleton<pf_util::RandomGenerator>::singleton_ = NULL;

namespace pf_util {

RandomGenerator *RandomGenerator::getsingleton_pointer() {
  return singleton_;
}

RandomGenerator &RandomGenerator::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

RandomGenerator::RandomGenerator(uint32_t seed) {
  __ENTER_FUNCTION
    srand(seed);
  __LEAVE_FUNCTION
}

RandomGenerator::~RandomGenerator() {
  //do nothing
}

uint32_t RandomGenerator::randuint32() {
  __ENTER_FUNCTION
    uint32_t a = (rand() & 0x00000FFF);
    uint32_t b = ((rand() & 0x00000FFF) << 12);
    uint32_t c = ((rand() & 0x000000FF) << 24);
    uint32_t d = a + b + c;
    return d;
  __LEAVE_FUNCTION
    return 0;
}

double RandomGenerator::randdouble() {
  __ENTER_FUNCTION
    double result = 
      static_cast<double>(randuint32()) / static_cast<double>(kRandmonMax);
    return result;
  __LEAVE_FUNCTION
    return 0.0;
}

uint32_t RandomGenerator::getrand(int32_t start, int32_t end) {
  __ENTER_FUNCTION
    Assert((end - start + 1) > 0);
    if (!RANDOM_GENERATOR_POINTER) {
      SLOW_ERRORLOG("util", 
                    "[util] RandomGenerator::getrand error, "
                    "RANDOM_GENERATOR_POINTER is NULL");
      return 0;
    };
    uint32_t result = 
      (RANDOM_GENERATOR_POINTER->randuint32() % (end - start + 1)) + start;
    return result;
  __LEAVE_FUNCTION
    return 0;
}

} //namespace pf_util
