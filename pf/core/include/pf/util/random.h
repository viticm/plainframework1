/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id random.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/19 18:09
 * @uses util of random
 *       cn: 随机相关工具
**/
#ifndef PF_UTIL_RANDOM_H_
#define PF_UTIL_RANDOM_H_

#include "pf/util/config.h"
#include "pf/base/singleton.h"

namespace pf_util {

template <uint32_t T_size>
class RandomTable {
  
 public:
   RandomTable() {
     srand((unsigned)time(NULL));
     for (int32_t i = 0; i < T_size; ++i)
       record_[i] = i;
     reset();
   };
   ~RandomTable() {}

 public:
   uint32_t getrand() {
      if (currentpoint_ >= T_size) reset();
      return record_[currentpoint_++];
   };

 protected:
   uint32_t record_[T_size];
   uint32_t currentpoint_;

 protected:
   void reset() {
     for (int32_t i = 0; i < T_size; ++i) {
       uint32_t from = rand() % T_size;
       uint32_t to = rand() % T_size;
       uint32_t temp = record_[to];
       record_[to] = record_[from];
       record_[from] = temp;
     }
     currentpoint_ = 0;
   };

};

class RandomGenerator : public pf_base::Singleton<RandomGenerator> {

 public:
   RandomGenerator(const uint32_t seed = 0);
   ~RandomGenerator();

 public:
   static RandomGenerator &getsingleton();
   static RandomGenerator *getsingleton_pointer();

 public:
   uint32_t randuint32();
   double randdouble();
   static uint32_t getrand(int32_t start, int32_t end);

 private:
   static const uint32_t k32BitLongMax = 0xFFFFFFFFLU;
   static const uint32_t kRandmonMax = k32BitLongMax;

};

}; //namespace pf_util

extern pf_util::RandomGenerator *g_random_generator;

#define RANDOM_GENERATOR_POINTER \
  pf_util::RandomGenerator::getsingleton_pointer()

#endif //PF_UTIL_RANDOM_H_
