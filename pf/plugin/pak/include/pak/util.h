/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id util.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/08/19 14:30
 * @uses pak util module
*/
#ifndef PAK_UTIL_H_
#define PAK_UTIL_H_

#include "pak/config.h"

extern int32_t g_pakerror;
extern int32_t g_pak_data_compressiontype;

namespace pak {

namespace util {

void set_lasterror(int32_t error);
int32_t get_lasterror();
const char *get_last_errorstr(int32_t error);
void freearchive(archive_t *&archive);
bool isvalid_archivepointer(archive_t *archive);
void freefile(file_t *&file);
bool isvalid_filepointer(file_t *file);
void freeattributes(attribute_t *attributes);
int32_t save_listfile(archive_t *archive, bool internal);
int32_t save_listfile(archive_t *archive);
int32_t savetables(archive_t *archive);
uint64_t save_attributefile(archive_t *archive, bool internal);
uint64_t save_attributefile(archive_t *archive);
int32_t addfile_toarchive(archive_t *archive, 
                          void *fp, 
                          const char *archivedname, 
                          uint64_t flag, 
                          uint64_t quality, 
                          int32_t filetype, 
                          bool *replace);
int32_t addfile_toarchive(archive_t *archive, 
                          const char *content, 
                          uint64_t size,
                          const char *archivedname,
                          uint64_t flag, 
                          uint64_t quality, 
                          int32_t filetype, 
                          bool *replace);
int32_t internal_updatefile(archive_t *archive,
                            void *fp, 
                            file_t *file,
                            const char *archivedname,
                            uint64_t flag, 
                            uint64_t quality, 
                            int32_t filetype,
                            bool *replace);
uint64_t internal_addfile(archive_t *archive, const char *filename);
uint64_t internal_readfile(file_t *file, 
                           uint64_t fileposition, 
                           char *buffer, 
                           uint64_t toread);
bool isvalid_searchpointer(search_t *search);

}; //namespace util

}; //namespace pak

#endif //PAK_UTIL_H_
