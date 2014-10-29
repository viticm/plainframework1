/**
 * PAP Engine ( https://github.com/viticm/plainframework )
 * $Id interface.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/10/11 11:54
 * @uses your description
*/
#ifndef PAK_INTERFACE_H_
#define PAK_INTERFACE_H_

#include "pak/config.h"

extern int32_t g_pak_compressiontype;

namespace pak {

PF_API archive_t *archivecreate(const char *filename, 
                                uint64_t &result, 
                                uint64_t hashtable_size, 
                                uint64_t usetype); //if type is normal, the hashtable_size will enough max

PF_API archive_t *archiveopen(const char *filename, 
                              uint64_t &result, 
                              bool writeflag = true);

PF_API bool archiveclose(archive_t *archive);

PF_API bool fileadd(archive_t *archive, 
                    const char *realname, 
                    const char *aliasname,
                    uint64_t flags,
                    uint64_t quality,
                    uint64_t filetype);

PF_API bool fileadd_frommemory(archive_t *archive,
                               const char *aliasname,
                               const char *content,
                               uint64_t size,
                               uint64_t flags,
                               uint64_t quality,
                               uint64_t filetype);

PF_API bool fileremove(archive_t *archive, const char *aliasname);

PF_API bool filerename(archive_t *archive, const char *oldname, const char* newname);

PF_API file_t *fileopen(archive_t *archive, const char *name, uint64_t &result);

PF_API uint64_t fileclose(file_t *file);

PF_API bool fileread(file_t *file, 
                     void *buffer, 
                     uint64_t toread, 
                     uint64_t *readed = NULL);

PF_API uint64_t filesize(file_t *file);

PF_API uint64_t file_setpointer(file_t *file, int64_t offset, uint64_t method);

PF_API uint64_t fileposition(file_t *file);

PF_API uint64_t fileskip(file_t *file, int64_t count);

PF_API uint64_t fileseek(file_t *file, int64_t count);

PF_API uint64_t filetell(file_t *file);

PF_API bool fileeof(file_t *file);

PF_API search_t *search_firstfile(archive_t *archive, 
                                  const char *mask, 
                                  findfile_data_t *findfile_data);

PF_API bool search_nextfile(search_t *find, findfile_data_t *findfile_data);

PF_API bool search_close(search_t *find);

PF_API uint64_t set_compressiontype(int32_t type);

PF_API uint64_t get_compressiontype();

}; //namespace pak

#endif //PAK_INTERFACE_H_
