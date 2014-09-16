/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/18 16:52
 * @uses plugin pak config file
 */
#ifndef PAK_CONFIG_H_
#define PAK_CONFIG_H_

#include "pf/base/config.h"
#include "pf/sys/thread.h"

#define PAK_FILE_IMPLODE 0x00000200 //Implode method (By PKWARE Data Compression Library)
#define PAK_FILE_COMPRESS 0x00000100 //Compress methods (My various methods)
#define PAK_FILE_COMPRESSED 0x0000FF00 //File is compressed 
#define PAK_FILE_ENCRYPTED 0x00020000 //Indicates whether file is encrypted 
#define PAK_FILE_FIXSEED 0x00010000 //File decrypt seed has to be fixed
#define PAK_FILE_SINGLE_UNIT 0x02000000 //File is stored as a single unit, rather than split into sectors (Thx, Quantam)
#define PAK_FILE_EXISTS 0x80000000 //Set if file exists, reset when the file was deleted
#define PAK_FILE_REPLACEEXISTING 0x80000000 //Replace when the file exist (SFileAddFile)

#define PAK_FILE_VALID_FLAGS (\
  PAK_FILE_IMPLODE | \
  PAK_FILE_COMPRESS | \
  PAK_FILE_ENCRYPTED | \
  PAK_FILE_SINGLE_UNIT | \
  PAK_FILE_EXISTS)

//Compression types for multilpe compressions
#define PAK_COMPRESSION_HUFFMANN 0x01 //Huffmann compression (used on WAVE files only)
#define PAK_COMPRESSION_ZLIB 0x02 //ZLIB compression
#define PAK_COMPRESSION_PKWARE 0x08 //PKWARE DCL compression
#define PAK_COMPRESSION_BZIP2 0x10 //BZIP2 compression
#define PAK_COMPRESSION_WAVE_MONO 0x40
#define PAK_COMPRESSION_WAVE_STEREO 0x80

#define PAK_FLAG_CHANGED  0x00000001 //If set, the pak has been changed
#define PAK_FLAG_PROTECTED 0x00000002 //Set on protected MPQs (like W3M maps)

//Flags for MPQ attributes
#define PAK_ATTRIBUTE_CRC32 0x00000001 //The "(attributes)" contain array of CRC32s
#define PAK_ATTRIBUTE_FILETIME 0x00000002 //The "(attributes)" contain array of filetime
#define PAK_ATTRIBUTE_MD5 0x00000004 //The "(attributes)" contain array of MD5s

#define PAK_LISTFILE_NAME "(list)"
#define PAK_SIGNATURE_NAME "(sign)" //Name of internal signature
#define PAK_ATTRIBUTES_NAME "(attr)" //Name of internal attributes file

#define PAK_LISTFILE_ENTRY_DELETED -2
#define PAK_LISTFILE_ENTRY_FREE -1

#define PAK_TYPE_NORMAL 0x01
#define PAK_TYPE_PATCH 0x02

#define PAK_MD5_LENGTH 16 //使用十六位MD5

/* pak error code { */
#define PAK_ERROR_NONE 0 //没有错误，成功
#define PAK_ERROR_INVALID_FUNCTION 1
#define PAK_ERROR_FILE_NOT_FOUND 2
#define PAK_ERROR_ACCESS_DENIED 5
#define PAK_ERROR_NOT_ENOUGH_MEMORY 8
#define PAK_ERROR_BAD_FORMAT 11
#define PAK_ERROR_NO_MORE_FILES 18
#define PAK_ERROR_GEN_FAILURE 31
#define PAK_ERROR_HANDLE_EOF 38
#define PAK_ERROR_HANDLE_DISK_FULL 39
#define PAK_ERROR_NOT_SUPPORTED 50
#define PAK_ERROR_INVALID_PARAMETER 87
#define PAK_ERROR_DISK_FULL 112
#define PAK_ERROR_CALL_NOT_IMPLEMENTED 120
#define PAK_ERROR_ALREADY_EXISTS 183
#define PAK_ERROR_CAN_NOT_COMPLETE 1003
#define PAK_ERROR_PARAMETER_QUOTA_EXCEEDED 1283
#define PAK_ERROR_FILE_CORRUPT 1392
#define PAK_ERROR_INSUFFICIENT_BUFFER 4999
/* } pak error code */

