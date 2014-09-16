/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id atomic.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/04 18:03
 * @uses the base atomic functions
 *       cn: linux 内核操作函数
 *           url: http://blog.csdn.net/vividonly/article/details/6599502
 */
#ifndef PF_BASE_ATOMIC_H_
#define PF_BASE_ATOMIC_H_

#if __LINUX__ /* { */

#ifdef __x86_64__
typedef struct { volatile int64_t counter; } atomic_t;
#else
typedef struct { volatile int32_t counter; } atomic_t;
#endif

typedef atomic_t atword_t;
#define initword(v,i) ((v).counter = (i))
#elif __WINDOWS__ /* }{ */
typedef volatile long atword_t;
#define initword(v,i) ((v) = (i))
#endif /* } */

#if __LINUX__

#define atomic_read(v) ((v)->counter)
#define atomic_set(v,i) ((v)->counter = (i))

#define LOCK_PREFIX "lock ;" 

#ifdef __x86_64__
static inline void atomic_add(int64_t i, atomic_t *v) {
  asm volatile(LOCK_PREFIX "addq %1,%0"
               : "=m" (v->counter)
               : "er" (i), "m" (v->counter));
}

static inline void atomic_sub(int32_t i, atomic_t *v) {
  asm volatile(LOCK_PREFIX "subq %1,%0"
               : "=m" (v->counter)
               : "ir" (i), "m" (v->counter));
}

static inline int32_t atomic_sub_and_test(long i, atomic_t *v) {
  uint8_t c;
  asm volatile(LOCK_PREFIX "subq %2,%0; sete %1"
               :"=m" (v->counter), "=qm" (c)
               :"ir" (i), "m" (v->counter)
               : "memory");
  return c;
}

static inline void atomic_inc(atomic_t *v) {
  asm volatile(LOCK_PREFIX "incq %0"
               :"=m" (v->counter)
               :"m" (v->counter));
}

static inline void atomic_dec(atomic_t *v) {
  asm volatile(LOCK_PREFIX "decq %0"
               :"=m" (v->counter) 
               :"m" (v->counter));
}

static inline int32_t atomic_dec_and_test(atomic_t *v) {
  uint8_t c;
  asm volatile(LOCK_PREFIX "decq %0; sete %1"
               :"=m" (v->counter), "=qm" (c)
               :"m" (v->counter)
               : "memory"); 
  return c != 0;
}

static inline int32_t atomic_inc_and_test(atomic_t *v) {
  uint8_t c;
  asm volatile(LOCK_PREFIX "incq %0; sete %1"
               :"=m" (v->counter), "=qm" (c)
               :"m" (v->counter)
               : "memory"); 
  return c != 0;
}

#else

static inline void atomic_add(int32_t i, atomic_t *v) {
  asm volatile(LOCK_PREFIX "addl %1,%0" : "+m"(v->counter) : "ir"(i));
}

static inline void atomic_sub(int32_t i, atomic_t *v) {
  asm volatile(LOCK_PREFIX "subl %1,%0" : "+m"(v->counter) : "ir"(i));
}

static inline int32_t atomic_sub_and_test(int i, atomic_t *v) {
  uint8_t c;
  asm volatile(LOCK_PREFIX "subl %2,%0; sete %1" : "+m"(v->counter), 
               "=qm"(c) : "ir"(i) : "memory");
  return c;
}

static inline void atomic_inc(atomic_t *v) {
  asm volatile(LOCK_PREFIX "incl %0" : "+m"(v->counter));
}

static inline void atomic_dec(atomic_t *v) {
  asm volatile(LOCK_PREFIX "decl %0" : "+m"(v->counter));
}

static inline int32_t atomic_dec_and_test(atomic_t *v) {
  uint8_t c;
  asm volatile(LOCK_PREFIX "decl %0; sete %1" : "+m"(v->counter), 
               "=qm"(c) : : "memory");
  return c != 0;
}

static inline int32_t atomic_inc_and_test(atomic_t *v) {
  uint8_t c;
  asm volatile(LOCK_PREFIX "incl %0; sete %1" : "+m"(v->counter), 
               "=qm"(c) : : "memory");
  return c != 0;
}

#endif //__x86_64__

#endif //__LINUX__

#endif //PF_BASE_ATOMIC_H_
