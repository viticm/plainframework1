/**
 * PAP Engine ( https://github.com/viticm/plainframework )
 * $Id listfile.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/10/13 14:32
 * @uses pak list file module(save pak file list informations)
*/
#ifndef PAK_LISTFILE_H_
#define PAK_LISTFILE_H_

#include "pak/config.h"

namespace pak {

uint64_t listfile_create(archive_t *archive);
uint64_t listfile_normaladd(archive_t *archive, const char *filename);
uint64_t listfile_patchadd(archive_t *archive, const char *filename);
uint64_t listfile_nodeadd(archive_t *archive, const char *filename);
uint64_t listfile_noderemove(archive_t *archive, const char *filename);
uint64_t listfile_noderename(archive_t *archive, 
                             const char *old_filename, 
                             const char *new_filename);

uint64_t attributefile_create(archive_t *archive);
uint64_t attributefile_normalload(archive_t *archive);
uint64_t attributefile_patchload(archive_t *archive);

}; //namespace pak

#endif //PAK_LISTFILE_H_
