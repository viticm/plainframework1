/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id compress.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/18 20:43
 * @uses pak compress module
 */
#ifndef PAK_COMPRESS_H_
#define PAK_COMPRESS_H_

#include "pak/config.h"

namespace pak {

namespace compress {

int32_t pklib(char *out, 
              int32_t *outsize, 
              char *in, 
              int32_t insize, 
              int32_t *type, 
              int32_t level);

int32_t de_pklib(char *out, int32_t *outsize, char *in, int32_t insize);

int32_t smart(char *out, 
              int32_t *outsize, 
              char *in, 
              int32_t insize, 
              int32_t compressions,
              int32_t type, 
              int32_t level);

int32_t de_smart(char *out, int32_t *outsize, char *in, int32_t insize);

int32_t zlib(char *out, 
             int32_t *outsize, 
             char *in, 
             int32_t insize, 
             int32_t *type, 
             int32_t level); 

int32_t de_zlib(char *out, int32_t *outsize, char *in, int32_t insize);

}; //namespace compress

}; //namespace pak

#endif //PAK_COMPRESS_H_
