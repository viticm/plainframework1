#include "pak/encrypt.h"
#include "pak/compress.h"
#include "pak/util.h"
#include "pak/interface.h"
#include "pak/listfile.h"

#define PAK_LISTFILE_CACHESIZE 0x1000

namespace pak {

typedef struct listfile_cache_struct {
  file_t *file;
  char *mask;
  uint64_t filesize;
  uint64_t buffersize;
  uint64_t fileposition;
  char *begin;
  char *position;
  char *end;
  char buffer[1];
} listfile_cache_t;

//local functions
static int32_t reloadcache(listfile_cache_t* cache) {
  __ENTER_FUNCTION
    //Check if there is enough characters in the cache
    //If not, we have to reload the next block
    if (cache->position >= cache->end) {
      //If the cache is already at the end, do nothing more
      if (cache->fileposition + cache->buffersize >= cache->filesize) return 0;
      cache->fileposition += cache->buffersize;
      fileread(cache->file, 
               cache->buffer, 
               cache->buffersize, 
               &cache->buffersize);
      if (0 == cache->buffersize) return 0;
      //Set the buffer pointers
      cache->begin = cache->position = &cache->buffer[0];
      cache->end = cache->begin + cache->buffersize;
    }
    return static_cast<int32_t>(cache->buffersize);
  __LEAVE_FUNCTION
    return -1;
}

static size_t readline(listfile_cache_t *cache, char *line, int32_t charmax) {
  __ENTER_FUNCTION
    char *linebegin = line;
    char *lineend = line + charmax + 1;
__BEGINE_LOADING:
    
    //Skip newlines, spaces, tabs and another non-printable stuff
    while (cache->position < cache->end && *cache->position <= 0x20) 
      ++cache->position;
    //Copy the remaining characters
    while (cache->position < cache->end && line < lineend) {
      //If we have found a newline, stop loading
      if (0x0D == *cache->position || 0x0A == *cache->position) break;
      *line++ = *cache->position++;
    }
    //If we now need to reload the cache, do it 
    if (cache->position == cache->end) {
      if (reloadcache(cache) > 0) goto __BEGINE_LOADING;
    }
    *line = 0;
    size_t result = line - linebegin;
    return result;
  __LEAVE_FUNCTION
    return -1;
}

static bool listfile_cachefree(listfile_cache_t *cache) {
  __ENTER_FUNCTION
    if (cache != NULL) {
      if (cache->file != NULL) fileclose(cache->file);
      SAFE_FREE(cache->mask);
      SAFE_FREE(cache);
      return true;
    }
    return false;
  __LEAVE_FUNCTION
    return false;
}

uint64_t listfile_create(archive_t *archive) {
  __ENTER_FUNCTION
    uint64_t count = archive->header.hashtable_size;
    //The listfile should be NULL now 
    Assert(NULL == archive->listfiles);
    archive->listfiles = (filenode_t **)malloc(sizeof(filenode_t *) * count);
    Assert(archive->listfiles);
    if (NULL == archive->listfiles) return PAK_ERROR_NOT_ENOUGH_MEMORY;
    memset(archive->listfiles, 0xFF, sizeof(filenode_t *) * count);
    return PAK_ERROR_NONE;
  __LEAVE_FUNCTION
    return PAK_ERROR_UNKOWN;
}

uint64_t listfile_normaladd(archive_t *archive, const char *filename) {
  __ENTER_FUNCTION
    USE_PARAM(filename);
    listfile_cache_t *cache = NULL;
    uint64_t cachesize = 0;
    uint64_t _filesize = 0;
    size_t length = 0;
    file_t *file = NULL;
    char _filename[FILENAME_MAX] = {0};
    uint64_t error = PAK_ERROR_NONE;
    uint64_t file_savedlength = 1024 * 1024 * 2; //2mb
    uint64_t used = 0;
    uint64_t bytesread = 0;
    char *listfile_cache = (char *)malloc(sizeof(char) * file_savedlength);
    Assert(listfile_cache);
    if (NULL == listfile_cache) {
      error = PAK_ERROR_NOT_ENOUGH_MEMORY;
    } else {
      memset(listfile_cache, 0, file_savedlength);
    }
    char *listfile_compress = 
      (char *)malloc(sizeof(char) * (file_savedlength / 2 + 8));
    Assert(listfile_compress);
    if (NULL == listfile_compress) {
      error = PAK_ERROR_NOT_ENOUGH_MEMORY;
    } else {
      memset(listfile_compress, 0, file_savedlength / 2 + 8);
    }
    //Open the local/internal listfile
    if (PAK_ERROR_NONE == error) 
      file = fileopen(archive, PAK_LISTFILE_NAME, error);
    if (PAK_ERROR_NONE == error) {
      fileread(file, listfile_compress, file_savedlength / 2 + 8, &bytesread);
      if (bytesread != file_savedlength / 2 + 8) error = PAK_ERROR_FILE_CORRUPT;
      if (*(uint64_t *)listfile_compress != file_savedlength / 2 + 8) {
        error = PAK_ERROR_FILE_CORRUPT;
      } else {
        used = *((uint64_t *)listfile_compress + 1);
      }
    }
    int32_t outlength = static_cast<int32_t>(file_savedlength);
    if (PAK_ERROR_NONE == error) {
      compress::de_zlib(listfile_cache, 
                        &outlength, 
                        listfile_compress + 8, 
                        static_cast<int32_t>(used));
    }
    if (PAK_ERROR_NONE == error) _filesize = cachesize = outlength;
    if (PAK_ERROR_NONE == error) {
      //Try to allocate memory for the complete file. If it fails,
      //load the part of the file
      cache = (listfile_cache_t *)malloc(sizeof(listfile_cache_t) + cachesize);
      if (NULL == cache) {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      } else {
        memset(cache, 0, sizeof(listfile_cache_t) + cachesize);
      }
    }
    if (PAK_ERROR_NONE == error) {
      //Initialize the file cache
      memset(cache, 0, sizeof(listfile_cache_t));
      cache->file = file;
      cache->filesize = _filesize;
      cache->buffersize = cachesize;
      cache->fileposition = 0;
      //Fill the cache
      cache->begin = cache->position = &cache->buffer[0];
      cache->end = cache->begin + cache->buffersize;
      //Load the node tree
      while ((length = readline(cache, _filename, sizeof(_filename) - 1)) > 0)
        listfile_nodeadd(archive, _filename);
    }
    SAFE_FREE(listfile_compress);
    SAFE_FREE(listfile_cache);
    if (cache != NULL) listfile_cachefree(cache);
    return error;
  __LEAVE_FUNCTION
    return PAK_ERROR_UNKOWN;
}

uint64_t listfile_patchadd(archive_t *archive, const char *filename) {
  __ENTER_FUNCTION
    USE_PARAM(filename);
    listfile_cache_t *cache = NULL;
    uint64_t cachesize = 0;
    uint64_t _filesize = 0; //区分函数filesize
    size_t length = 0;
    file_t *file = NULL;
    char _filename[FILENAME_MAX] = {0};
    uint64_t error = PAK_ERROR_NONE;

    //Open the local/internal listfile 
    file = fileopen(archive, PAK_LISTFILE_NAME, error);
    
    return error;
    if (PAK_ERROR_NONE == error) {
      cachesize = _filesize = filesize(file);
      //Try to allocate memory for the complete file. If it fails,
      //load the part of the file
      cache = (listfile_cache_t *)malloc(sizeof(listfile_cache_t) + cachesize);
      if (NULL == cache) {
        cachesize = PAK_LISTFILE_CACHESIZE;
        cache = 
          (listfile_cache_t *)malloc(sizeof(listfile_cache_t) + cachesize);
      }
      if (NULL == cache) { 
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      } else {
        memset(cache, 0, sizeof(listfile_cache_t) + cachesize);
      }
    }

    if (PAK_ERROR_NONE == error) {
      //Initialize the file cache
      memset(cache, 0, sizeof(listfile_cache_t));
      cache->file = file;
      cache->filesize = _filesize;
      cache->buffersize = cachesize;
      cache->fileposition = 0;
      //Fill the cache
      fileread(file, cache->buffer, cache->buffersize, &cache->buffersize);
      //Initialize the pointers
      cache->begin = cache->position = &cache->buffer[0];
      cache->end = cache->begin + cache->buffersize;
      //Load the node tree
      while ((length = readline(cache, _filename, sizeof(_filename) - 1)) > 0)
        listfile_nodeadd(archive, _filename);
    }
    if (cache != NULL) listfile_cachefree(cache);
    return error;
  __LEAVE_FUNCTION
    return PAK_ERROR_UNKOWN;
}

uint64_t listfile_nodeadd(archive_t *archive, const char *filename) {
  __ENTER_FUNCTION
    filenode_t *node = NULL;
    hashkey_t *hashend = archive->hashkey_table + archive->header.hashtable_size;
    hashkey_t *hash0 = get_hashentry(archive, filename);
    hashkey_t *hash = hash0;
    uint64_t hashindex = 0;
    size_t length = 0;
    uint64_t name1 = 0, name2 = 0;
    //If the file does not exist within the pak, do nothing
    if (NULL == hash) return PAK_ERROR_NONE;
    //If the listfile entry already exists, do nothing 
    hashindex = (uint64_t)(hash - archive->hashkey_table);
    name1 = hash->name1;
    name2 = hash->name2;
    if ((uint64_t *)archive->listfiles[hashindex] < PAK_LISTFILE_ENTRY_DELETED)
      return PAK_ERROR_ALREADY_EXISTS;
    //Create the listfile node and insert it into the listfile table 
    length = strlen(filename);
    node = (filenode_t *)malloc(sizeof(filenode_t) + length);
    Assert(node);
    if (NULL == node) return PAK_ERROR_NOT_ENOUGH_MEMORY;
    memset(node, 0, sizeof(filenode_t) + length);
    node->referencescount = 0;
    node->length = length;
    strcpy(node->filename, filename);
    //Fill the nodes for all language versions
    while (hash->blockindex < kBlockStatusHashEntryDeleted) {
      if (hash->name1 == name1 && hash->name2 == name2) {
        ++(node->referencescount);
        archive->listfiles[hash - archive->hashkey_table] = node;
      }
      if (++hash >= hashend) hash = archive->hashkey_table;
      if (hash == hash0) break;
    }
    return PAK_ERROR_NONE;
  __LEAVE_FUNCTION
    return PAK_ERROR_UNKOWN;
}

uint64_t listfile_noderemove(archive_t *archive, const char *filename) {
  __ENTER_FUNCTION
    filenode_t *node = NULL;
    hashkey_t *hash = get_hashentry(archive, filename);
    size_t hashindex = 0;
    if (hash != NULL) {
      hashindex = hash - archive->hashkey_table;
      node = archive->listfiles[hashindex];
      archive->listfiles[hashindex] = (filenode_t *)PAK_LISTFILE_ENTRY_DELETED;
      //If the reference count has reached zero, do nothing
      if (0 == --(node->referencescount)) SAFE_FREE(node);
    }
    return PAK_ERROR_NONE;
  __LEAVE_FUNCTION
    return PAK_ERROR_UNKOWN;
}

uint64_t listfile_noderename(archive_t *archive, 
                             const char *old_filename, 
                             const char *new_filename) {
  __ENTER_FUNCTION
    uint64_t error = listfile_noderemove(archive, old_filename);
    if (PAK_ERROR_NONE == error) 
      error = listfile_nodeadd(archive, new_filename);
    return error;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t attributefile_create(archive_t *archive) {
  __ENTER_FUNCTION
    attribute_t *attribute = NULL;
    uint64_t error = PAK_ERROR_NONE;
    //There should now be any attributes
    Assert(NULL == archive->attribute);
    attribute = (attribute_t *)malloc(sizeof(attribute_t));
    Assert(attribute);
    if (attribute != NULL) {
      memset(attribute, 0, sizeof(attribute_t));
      //Pre-set the structure
      attribute->version = PAK_VERSION;
      attribute->flag = 0;
      //Allocate array for CRC32
      attribute->crc32 = 
        (crc32_t *)malloc(sizeof(crc32_t) * archive->header.hashtable_size);
      Assert(attribute->crc32);
      if (attribute->crc32 != NULL) {
        attribute->flag |= PAK_ATTRIBUTE_CRC32;
        memset(attribute->crc32, 
               0, 
               sizeof(crc32_t) * archive->header.hashtable_size);
      } else {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      }
      //Allocate array for FILETIME
      attribute->filetime = 
        (filetime_t *)malloc(sizeof(filetime_t) * archive->header.hashtable_size);
      Assert(attribute->filetime);
      if (attribute->filetime != NULL) {
        attribute->flag |= PAK_ATTRIBUTE_FILETIME;
        memset(attribute->filetime,
               0,
               sizeof(filetime_t) * archive->header.hashtable_size);
      } else {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      }
      //Allocate array for MD5
      attribute->md5 = 
        (md5_t *)malloc(sizeof(md5_t) * archive->header.hashtable_size);
      Assert(attribute->md5);
      if (attribute->md5) {
        attribute->flag |= PAK_ATTRIBUTE_MD5;
        memset(attribute->md5,
               0,
               sizeof(md5_t) * archive->header.hashtable_size);
      } else {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      }
    }
    if (error != PAK_ERROR_NONE) {
      util::freeattributes(attribute);
      attribute = NULL;
    }
    archive->attribute = attribute;
    return error;
  __LEAVE_FUNCTION
    return PAK_ERROR_UNKOWN;
}

uint64_t attributefile_normalload(archive_t *archive) {
  __ENTER_FUNCTION
    attribute_t *attribute = NULL;
    uint64_t bytesread = 0;
    uint64_t toread = 0;
    file_t *file = NULL;
    uint64_t used = 0;

    uint16_t file_headerlength = sizeof(uint64_t) + sizeof(uint64_t); 
    uint64_t file_savedlength = file_headerlength;
    file_savedlength += sizeof(crc32_t) * archive->header.blocktable_size;
    file_savedlength += sizeof(filetime_t) * archive->header.blocktable_size;
    file_savedlength += sizeof(md5_t) * archive->header.blocktable_size;
    uint64_t error = PAK_ERROR_NONE;

    //Initially, set the attrobutes to NULL
    archive->attribute = NULL;
    //Attempt to open the "(attributes)" file.
    //If it's not there, we don't support attributes
    file = fileopen(archive, PAK_ATTRIBUTES_NAME, error);
   
    char *attributefile_cache = (char *)malloc(sizeof(char) * file_savedlength);
    Assert(attributefile_cache);
    if (attributefile_cache != NULL) {
      memset(attributefile_cache, 0, file_savedlength);
    } else {
      error = PAK_ERROR_NOT_ENOUGH_MEMORY;
    }
    char *attributefile_compress = 
      (char *)malloc(sizeof(char) * (file_savedlength + 8));
    Assert(attributefile_compress);
    if (attributefile_compress != NULL) {
      memset(attributefile_compress, 0, file_savedlength + 8);
    } else {
      error = PAK_ERROR_NOT_ENOUGH_MEMORY;
    }
    if (PAK_ERROR_NONE == error) {
      fileread(file, attributefile_compress, file_savedlength + 8, &bytesread);
      if (bytesread != file_savedlength + 8) error = PAK_ERROR_FILE_CORRUPT;
      if (*(uint64_t *)attributefile_compress != file_savedlength) {
        error = PAK_ERROR_FILE_CORRUPT;
      } else {
        used = *((uint64_t *)attributefile_compress + 1);
      }
    }
    int32_t outlength = static_cast<int32_t>(file_savedlength);
    if (PAK_ERROR_NONE == error) {
      compress::de_zlib(attributefile_cache, 
                        &outlength, 
                        attributefile_compress + file_headerlength, 
                        static_cast<int32_t>(used));
      if (outlength != static_cast<int32_t>(file_savedlength)) 
        error = PAK_ERROR_FILE_CORRUPT;
    }
    //Allocate space for the TMPQAttributes
    if (PAK_ERROR_NONE == error) {
      attribute = (attribute_t *)malloc(sizeof(attribute_t));
      Assert(attribute);
      if (NULL == attribute) {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      } else {
        memset(attribute, 0, sizeof(attribute_t));
      }
    }
    char *copybuffer = attributefile_cache;
    //Load the content of the attributes file
    if (PAK_ERROR_NONE == error) {
      memset(attribute, 0, sizeof(attribute_t));
      toread = sizeof(uint64_t) + sizeof(uint64_t);
      memcpy(attribute, copybuffer, toread);
      copybuffer += toread;
    }
    //Verify format of the attributes 
    if (PAK_ERROR_NONE == error) {
      if (attribute->version > PAK_VERSION) error = PAK_ERROR_BAD_FORMAT;
    }

    //Read the CRC32s (if any)
    if (PAK_ERROR_NONE == error && attribute->flag & PAK_ATTRIBUTE_CRC32) {
      attribute->crc32 = 
        (crc32_t *)malloc(sizeof(crc32_t) * archive->header.hashtable_size);
      Assert(attribute->crc32);
      if (attribute->crc32 != NULL) {
        memset(attribute->crc32, 
               0, 
               sizeof(crc32_t) * archive->header.hashtable_size);
        toread = sizeof(crc32_t) * archive->header.hashtable_size;
        copybuffer += toread;
      } else {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      }
    }

    //Read the FILETIMEs (if any)
    if (PAK_ERROR_NONE == error && attribute->flag & PAK_ATTRIBUTE_FILETIME) {
      attribute->filetime = 
        (filetime_t *)malloc(sizeof(filetime_t) * archive->header.hashtable_size);
      Assert(attribute->filetime);
      if (attribute->filetime != NULL) {
        memset(attribute->filetime, 
               0, 
               sizeof(filetime_t) * archive->header.hashtable_size);
        toread = sizeof(filetime_t) * archive->header.hashtable_size;
        copybuffer += toread;
      } else {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      }
    }
    //Read the MD5s (if any)
    if (PAK_ERROR_NONE == error && attribute->flag & PAK_ATTRIBUTE_MD5) {
      attribute->md5 = 
        (md5_t *)malloc(sizeof(md5_t) * archive->header.hashtable_size);
      Assert(attribute->md5);
      if (attribute->md5 != NULL) {
        memset(attribute->md5, 
               0, 
               sizeof(md5_t) * archive->header.hashtable_size);
        toread = sizeof(md5_t) * archive->header.hashtable_size;
        copybuffer += toread;
      } else {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      }
    }
    SAFE_FREE(attributefile_compress);
    SAFE_FREE(attributefile_cache);
    //Set the attributes into the MPQ archive
    if (PAK_ERROR_NONE == error) {
      archive->attribute = attribute;
      attribute = NULL;
    }
    fileclose(file);
    return error;
  __LEAVE_FUNCTION
    return PAK_ERROR_UNKOWN;
}

uint64_t attributefile_patchload(archive_t *archive) {
  __ENTER_FUNCTION
    attribute_t *attribute = NULL;
    uint64_t bytesread = 0;
    uint64_t toread = 0;
    file_t *file = NULL;
    uint64_t error = PAK_ERROR_NONE;
    //Initially, set the attrobutes to NULL
    archive->attribute = NULL;
    //Attempt to open the "(attributes)" file.
    //If it's not there, we don't support attributes
    file = fileopen(archive, PAK_ATTRIBUTES_NAME, error);
    
    //Allocate space for the TMPQAttributes 
    if (PAK_ERROR_NONE == error) {
      attribute = (attribute_t *)malloc(sizeof(attribute_t));
      Assert(attribute);
      if (attribute != NULL) {
        memset(attribute, 0, sizeof(attribute_t));
      } else {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      }
    }
    //Load the content of the attributes file
    if (PAK_ERROR_NONE == error) {
      memset(attribute, 0, sizeof(attribute_t));
      toread = sizeof(uint64_t) + sizeof(uint64_t);
      fileread(file, attribute, toread, &bytesread);
      if (bytesread != toread) error = PAK_ERROR_FILE_CORRUPT;
    }
    //Verify format of the attributes
    if (PAK_ERROR_NONE == error) {
      if (attribute->version > PAK_VERSION) error = PAK_ERROR_BAD_FORMAT;
    }
    //Read the CRC32s (if any)
    if (PAK_ERROR_NONE == error && (attribute->flag & PAK_ATTRIBUTE_CRC32)) {
      attribute->crc32 = 
        (crc32_t *)malloc(sizeof(crc32_t) * archive->header.hashtable_size);
      Assert(attribute->crc32);
      if (attribute->crc32 != NULL) {
        memset(attribute->crc32, 
               0, 
               sizeof(crc32_t) * archive->header.hashtable_size);
        toread = sizeof(crc32_t) * archive->header.hashtable_size;
        fileread(file, attribute->crc32, toread, &bytesread);
        if (bytesread != toread) error = PAK_ERROR_FILE_CORRUPT;
      } else {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      }
    }
    //Read the FILETIMEs (if any)
    if (PAK_ERROR_NONE == error && (attribute->flag & PAK_ATTRIBUTE_FILETIME)) {
      attribute->filetime = 
        (filetime_t *)malloc(sizeof(filetime_t) * archive->header.hashtable_size);
      Assert(attribute->filetime);
      if (attribute->filetime != NULL) {
        memset(attribute->filetime, 
               0, 
               sizeof(filetime_t) * archive->header.hashtable_size);
        toread = sizeof(filetime_t) * archive->header.hashtable_size;
        fileread(file, attribute->filetime, toread, &bytesread);
        if (bytesread != toread) error = PAK_ERROR_FILE_CORRUPT;
      } else {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      }
    }
    //Read the MD5s (if any)
    if (PAK_ERROR_NONE == error && (attribute->flag & PAK_ATTRIBUTE_MD5)) {
      attribute->md5 = 
        (md5_t *)malloc(sizeof(md5_t) * archive->header.hashtable_size);
      Assert(attribute->md5);
      if (attribute->md5 != NULL) {
        memset(attribute->md5, 
               0, 
               sizeof(md5_t) * archive->header.hashtable_size);
        toread = sizeof(md5_t) * archive->header.hashtable_size;
        fileread(file, attribute->md5, toread, &bytesread);
        if (bytesread != toread) error = PAK_ERROR_FILE_CORRUPT;
      } else {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      }
    }
    //Set the attributes into the MPQ archive
    if (PAK_ERROR_NONE == error) {
      archive->attribute = attribute;
      attribute = NULL;
    }
    fileclose(file);
    return error;
  __LEAVE_FUNCTION
    return PAK_ERROR_UNKOWN;
}

} //namespace pak