namespace pak { //一些结构体的定义

typedef void *handle_t; //句柄指针

typedef union {
#ifdef PAK_PLATFORM_LITTLE_ENDIAN
  struct {
    uint64_t low;
    int64_t high;
  };
#else
  struct {
    int64_t low;
    uint64_t high;
  };
#endif
  int64_t quad;
} lagreint_t;

#pragma pack(push, 1)
typedef struct archive_struct archive_t;

typedef struct header_struct {
  uint64_t magic;
  uint64_t headersize;
  uint64_t archivesize;
  uint64_t version;
  uint16_t blocksize;
  int64_t hashtable_position; //哈希表偏移量
  int64_t blocktable_position; //文件块表偏移量
  uint64_t hashtable_size;
  uint64_t blocktable_size;
  uint8_t type; //包的类型
  uint64_t listfile_cache_max; //缓存大小
} header_t;

typedef enum {
  kBlockStatusHashEntryDeleted = 0xFFFFFFFE, //Block index for deleted hash entry
  kBlockStatusHashEntryFree = 0xFFFFFFFF, //Block index for free hash entry 
} blockstatus_t;

typedef struct hashkey_struct {
  uint64_t name1; //The hash of the file path, using method A.
  uint64_t name2; //The hash of the file path, using method B.
  uint64_t blockindex; //The blockindex of block_struct
} hashkey_t;

typedef struct block_struct {
  int64_t fileposition;
  uint64_t compressedsize; //Compressed file size 
  uint64_t uncompressedsize; //Uncompressed file size
  uint64_t flag; //flag for file
  void cleanup() {
    fileposition = 0;
    compressedsize = 0;
    uncompressedsize = 0;
    flag = 0;
  }
} block_t;

typedef struct crc32_struct {
  uint64_t value; //Value of CRC32 for each block 
} crc32_t; //CRC32 present in the (attr) file 

typedef struct filetime_struct {
  uint64_t low;
  uint64_t high;
} filetime_t; //FILETIME present in the (attr) file

typedef struct md5_struct {
  char value[PAK_MD5_LENGTH + 1];
} md5_t; //MD5 present in the (attr) file

typedef struct attribute_struct {
  uint64_t version;
  uint64_t flag;
  crc32_t *crc32;
  filetime_t *filetime;
  md5_t *md5;
} attribute_t; //Data from (attr) file

typedef struct findfile_data_sturct {
  char name[FILENAME_MAX];
  char *plainname; //Pointer to file part
  uint64_t size;
  uint64_t flag;
  uint64_t blockindex;
  uint64_t compressedsize;
} findfile_data_t;

typedef struct search_struct {
  archive_t *archive; //pointer to pak, where the search runs 
  uint64_t nextindex;
  uint64_t name1;
  uint64_t name2;
  char mask[1]; //Search mask (variable length)
} search_t; //Used by searching in pak archives

#pragma pack(pop)

typedef struct filenode_struct {
  uint64_t referencescount; //Number of references
  uint64_t length; //file name length
  char filename[1]; //File name, variable length
} filenode_t;

typedef struct file_struct file_t;

typedef struct blockcache_struct {
  file_t *file;
  uint64_t blockoffset;
  char *buffer;
  uint64_t bufferoffset;
} blockcache_t;

typedef struct archive_struct {
  char filename[FILENAME_MAX]; //Archive file path
  uint64_t priority; //Priority of the archive
  void *fp; //Archive File handle
  int64_t headeroffset; //File header offset (relative to the begin of the file)
  lagreint_t hashtable_offset; //Hash table offset (relative to the begin of the file)
  lagreint_t blocktable_offset; //Block table offset (relative to the begin of the file)
  int64_t currentoffset; //Current file position (relative to begin of the file)
  int64_t paksize; //Size of pak archive
  uint64_t blocksize; //Size of file block
  blockcache_t blockcache; 
  header_t header; //Pointer to header
  hashkey_t *hashkey_table; //Pointer to hashKey table
  block_t *block_table; //Pointer to block table
  attribute_t *attribute; //Pointer to file attribute in (attr) file
  filenode_t **listfiles; //File name array
  uint64_t flag; //
  pf_sys::ThreadLock lock;
  void cleanup() {
    memset(this, 0, sizeof(*this));
  }
  archive_t *createclone(uint64_t &error) {
    error = PAK_ERROR_NONE;
    archive_t *archive = NULL;
    archive = reinterpret_cast<archive_t *>(malloc(sizeof(archive_t) * 1));
    if (NULL == archive) {
      error = PAK_ERROR_NOT_ENOUGH_MEMORY;
    } else {
      archive->cleanup();
    }
    if (PAK_ERROR_NONE == error) {
      *archive = *this;
      archive->blockcache.buffer = NULL;
      archive->blockcache.bufferoffset = 0;
      archive->blockcache.blockoffset = 0;
      archive->blockcache.buffer = 
        reinterpret_cast<char *>(malloc(sizeof(char) * archive->blocksize));
      if (NULL == archive->blockcache.buffer) {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
        SAFE_FREE(archive);
      } else {
        memset(archive->blockcache.buffer, 0, archive->blocksize);
        archive->currentoffset = 0;
      }
    }
    return archive;
  }
  void destoryclone(archive_t *archive) {
    if (archive) {
      SAFE_FREE(archive->blockcache.buffer);
      SAFE_FREE(archive);
    }
  }
} archive_t;

typedef struct file_struct {
  void *fp; //File handle
  archive_t *archive; //Pointer to archive
  hashkey_t *hashkey; //Pointer to hash table
  block_t *block; //Pointer to block table
  uint64_t seed; //Seed used for file decrypt
  uint64_t offset; //File offset from file begin
  lagreint_t rawoffset; //Offset in pak archive (relative to file begin)
  lagreint_t mpqoffset; //Offset in pak archive (relative to pak header)
  lagreint_t *blockoffset; //Position of each file block
  uint64_t blockcount; //Block count
  bool blockposition_loaded; //if block positions loaded
  char *buffer;
  crc32_t *crc32;
  filetime_t *filetime;
  md5_t *md5;
  uint64_t hashindex; //Index to hash table
  uint64_t blockindex; //Index to block table
  char name[1]; //File name (variable length)
} file_t;

typedef enum {
  kFileTypeData = 0, //Process the file as data file
  kFileTypeWave
} filetype_t;

}; //namespace pak

#endif //PAK_CONFIG_H_
