#include "pf/base/string.h"
#include "crc32.h"
#include "md5.h"
#include "pak/compress.h"
#include "pak/file.h"
#include "pak/encrypt.h"
#include "pak/listfile.h"
#include "pak/interface.h"
#include "pak/util.h"

#define ID_WAVE 0x46464952
#define ID_EXE 0x00005A4D

int32_t g_pakerror;
int32_t g_pak_data_compressiontype = PAK_COMPRESSION_ZLIB;

#if __WINDOWS__
pf_sys::ThreadLock g_thread_lock;
#endif

namespace pak {

namespace util {

static int32_t wave_cmplevel[] = {-1, 4, 2};
static int32_t wave_cmptype[] = {PAK_COMPRESSION_PKWARE, 0x81, 0x81};

void set_lasterror(int32_t error) {
  g_pakerror = error;
}

int32_t get_lasterror() {
  return g_pakerror;
}

const char *get_last_errorstr(int32_t error) {
  const char *errorstr;
  switch (error) {
    case PAK_ERROR_INVALID_FUNCTION:
      errorstr = "function not implemented";
      break;
    case PAK_ERROR_FILE_NOT_FOUND:
      errorstr = "file not found";
      break;
    case PAK_ERROR_ACCESS_DENIED:
      errorstr = "access denied";
      break;
    case PAK_ERROR_NOT_ENOUGH_MEMORY:
      errorstr = "not enough memory";
      break;
    case PAK_ERROR_BAD_FORMAT:
      errorstr = "bad format";
      break;
    case PAK_ERROR_NO_MORE_FILES:
      errorstr = "no more files";
      break;
    case PAK_ERROR_HANDLE_EOF:
      errorstr = "access beyound EOF";
      break;
    case PAK_ERROR_HANDLE_DISK_FULL:
      errorstr = "no space left on device";
      break;
    case PAK_ERROR_INVALID_PARAMETER:
      errorstr = "invalid parameter";
      break;
    case PAK_ERROR_DISK_FULL:
      errorstr = "no space left on device";
      break;
    case PAK_ERROR_ALREADY_EXISTS:
      errorstr = "file exists";
      break;
    case PAK_ERROR_CAN_NOT_COMPLETE:
      errorstr = "operation cannot be completed";
      break;
    default:
      errorstr = "unkown error";
      break;
  }
  return errorstr;
}

void freearchive(archive_t *&archive) {
  __ENTER_FUNCTION
    if (archive) {
      SAFE_FREE(archive->blockcache.buffer);
      SAFE_FREE(archive->block_table);
      SAFE_FREE(archive->hashkey_table);
      if (archive->fp != HANDLE_INVALID_VALUE) {
        file::closeex(archive->fp);
      }
      freeattributes(archive->attribute);
      for (uint64_t i = 0; i < archive->header.hashtable_size; ++i) {
        if (archive->listfiles) {
          filenode_t *node = archive->listfiles[i];
          if ((int64_t *)node < (int64_t *)PAK_LISTFILE_ENTRY_DELETED) {
            SAFE_FREE(node);
          }
        }
      }
      SAFE_FREE(archive->listfiles);
      SAFE_FREE(archive);
    }
  __LEAVE_FUNCTION
}

bool isvalid_archivepointer(archive_t *archive) {
  __ENTER_FUNCTION
    if (NULL == archive) return false;
    if (NULL == &archive->header) return false;
    if (archive->header.magic != PAK_MAGIC) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void freefile(file_t *&file) {
  __ENTER_FUNCTION
    if (file != NULL) {
      if (file->fp != HANDLE_INVALID_VALUE) {
        file::closeex(file->fp);
      }
      SAFE_FREE(file->blockoffset);
      SAFE_FREE(file->buffer);
      SAFE_FREE(file);
    }
  __LEAVE_FUNCTION
}

bool isvalid_filepointer(file_t *file) {
  __ENTER_FUNCTION
    if (NULL == file) return false;
    if (NULL == file->fp) return false;
    if (!isvalid_archivepointer(file->archive)) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void freeattributes(attribute_t *attributes) {
  __ENTER_FUNCTION
    if (attributes != NULL) {
      SAFE_FREE(attributes->crc32);
      SAFE_FREE(attributes->filetime);
      SAFE_FREE(attributes->md5);
      SAFE_FREE(attributes);
    }
  __LEAVE_FUNCTION
}

int32_t save_listfile(archive_t *archive, bool internal) {
  __ENTER_FUNCTION
    int32_t error = PAK_ERROR_NONE;
    handle_t fp = HANDLE_INVALID_VALUE;
    hashkey_t *hashend = NULL, *hash0 = NULL, *hash = NULL;
    uint64_t name1 = 0, name2 = 0;
    filenode_t *node = NULL;
    uint64_t transferred;
    size_t length = 0;
    uint64_t file_savedlength = 1024 * 1024 * 2;
    if (NULL == archive->listfiles) return PAK_ERROR_NONE;
    if (PAK_ERROR_NONE == error) {
      fp = file::createex(PAK_LISTFILE_NAME, 
                          GENERIC_READ | GENERIC_WRITE, 
                          0, 
                          NULL, 
                          CREATE_ALWAYS, 
                          FILE_FLAG_DELETE_ON_CLOSE, 
                          NULL);
      if (HANDLE_INVALID_VALUE == fp) error = get_lasterror();
    }
    char *attributefile_cache = (char *)malloc(sizeof(char) * file_savedlength);
    Assert(attributefile_cache);
    if (NULL == attributefile_cache) {
      error = PAK_ERROR_NOT_ENOUGH_MEMORY;
    } else {
      memset(attributefile_cache, 0x00, file_savedlength);
    }
    char *attributefile_compress = 
      (char *)malloc(sizeof(char) * (file_savedlength + 8));
    Assert(attributefile_compress);
    if (NULL == attributefile_compress) {
      error = PAK_ERROR_NOT_ENOUGH_MEMORY;
    } else {
      memset(attributefile_compress, 0x00, file_savedlength + 8);
    }
    *(uint64_t *)attributefile_compress = file_savedlength / 2 + 8;
    uint64_t memory_copyed = 0;
    char *copybuffer = attributefile_cache;
    hashend = archive->hashkey_table + archive->header.hashtable_size;
    hash0 = hash = get_hashentry(archive, NULL);
    if (NULL == hash) hash0 = hash = archive->hashkey_table;
    if (PAK_ERROR_NONE == error) {
      for (;;) {
        if (hash->name1 != name1 && 
            hash->name2 != name2 && 
            hash->blockindex < kBlockStatusHashEntryDeleted) {
          name1 = hash->name1;
          name2 = hash->name2;
          node = archive->listfiles[hash - archive->hashkey_table];
          if ((uint64_t *)node < (uint64_t *)PAK_LISTFILE_ENTRY_DELETED) {
            memory_copyed += node->length + 2;
            if (memory_copyed > file_savedlength) {
              error = PAK_ERROR_INSUFFICIENT_BUFFER;
            } else {
              memcpy(copybuffer, node->filename, node->length);
              copybuffer[node->length + 0] = 0x0D;
              copybuffer[node->length + 1] = 0x0A;
              copybuffer += node->length + 2;
            }
          }
        }
        if (++hash >= hashend) hash = archive->hashkey_table;
        if (hash == hash0) break;
      }
      if (NULL == get_hashentry(archive, PAK_LISTFILE_NAME)) {
        length += strlen(PAK_LISTFILE_NAME);
        memory_copyed += length;
        if (memory_copyed > file_savedlength) {
          error = PAK_ERROR_INSUFFICIENT_BUFFER;
        } else {
          memcpy(copybuffer, PAK_LISTFILE_NAME, length);
          copybuffer[length + 0] = 0x0D;
          copybuffer[length + 1] = 0x0A;
          copybuffer += (length + 2);
        }
      }
      if (PAK_ERROR_NONE == error) {
        int32_t out_length;
        int32_t cmptype = 0; //compression type 压缩类型
        compress::zlib((char *)(attributefile_compress) + 8,
                       &out_length,
                       (char *)attributefile_cache,
                       static_cast<int32_t>(memory_copyed),
                       &cmptype,
                       0);
        *((uint64_t *)attributefile_compress + 1) = static_cast<uint64_t>(out_length);
      }
      if (PAK_ERROR_NONE == error) {
        file::writeex(fp, 
                      attributefile_compress, 
                      file_savedlength / 2 + 8,
                      &transferred, 
                      NULL);
        if (transferred != file_savedlength / 2 + 8)
          error = PAK_ERROR_DISK_FULL;
      }
      SAFE_FREE(attributefile_cache);
      SAFE_FREE(attributefile_compress);
      if (PAK_ERROR_NONE == error) {
        if (internal) {
          uint64_t result = 0;
          file_t *file = fileopen(archive, PAK_LISTFILE_NAME, result);
          if (file && file->block->uncompressedsize > 0) {
            error = internal_updatefile(archive,
                                        fp,
                                        file,
                                        PAK_LISTFILE_NAME,
                                        PAK_FILE_REPLACEEXISTING,
                                        0,
                                        kFileTypeData,
                                        NULL);
          }
          fileclose(file);
        } else {
          error = addfile_toarchive(archive, 
                                    fp,
                                    PAK_LISTFILE_NAME,
                                    PAK_FILE_REPLACEEXISTING,
                                    0,
                                    kFileTypeData, 
                                    NULL);
        }
      }
    }
    if (fp != HANDLE_INVALID_VALUE) file::closeex(fp);
    return error;
  __LEAVE_FUNCTION
    return -1;
}

int32_t save_listfile(archive_t *archive) {
  __ENTER_FUNCTION
    int32_t error = PAK_ERROR_NONE;
    handle_t fp = HANDLE_INVALID_VALUE;
    hashkey_t *hashend = NULL;
    hashkey_t *hash = NULL;
    hashkey_t *hash0 = NULL;
    uint64_t name1 = 0;
    uint64_t name2 = 0;
    filenode_t *node = NULL;
    char buffer[FILENAME_MAX + 4] = {0};
    uint64_t transferred = 0;
    size_t length = 0;
    uint64_t file_savedlength = 0;
    if (NULL == archive->listfiles) return PAK_ERROR_NONE;
    if (PAK_ERROR_NONE == error) {
      fp = file::createex(PAK_LISTFILE_NAME,
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          CREATE_ALWAYS,
                          FILE_FLAG_DELETE_ON_CLOSE,
                          NULL);
      if (NULL == fp) error = get_lasterror();
    }
    //find the hash entry corresponding to listfile
    hashend = archive->hashkey_table + archive->header.hashtable_size;
    hash0 = hash = get_hashentry(archive, 0);
    if (NULL == archive) hash0 = hash = archive->hashkey_table;
    //save file
    if (PAK_ERROR_NONE == error) {
      for (;;) {
        if (hash->name1 != name1 &&
            hash->name2 != name2 &&
            hash->blockindex < kBlockStatusHashEntryDeleted) {
          name1 = hash->name1;
          name2 = hash->name2;
          node = archive->listfiles[hash - archive->hashkey_table];
          if ((uint64_t *)node < PAK_LISTFILE_ENTRY_DELETED) {
            memcpy(buffer, node->filename, node->length);
            buffer[node->length + 0] = 0x0D;
            buffer[node->length + 1] = 0x0A;
            file::writeex(fp, buffer, node->length + 2, &transferred, NULL);
            file_savedlength += transferred;
          }
        }
        if (++hash >= hashend) hash = archive->hashkey_table;
        if (hash == hash0) break;
      }
       if (NULL == get_hashentry(archive, PAK_LISTFILE_NAME)) {
        memcpy(buffer, PAK_LISTFILE_NAME, length);
        buffer[length + 0] = 0x0D;
        buffer[length + 1] = 0x0A;
        file::writeex(fp, buffer, length + 2, &transferred, NULL);
        file_savedlength += transferred;
      }

      {
        error = addfile_toarchive(
            archive, 
            fp,
            PAK_LISTFILE_NAME, 
            PAK_FILE_ENCRYPTED | PAK_FILE_COMPRESS | PAK_FILE_REPLACEEXISTING,
            0,
            kFileTypeData,
            NULL);
      }
    }
    if (fp != HANDLE_INVALID_VALUE) file::closeex(fp);
    return error;
  __LEAVE_FUNCTION
    return -1;
}

int32_t savetables(archive_t *archive) {
  __ENTER_FUNCTION
    int32_t error = PAK_ERROR_NONE;
    uint64_t buffersize = max(
        archive->header.hashtable_size * sizeof(hashkey_t),
        archive->header.blocktable_size * sizeof(block_t));
    char *buffer = NULL;
    uint64_t written = 0;
    uint64_t bytes = 0;
    //allocate buffer for encrypted tables
    if (PAK_ERROR_NONE == error) {
      buffer = (char *)malloc(sizeof(char) * buffersize);
      Assert(buffer);
      if (NULL == buffer) {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      } else {
        memset(buffer, 0, buffersize);
      }
    }
    if (PAK_ERROR_NONE == error) {
      uint64_t headersize = archive->header.headersize;
      int64_t filepointer_high = 0;
      file::setpointer(archive->fp, 0, &filepointer_high, FILE_BEGIN);
      file::writeex(archive->fp, &archive->header, headersize, &written, NULL);
      if (written != archive->header.headersize) error = PAK_ERROR_DISK_FULL;
    }
    //write the hash table
    if (PAK_ERROR_NONE == error) {
      //copy the hash table to temporary buffer 
      bytes = archive->header.hashtable_size * sizeof(hashkey_t);
      memcpy(buffer, archive->hashkey_table, bytes);
      //convert to little endian for file save
      encrypt((uint64_t *)buffer, (char *)"(hashkey table)", bytes >> 4);
      //set the file pointer to the offset of the hash table and write it
      file::setpointer(archive->fp, 
                       archive->hashtable_offset.low, 
                       (int64_t *)&archive->hashtable_offset.high, 
                       FILE_BEGIN);
      file::writeex(archive->fp, buffer, bytes, &written, NULL);
      if (written != bytes) error = PAK_ERROR_DISK_FULL;
    }
    //write the block table
    if (PAK_ERROR_NONE == error) {
      //copy the block table to temporary buffer
      bytes = archive->header.blocktable_size * sizeof(block_t);
      memcpy(buffer, archive->block_table, bytes);
      //encrypt the block table and write it to the file
      encrypt((uint64_t *)buffer, (char *)"(block table)", bytes >> 4);
      //convert to little endian for file save
      file::writeex(archive->fp, buffer, bytes, &written, NULL);
      if (written != bytes) error = PAK_ERROR_DISK_FULL;
    }
    SAFE_FREE(buffer);
    return error;
  __LEAVE_FUNCTION
    return -1;
}

uint64_t save_attributefile(archive_t *archive, bool internal) {
  __ENTER_FUNCTION
    handle_t fp = HANDLE_INVALID_VALUE;
    uint64_t towrite = 0;
    uint64_t written = 0;
    int32_t error = PAK_ERROR_NONE;
    uint16_t file_headerlength = sizeof(towrite) + sizeof(written);
    uint64_t file_savedlength = file_headerlength;
    file_savedlength += archive->header.blocktable_size * sizeof(crc32_t);
    file_savedlength += archive->header.blocktable_size * sizeof(filetime_t);
    file_savedlength += archive->header.blocktable_size * sizeof(md5_t);
    //if there are no attributes, do nothing
    if (NULL == archive->attribute) return PAK_ERROR_NONE;
    //create the local attributes file
    if (PAK_ERROR_NONE == error) {
      fp = file::createex(PAK_ATTRIBUTES_NAME,
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          CREATE_ALWAYS,
                          FILE_FLAG_DELETE_ON_CLOSE,
                          NULL);
      if (HANDLE_INVALID_VALUE == fp) error = get_lasterror();
    }
    char *attributefile_cache = (char *)malloc(sizeof(char) * file_savedlength);
    Assert(attributefile_cache);
    if (NULL == attributefile_cache) {
      error = PAK_ERROR_NOT_ENOUGH_MEMORY;
    } else {
      memset(attributefile_cache, 0xFF, file_savedlength);
    }
    char *attributefile_compress = 
      (char *)malloc(sizeof(char) * file_savedlength + 8);
    Assert(attributefile_compress);
    if (NULL == attributefile_compress)  {
      error = PAK_ERROR_NOT_ENOUGH_MEMORY;
    } else {
      memset(attributefile_compress, 0xFF, file_savedlength);
    }
    *(uint64_t *)attributefile_compress = file_savedlength;
    uint64_t memory_copyed = 0;
    char *copybuffer = attributefile_cache;
    if (PAK_ERROR_NONE == error) {
      towrite = sizeof(uint64_t) + sizeof(uint64_t);
      memory_copyed += towrite;
      if (memory_copyed > file_savedlength) {
        error = PAK_ERROR_INSUFFICIENT_BUFFER;
      } else {
        memcpy(copybuffer, archive->attribute, towrite);
        copybuffer += towrite;
      }
    }
    if (PAK_ERROR_NONE == error && archive->attribute->crc32 != NULL) {
      towrite = sizeof(crc32_t) * archive->header.blocktable_size;
      memory_copyed += towrite;
      if (memory_copyed > file_savedlength) {
        error = PAK_ERROR_INSUFFICIENT_BUFFER;
      } else {
        memcpy(copybuffer, archive->attribute->crc32, towrite);
        copybuffer += towrite;
      }
    }
    if (PAK_ERROR_NONE == error && archive->attribute->filetime != NULL) {
      towrite = sizeof(filetime_t) * archive->header.blocktable_size;
      memory_copyed += towrite;
      if (memory_copyed > file_savedlength) {
        error = PAK_ERROR_INSUFFICIENT_BUFFER;
      } else {
        memcpy(copybuffer, archive->attribute->filetime, towrite);
        copybuffer += towrite;
      }
    }
    if (PAK_ERROR_NONE == error && archive->attribute->md5 != NULL) {
      towrite = sizeof(md5_t) * archive->header.blocktable_size;
      memory_copyed += towrite;
      if (memory_copyed > file_savedlength) {
        error = PAK_ERROR_INSUFFICIENT_BUFFER;
      } else {
        memcpy(copybuffer, archive->attribute->md5, towrite);
        copybuffer += towrite;
      }
    }
    if (PAK_ERROR_NONE == error) {
      int32_t out_length = static_cast<int32_t>(file_savedlength);
      int32_t cmptype = 0;
      compress::zlib(attributefile_compress + file_headerlength, 
                     &out_length, 
                     attributefile_cache, 
                     static_cast<int32_t>(memory_copyed), 
                     &cmptype, 
                     0);
      *((uint64_t *)attributefile_compress + 1) = out_length;
    }
    if (PAK_ERROR_NONE == error) {
      file::writeex(fp, 
                    attributefile_compress, 
                    file_savedlength + 8, 
                    &written, 
                    NULL);
      if (written != file_savedlength + 8) error = PAK_ERROR_DISK_FULL;
    }
    SAFE_FREE(attributefile_compress);
    SAFE_FREE(attributefile_cache);
    //add the attributes into MPQ
    if (PAK_ERROR_NONE == error) {
      if (internal) {
        uint64_t result;
        file_t *file = fileopen(archive, PAK_ATTRIBUTES_NAME, result);
        if (file && file->block->uncompressedsize > 0) {
          error = internal_updatefile(archive, 
                                      fp, 
                                      file, 
                                      PAK_ATTRIBUTES_NAME, 
                                      PAK_FILE_REPLACEEXISTING, 
                                      0, 
                                      kFileTypeData, 
                                      NULL);
          fileclose(file);
        }
      } else {
        error = addfile_toarchive(archive, 
                                  fp, 
                                  PAK_ATTRIBUTES_NAME, 
                                  PAK_FILE_REPLACEEXISTING, 
                                  0, 
                                  kFileTypeData, 
                                  NULL);
      }
    }
    if (fp != HANDLE_INVALID_VALUE) file::closeex(fp);
    return error;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t save_attributefile(archive_t *archive) {
  __ENTER_FUNCTION
    handle_t fp = HANDLE_INVALID_VALUE;
    uint64_t towrite = 0;
    uint64_t written = 0;
    int32_t error = PAK_ERROR_NONE;
    //if there are no attributes, do nothing
    if (NULL == archive->attribute) return PAK_ERROR_NONE;
    //create the local attributes file
    if (PAK_ERROR_NONE == error) {
      fp = file::createex(PAK_ATTRIBUTES_NAME, 
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          CREATE_ALWAYS,
                          FILE_FLAG_DELETE_ON_CLOSE,
                          NULL);
      if (HANDLE_INVALID_VALUE == fp) error = get_lasterror();
    }
    //write the content of the attributes to the file
    if (PAK_ERROR_NONE == error) {
      //write the header of the attributes file
      towrite = sizeof(uint64_t) + sizeof(uint64_t);
      file::writeex(fp, archive->attribute, towrite, &written, NULL);
      if (written != towrite) error = PAK_ERROR_DISK_FULL;
    }
    //write the array of crc32
    if (PAK_ERROR_NONE == error) {
      towrite = sizeof(crc32_t) * archive->header.blocktable_size;
      file::writeex(fp, archive->attribute->crc32, towrite, &written, NULL);
      if (written != towrite) error = PAK_ERROR_DISK_FULL;
    }
    //write the array of filetime
    if (PAK_ERROR_NONE == error) {
      towrite = sizeof(filetime_t) * archive->header.blocktable_size;
      file::writeex(fp, archive->attribute->filetime, towrite, &written, NULL);
      if (written != towrite) error = PAK_ERROR_DISK_FULL;
    }
    //write the array of md5
    if (PAK_ERROR_NONE == error) {
      towrite = sizeof(md5_t) * archive->header.blocktable_size;
      file::writeex(fp, archive->attribute->md5, towrite, &written, NULL);
      if (written != towrite) error = PAK_ERROR_DISK_FULL;
    }
    //add the attributes into MPQ
    if (PAK_ERROR_NONE == error) {
      error = addfile_toarchive(archive, 
                                fp, 
                                PAK_ATTRIBUTES_NAME, 
                                PAK_FILE_COMPRESS | PAK_FILE_REPLACEEXISTING, 
                                0, 
                                kFileTypeData, 
                                NULL);
    }
    if (fp != HANDLE_INVALID_VALUE) file::closeex(fp);
    return error;
  __LEAVE_FUNCTION
    return 0;
}

int32_t addfile_toarchive(archive_t *archive, 
                          void *fp, 
                          const char *archivedname, 
                          uint64_t flag, 
                          uint64_t quality, 
                          int32_t filetype, 
                          bool *replace) {
  __ENTER_FUNCTION
    int32_t error = PAK_ERROR_NONE;
    int32_t cmpfirst = static_cast<int32_t>(get_compressiontype());
    int32_t cmpnext = static_cast<int32_t>(get_compressiontype());
    int32_t cmp = static_cast<int32_t>(get_compressiontype());
    int32_t cmplevel = -1;

    block_t *blockend = NULL;  //pointer to end of the block table
    file_t *file = NULL; //file structure for newly added file
    bool replaced = false; //true replaced, false is add
    int64_t temp_fileposistion; //for various file offset calculations
    uint64_t _filesize = 0;
    uint64_t _filesize_high = 0;
    char *towrite = NULL; //data to write to the file
    char *compressed = NULL; //compressed (target) data
    uint64_t transferred = 0;
    
    blockend = archive->block_table + archive->header.blocktable_size;

    //set the correct compression types 
    if (flag & PAK_FILE_IMPLODE) cmpfirst = cmpnext = PAK_COMPRESSION_PKWARE;

    if (flag & PAK_FILE_COMPRESS) {
      if (kFileTypeData == filetype) {
        cmpfirst = cmpnext = g_pak_data_compressiontype;
      }
      if (kFileTypeWave == filetype) {
        cmpnext = wave_cmptype[quality];
        cmplevel = wave_cmplevel[quality];
      }
    }
    //get the size of the file to be added
    if (PAK_ERROR_NONE == error) {
      _filesize = file::getszie(fp, &_filesize_high);
      //adjust file flags for too-small files
      if (_filesize < 0x04) flag &= ~(PAK_FILE_ENCRYPTED | PAK_FILE_FIXSEED);
      if (_filesize < 0x20) flag &= ~PAK_FILE_COMPRESSED;
      //File in Pak cannot be greater than 4GB
      if (_filesize_high != 0) error = PAK_ERROR_PARAMETER_QUOTA_EXCEEDED;
    }
    //allocate the file entry for newly added file
    if (PAK_ERROR_NONE == error) {
      file = (file_t *)malloc(
          sizeof(char) * (sizeof(file_t) + strlen(archivedname)));
      Assert(file);
      if (NULL == file) {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      } else {
        memset(file, 0, sizeof(file_t) + strlen(archivedname));
      }
    }
    //reset the TMPQFile structure
    if (PAK_ERROR_NONE == error) {
      memset(file, 0, sizeof(file_t));
      pf_base::string::safecopy(file->name, archivedname, sizeof(file->name));
      file->archive = archive;
      //check if the file already exists in the archive
      if ((file->hashkey = get_hashentry(archive, archivedname)) != NULL) {
        if (0 == (flag & PAK_FILE_REPLACEEXISTING)) {
          error = PAK_ERROR_ALREADY_EXISTS;
          file->hashkey = NULL;
        } else {
          file->block = archive->block_table + file->hashkey->blockindex;
          replaced = true;
        }
      }
      if (PAK_ERROR_NONE == error && NULL == file->hashkey) {
        file->hashkey = find_free_hashentry(archive, archivedname);
        if (NULL == file->hashkey) error = PAK_ERROR_HANDLE_DISK_FULL;
      }
      //set the hash index
      file->hashindex = 
        static_cast<uint64_t>(file->hashkey - archive->hashkey_table);
    }
    //find a block table entry for the file
    if (PAK_ERROR_NONE == error) {
      block_t *block = NULL; //entry in the block table 
      //get the position of the first file
      file->mpqoffset.quad = archive->header.blocktable_position + 
                             archive->header.blocktable_size * sizeof(block_t);
      //search the entire block table and find a free block
      //also find PAK offset at which the file data will be stored
      for (block = archive->block_table; block < blockend; ++block) {
        if (block->flag & PAK_FILE_EXISTS) {
          temp_fileposistion = block->fileposition;
          temp_fileposistion += block->compressedsize;
          if (temp_fileposistion > file->mpqoffset.quad)
            file->mpqoffset.quad = temp_fileposistion;
        } else {
          if (NULL == file->block) file->block = block;
        }
      }
      //calculate the raw file offset
      file->rawoffset.quad = file->mpqoffset.quad + archive->headeroffset;
      //if no free block in the middle of the block table,
      //use the one after last used block
      if (NULL == file->block) file->block = block;
      //if the block offset exceeds number of hash entries,
      //we cannot add new file to the PAK
      file->blockindex = 
        static_cast<uint64_t>(file->block - archive->block_table);
      if (file->blockindex >= archive->header.hashtable_size)
        error = PAK_ERROR_HANDLE_DISK_FULL;
    }
    //create seed for file encryption 
    if (PAK_ERROR_NONE == error && (flag & PAK_FILE_ENCRYPTED)) {
      const char *temp = strchr(archivedname, '\\');
      //create seed1 for file encryption
      if (temp != NULL) archivedname = temp + 1;
      file->seed = decrypt_fileseed(archivedname);
      if (flag & PAK_FILE_FIXSEED)
        file->seed = (file->seed + file->mpqoffset.low) ^ _filesize;
    }
    //resolve crc32 and md5 entry for the file
    //only do it when the MPQ archive has attributes
    if (PAK_ERROR_NONE == error && archive->attribute != NULL) {
      if (archive->attribute->crc32 != NULL)
        file->crc32 = archive->attribute->crc32 + file->blockindex;
      if (archive->attribute->filetime != NULL)
        file->filetime = archive->attribute->filetime + file->blockindex;
      if (archive->attribute->md5 != NULL)
        file->md5 = archive->attribute->md5 + file->blockindex;
    }
    //allocate buffers for the compressed data
    if (PAK_ERROR_NONE == error) {
      file->blockcount = (_filesize / archive->blocksize) + 1;
      if (_filesize % archive->blocksize) ++(file->blockcount);
      if (NULL == 
          (file->buffer = (char *)malloc(sizeof(char) * archive->blocksize))) {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      } else {
        memset(file->buffer, 0, archive->blocksize);
      }
      towrite = file->buffer;
    }
    //for compressed files, allocate buffer for 
    //block positions and for the compressed data
    if (PAK_ERROR_NONE == error && (flag & PAK_FILE_COMPRESSED)) {
      file->blockoffset = 
        (lagreint_t *)(char *)malloc(sizeof(lagreint_t) * (file->blockcount + 1));
      Assert(file->blockoffset);
      compressed = (char *)malloc(sizeof(char) * archive->blocksize * 2);
      Assert(compressed);
      if (NULL == file->blockoffset || NULL == compressed)
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      if (file->blockoffset) {
        memset(file->blockoffset, 
               0, 
               sizeof(lagreint_t) * (file->blockcount + 1));
      }
      if (compressed) memset(compressed, 0, archive->blocksize * 2);
      towrite = compressed;
    }
    //set the file position to the point where the file will be stored
    if (PAK_ERROR_NONE == error) {
      //set the file pointer to file data position
      if (file->rawoffset.quad != archive->currentoffset) {
        file::setpointer(archive->fp, 
                         file->rawoffset.low, 
                         &file->rawoffset.high, 
                         FILE_BEGIN);
        archive->currentoffset = file->rawoffset.quad;
      }
      //initialize the hash entry for the file
      file->hashkey->blockindex = file->blockindex;
      //initialize the block table entry for the file 
      file->block->fileposition = file->mpqoffset.quad;
      file->block->uncompressedsize = _filesize;
      file->block->compressedsize = 0;
      file->block->flag = flag | PAK_FILE_EXISTS;
    }
    uint64_t blockposition_length = 0; //length of the file block offset (in bytes)
    //write block positions (if the file will be compressed)
    if (PAK_ERROR_NONE == error && (flag & PAK_FILE_COMPRESSED)) {
      blockposition_length = file->blockcount * sizeof(lagreint_t);
      memset(file->blockoffset, 0, blockposition_length);
      file->blockoffset[0].low = blockposition_length;
      //write the block positions. only swap the first item, rest is zeros.
      file::writeex(archive->fp, 
                    file->blockoffset, 
                    blockposition_length, 
                    &transferred, 
                    NULL);
      //from this position, the archive is considered changed,
      //so the hash table and block table will be written when the archive is closed.
      archive->currentoffset += transferred;
      archive->flag |= PAK_FLAG_CHANGED;
      uint64_t _file_size = 0;
      uint64_t _high = 0;
      _file_size = file::getszie(archive->fp, &_high);
      if (transferred == blockposition_length) {
        file->block->compressedsize += blockposition_length;
      } else {
        error = get_lasterror();
      }
    }
    //write all file blocks
    if (PAK_ERROR_NONE == error) {
      crc32_context crc32_ctx;
      md5_context md5_ctx;
      uint64_t blockcount = 0;
      //initialize crc32 and md5 processing
      CRC32_Init(&crc32_ctx);
      MD5_Init(&md5_ctx);
      cmp = cmpfirst;
      //move the file pointer to the begin of the file
      file::setpointer(fp, 0, NULL, FILE_BEGIN);
      for (blockcount = 0; blockcount < file->blockcount - 1; ++blockcount) {
        uint64_t inlength = archive->blocksize; 
        uint64_t outlength = archive->blocksize;
        //load the block from the file
        file::readex(fp, file->buffer, archive->blocksize, &inlength, NULL);
        if (0 == inlength) break;
        //update crc32 and md5 for the file 
        if (file->crc32 != NULL) {
          CRC32_Update(&crc32_ctx, 
                       (unsigned char *)file->buffer, 
                       static_cast<int32_t>(inlength));
        }
        if (file->md5 != NULL) {
          MD5_Update(&md5_ctx, 
                     (unsigned char *)file->buffer, 
                     static_cast<int32_t>(inlength));
        }
        //compress the block, if necessary
        outlength = inlength;
        if (file->block->flag & PAK_FILE_COMPRESSED) {
          //should be enough for compression
          int32_t _outlength = static_cast<int32_t>(archive->blocksize * 2);
          int32_t _cmptype = 0;
          if (file->block->flag & PAK_FILE_IMPLODE) {
            compress::pklib(compressed, 
                            &_outlength, 
                            file->buffer, 
                            static_cast<int32_t>(inlength), 
                            &_cmptype, 
                            0);
          }
          if (file->block->flag & PAK_FILE_COMPRESS) {
            compress::smart(compressed,
                            &_outlength,
                            file->buffer,
                            static_cast<int32_t>(inlength),
                            cmp,
                            0,
                            cmplevel);
          }
          //the compressed block size must not be the same or greater like
          //the original block size. If yes, do not compress the block 
          //and store the data as-is.
          if (_outlength >= static_cast<int32_t>(inlength)) {
            memcpy(compressed, file->buffer, inlength);
            _outlength = static_cast<int32_t>(inlength);
          }
          //update block positions 
          outlength = _outlength;
          file->blockoffset[blockcount + 1].quad = 
            file->blockoffset[blockcount].quad + outlength;
          cmp = cmpnext;
        }
        //encrypt the block, if necessary
        if (file->block->flag & PAK_FILE_ENCRYPTED) {
          encryptblock((uint64_t *)towrite, 
                       outlength, 
                       static_cast<uint32_t>(file->seed + blockcount));
        }
        //write the block
        file::writeex(archive->fp, towrite, outlength, &transferred, NULL);
        if (transferred != outlength) {
          error = PAK_ERROR_DISK_FULL;
          break;
        }
        //update the hash table position and the compressed file size
        archive->currentoffset += transferred;
        file->block->compressedsize += transferred;
        archive->flag |= PAK_FLAG_CHANGED;
      }
      if (file->crc32 != NULL)
        CRC32_Finish(&crc32_ctx, (uint64_t *)&file->crc32->value);
      if (file->md5 != NULL)
        MD5_Finish(&md5_ctx, (unsigned char *)file->md5->value);
    }
    //now save the block positions
    if (PAK_ERROR_NONE == error && 
        (file->block->flag & PAK_FILE_COMPRESSED)) {
      //if file is encrypted, block positions are also encrypted
      if (flag & PAK_FILE_ENCRYPTED) {
        encryptblock((uint64_t *)file->blockoffset, 
                     blockposition_length, 
                     static_cast<uint32_t>(file->seed - 1));
      }
      //set the position back to the block table
      file::setpointer(archive->fp, 
                       file->rawoffset.low, 
                       &file->rawoffset.high, 
                       FILE_BEGIN);
      //write block positions to the archive
      file::writeex(archive->fp, 
                    file->blockoffset, 
                    blockposition_length, 
                    &transferred, 
                    NULL);
      uint64_t _file_size = 0;
      uint64_t _high = 0;
      _file_size = file::getszie(archive->fp, &_high);
      if (transferred != blockposition_length)
        error = PAK_ERROR_DISK_FULL;
      //update the file position in the archive
      archive->currentoffset = file->rawoffset.quad + transferred;
    }
    
    //if success, we have to change the settings 
    //in pak header. if failed, we have to clean hash entry
    if (PAK_ERROR_NONE == error) {
      archive->blockcache.file = NULL;
      archive->blockcache.blockoffset = 0;
      archive->blockcache.bufferoffset = 0;
      //add new entry to the block table (if needed)
      if (file->blockindex >= archive->header.blocktable_size)
        ++(archive->header.blocktable_size);
      temp_fileposistion = file->mpqoffset.quad + file->block->compressedsize;
      //update archive size (only valid for version V1)
      archive->paksize = temp_fileposistion;
      archive->header.archivesize = temp_fileposistion;
    } else {
      if (file != NULL && file->hashkey != NULL)
        memset(file->hashkey, 0xFF, sizeof(hashkey_t));
    }
    //cleanup
    SAFE_FREE(compressed);
    if (replace != NULL) *replace = replaced;
    freefile(file);
    return error;
  __LEAVE_FUNCTION
    return -1;
}

int32_t addfile_toarchive(archive_t *archive, 
                          const char *content, 
                          uint64_t size,
                          const char *archivedname,
                          uint64_t flag, 
                          uint64_t quality, 
                          int32_t filetype, 
                          bool *replace) {
  __ENTER_FUNCTION
    int32_t error = PAK_ERROR_NONE;
    int32_t cmpfirst = static_cast<int32_t>(get_compressiontype());
    int32_t cmpnext = static_cast<int32_t>(get_compressiontype());
    int32_t cmp = static_cast<int32_t>(get_compressiontype());
    int32_t cmplevel = -1;

    block_t *blockend;  //pointer to end of the block table
    file_t *file = NULL; //file structure for newly added file
    bool replaced = false; //true replaced, false is add
    int64_t temp_fileposistion; //for various file offset calculations
    char *towrite = NULL; //data to write to the file
    char *compressed = NULL; //compressed (target) data
    char *file_content = NULL;
    uint64_t transferred = 0;
    uint64_t filesize_left = size;
    
    blockend = archive->block_table + archive->header.blocktable_size;

    //set the correct compression types 
    if (flag & PAK_FILE_IMPLODE) cmpfirst = cmpnext = PAK_COMPRESSION_PKWARE;

    if (flag & PAK_FILE_COMPRESS) {
      if (kFileTypeData == filetype) {
        cmpfirst = cmpnext = g_pak_data_compressiontype;
      }
      if (kFileTypeWave == filetype) {
        cmpnext = wave_cmptype[quality];
        cmplevel = wave_cmplevel[quality];
      }
    }
    //get the size of the file to be added
    if (PAK_ERROR_NONE == error) {
      //adjust file flags for too-small files
      if (size < 0x04) flag &= ~(PAK_FILE_ENCRYPTED | PAK_FILE_FIXSEED);
      if (size < 0x20) flag &= ~PAK_FILE_COMPRESSED;
    }
    //allocate the file entry for newly added file
    if (PAK_ERROR_NONE == error) {
      file = (file_t *)malloc(
          sizeof(char) * (sizeof(file_t) + strlen(archivedname)));
      Assert(file);
      if (NULL == file) {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      } else {
        memset(file, 0, sizeof(file_t) + strlen(archivedname));
      }
    }
    //reset the TMPQFile structure
    if (PAK_ERROR_NONE == error) {
      memset(file, 0, sizeof(file_t));
      pf_base::string::safecopy(file->name, archivedname, sizeof(file->name));
      file->archive = archive;
      //check if the file already exists in the archive
      if ((file->hashkey = get_hashentry(archive, archivedname)) != NULL) {
        if (0 == (flag & PAK_FILE_REPLACEEXISTING)) {
          error = PAK_ERROR_ALREADY_EXISTS;
          file->hashkey = NULL;
        } else {
          file->block = archive->block_table + file->hashkey->blockindex;
          replaced = true;
        }
      }
      if (PAK_ERROR_NONE == error && NULL == file->hashkey) {
        file->hashkey = find_free_hashentry(archive, archivedname);
        if (NULL == file->hashkey) error = PAK_ERROR_HANDLE_DISK_FULL;
      }
      //set the hash index
      file->hashindex = 
        static_cast<uint64_t>(file->hashkey - archive->hashkey_table);
    }
    //find a block table entry for the file
    if (PAK_ERROR_NONE == error) {
      block_t *block = NULL; //entry in the block table 
      //get the position of the first file
      file->mpqoffset.quad = archive->header.blocktable_position + 
                             archive->header.blocktable_size * sizeof(block_t);
      //search the entire block table and find a free block
      //also find PAK offset at which the file data will be stored
      int64_t emptydisk_space = 0;
      int64_t emptydisk_begin = 0;
      for (block = archive->block_table; block < blockend; ++block) {
        if (block->flag & PAK_FILE_EXISTS) {
          if (emptydisk_begin > 0) 
            emptydisk_space = block->fileposition - emptydisk_begin;
          temp_fileposistion = block->fileposition;
          temp_fileposistion += block->compressedsize;
          if (temp_fileposistion > file->mpqoffset.quad)
            file->mpqoffset.quad = temp_fileposistion;
        } else {
          if (NULL == file->block) file->block = block;
          if (0 == emptydisk_begin) emptydisk_begin = temp_fileposistion;
        }
      }
      //calculate the raw file offset
      file->rawoffset.quad = file->mpqoffset.quad + archive->headeroffset;
      //if no free block in the middle of the block table,
      //use the one after last used block
      if (NULL == file->block) file->block = block;
      //if the block offset exceeds number of hash entries,
      //we cannot add new file to the PAK
      file->blockindex = 
        static_cast<uint64_t>(file->block - archive->block_table);
      if (file->blockindex >= archive->header.hashtable_size)
        error = PAK_ERROR_HANDLE_DISK_FULL;
    }
    //create seed for file encryption 
    if (PAK_ERROR_NONE == error && (flag & PAK_FILE_ENCRYPTED)) {
      const char *temp = strchr(archivedname, '\\');
      //create seed1 for file encryption
      if (temp != NULL) archivedname = temp + 1;
      if (flag & PAK_FILE_FIXSEED)
        file->seed = (file->seed + file->mpqoffset.low) ^ size;
    }
    //resolve crc32 and md5 entry for the file
    //only do it when the MPQ archive has attributes
    if (PAK_ERROR_NONE == error && archive->attribute != NULL) {
      if (archive->attribute->crc32 != NULL)
        file->crc32 = archive->attribute->crc32 + file->blockindex;
      if (archive->attribute->filetime != NULL)
        file->filetime = archive->attribute->filetime + file->blockindex;
      if (archive->attribute->md5 != NULL)
        file->md5 = archive->attribute->md5 + file->blockindex;
    }
    //allocate buffers for the compressed data
    if (PAK_ERROR_NONE == error) {
      file->blockcount = (size / archive->blocksize) + 1;
      if (size % archive->blocksize) ++file->blockcount;
      if (NULL == 
          (file->buffer = (char *)malloc(sizeof(char) * archive->blocksize))) {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      } else {
        memset(file->buffer, 0, archive->blocksize);
      }
      towrite = file->buffer;
    }
    //for compressed files, allocate buffer for 
    //block positions and for the compressed data
    if (PAK_ERROR_NONE == error && (flag & PAK_FILE_COMPRESSED)) {
      file->blockoffset = 
        (lagreint_t *)malloc(sizeof(lagreint_t) * (file->blockcount + 1));
      Assert(file->blockoffset);
      compressed = (char *)malloc(sizeof(char) * archive->blocksize * 2);
      Assert(compressed);
      if (NULL == file->blockoffset || NULL == compressed)
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      if (file->blockoffset) {
        memset(file->blockoffset, 
               0, 
               sizeof(lagreint_t) * (file->blockcount + 1));
      }
      if (compressed) memset(compressed, 0, archive->blocksize * 2);
      towrite = compressed;
    }
    //set the file position to the point where the file will be stored
    if (PAK_ERROR_NONE == error) {
      //set the file pointer to file data position
      if (file->rawoffset.quad != archive->currentoffset) {
        file::setpointer(archive->fp, 
                         file->rawoffset.low, 
                         &file->rawoffset.high, 
                         FILE_BEGIN);
        archive->currentoffset = file->rawoffset.quad;
      }
      //initialize the hash entry for the file
      file->hashkey->blockindex = file->blockindex;
      //initialize the block table entry for the file 
      file->block->fileposition = file->mpqoffset.quad;
      file->block->uncompressedsize = size;
      file->block->compressedsize = 0;
      file->block->flag = flag | PAK_FILE_EXISTS;
    }
    uint64_t blockposition_length = 0; //length of the file block offset (in bytes)
    //write block positions (if the file will be compressed)
    if (PAK_ERROR_NONE == error && (flag & PAK_FILE_COMPRESSED)) {
      blockposition_length = file->blockcount * sizeof(lagreint_t);
      memset(file->blockoffset, 0, blockposition_length);
      file->blockoffset[0].low = blockposition_length;
      //write the block positions. only swap the first item, rest is zeros.
      file::writeex(archive->fp, 
                    file->blockoffset, 
                    blockposition_length, 
                    &transferred, 
                    NULL);
      //from this position, the archive is considered changed,
      //so the hash table and block table will be written when the archive is closed.
      archive->currentoffset = transferred;
      archive->flag |= PAK_FLAG_CHANGED;
      if (transferred == blockposition_length) {
        file->block->compressedsize += blockposition_length;
      } else {
        error = get_lasterror();
      }
    }
    //write all file blocks
    if (PAK_ERROR_NONE == error) {
      crc32_context crc32_ctx;
      md5_context md5_ctx;
      uint64_t blockcount = 0;
      //initialize crc32 and md5 processing
      CRC32_Init(&crc32_ctx);
      MD5_Init(&md5_ctx);
      cmp = cmpfirst;
      //move the file pointer to the begin of the file
      file_content = (char *)content;
      for (blockcount = 0; blockcount < file->blockcount - 1; ++blockcount) {
        uint64_t inlength = archive->blocksize < filesize_left ? 
                            archive->blocksize : 
                            filesize_left;
        uint64_t outlength = archive->blocksize;
        if (0 == inlength) break;
        //load the block from the file
        memcpy(file->buffer, file_content, inlength);
        file_content += inlength;
        filesize_left -= inlength;
        //update crc32 and md5 for the file 
        if (file->crc32 != NULL) {
          CRC32_Update(&crc32_ctx, 
                       (unsigned char *)file->buffer, 
                       static_cast<int32_t>(inlength));
        }
        if (file->md5 != NULL) {
          MD5_Update(&md5_ctx, 
                     (unsigned char *)file->buffer, 
                     static_cast<int32_t>(inlength));
        }
        //compress the block, if necessary
        outlength = inlength;
        if (file->block->flag & PAK_FILE_COMPRESSED) {
          //should be enough for compression
          int32_t _outlength = static_cast<int32_t>(archive->blocksize * 2);
          int32_t _cmptype = 0;
          if (file->block->flag & PAK_FILE_IMPLODE) {
            compress::pklib(compressed, 
                            &_outlength, 
                            file->buffer, 
                            static_cast<int32_t>(inlength), 
                            &_cmptype, 
                            0);
          }
          if (file->block->flag & PAK_FILE_COMPRESS) {
          compress::smart(compressed,
                          &_outlength,
                          file->buffer,
                          static_cast<int32_t>(inlength),
                          cmp,
                          0,
                          cmplevel);
          }
          //the compressed block size must not be the same or greater like
          //the original block size. If yes, do not compress the block 
          //and store the data as-is.
          if (_outlength >= static_cast<int32_t>(inlength)) {
            memcpy(compressed, file->buffer, inlength);
            _outlength = static_cast<int32_t>(inlength);
          }
          //update block positions 
          outlength = _outlength;
          file->blockoffset[blockcount + 1].quad = 
            file->blockoffset[blockcount].quad + outlength;
          cmp = cmpnext;
        }
        //encrypt the block, if necessary
        if (file->block->flag & PAK_FILE_ENCRYPTED) {
          encryptblock((uint64_t *)towrite, 
                       outlength, 
                       static_cast<uint32_t>(file->seed + blockcount));
        }
        //write the block
        file::writeex(archive->fp, towrite, outlength, &transferred, NULL);
        if (transferred != outlength) {
          error = PAK_ERROR_DISK_FULL;
          break;
        }
        //update the hash table position and the compressed file size
        archive->currentoffset += transferred;
        file->block->compressedsize += transferred;
        archive->flag |= PAK_FLAG_CHANGED;
      }
      if (file->crc32 != NULL)
        CRC32_Finish(&crc32_ctx, (uint64_t *)&file->crc32->value);
      if (file->md5 != NULL)
        MD5_Finish(&md5_ctx, (unsigned char *)file->md5->value);
    }

    //now save the block positions
    if (PAK_ERROR_NONE == error && 
        (file->block->flag & PAK_FILE_COMPRESSED)) {
      //if file is encrypted, block positions are also encrypted
      if (flag & PAK_FILE_ENCRYPTED) {
        encryptblock((uint64_t *)file->blockoffset, 
                     blockposition_length, 
                     static_cast<uint32_t>(file->seed - 1));
      }
      //set the position back to the block table
      file::setpointer(archive->fp, 
                       file->rawoffset.low, 
                       &file->rawoffset.high, 
                       FILE_BEGIN);
      //write block positions to the archive
      file::writeex(archive->fp, 
                    file->blockoffset, 
                    blockposition_length, 
                    &transferred, 
                    NULL);
      if (transferred != blockposition_length)
        error = PAK_ERROR_DISK_FULL;
      //update the file position in the archive
      archive->currentoffset += file->rawoffset.quad + transferred;
    }
    
    //if success, we have to change the settings 
    //in pak header. if failed, we have to clean hash entry
    if (PAK_ERROR_NONE == error) {
      archive->blockcache.file = NULL;
      archive->blockcache.blockoffset = 0;
      archive->blockcache.bufferoffset = 0;
      //add new entry to the block table (if needed)
      if (file->blockindex >= archive->header.blocktable_size)
        ++(archive->header.blocktable_size);
      temp_fileposistion = file->mpqoffset.quad + file->block->compressedsize;
      //update archive size (only valid for version V1)
      archive->paksize = temp_fileposistion;
      archive->header.archivesize = temp_fileposistion;
    } else {
      if (file != NULL && file->hashkey != NULL)
        memset(file->hashkey, 0xFF, sizeof(hashkey_t));
    }
    //cleanup
    SAFE_FREE(compressed);
    if (replace != NULL) *replace = replaced;
    freefile(file);
    return error;
  __LEAVE_FUNCTION
    return -1;
}

int32_t internal_updatefile(archive_t *archive,
                            void *fp, 
                            file_t *file,
                            const char *archivedname,
                            uint64_t flag, 
                            uint64_t quality, 
                            int32_t filetype,
                            bool *replace) {
  __ENTER_FUNCTION
    int32_t error = PAK_ERROR_NONE;
    block_t *blockend = NULL;
    uint64_t _filesize = 0;
    uint64_t _filesize_high = 0;
    bool replaced = false;
    char *towrite = NULL;
    uint64_t transferred = 0;

    blockend = archive->block_table + archive->header.blocktable_size;
    //Set the correct compression types
    if (flag & PAK_FILE_IMPLODE) error = PAK_ERROR_INVALID_PARAMETER;
    if (flag & PAK_FILE_COMPRESS) error = PAK_ERROR_INVALID_PARAMETER;
    //Get the size of the file to be added
    if (PAK_ERROR_NONE == error) {
      _filesize = file::getszie(fp, &_filesize_high);
      //File in PAK cannot be greater than 4GB
      if (_filesize_high != 0) error = PAK_ERROR_PARAMETER_QUOTA_EXCEEDED;
    }
    
    //Reset the TMPQFile structure
    if (PAK_ERROR_NONE == error) {
      //Check if the file already exists in the archive
      if ((file->hashkey = get_hashentry(archive, archivedname)) != NULL) {
        if (0 == (flag & PAK_FILE_REPLACEEXISTING)) {
          error = PAK_ERROR_ALREADY_EXISTS;
          file->hashkey = NULL;
        } else {
          file->block = archive->block_table + file->hashkey->blockindex;
          replaced = true;
        }
      }
      if (PAK_ERROR_NONE == error && NULL == file->hashkey) {
        Assert(false);
        error = PAK_ERROR_DISK_FULL;
      }
    }

    //Allocate buffers for the compressed data
    if (PAK_ERROR_NONE == error) {
      file->blockcount = (_filesize / archive->blocksize) + 1;
      if (_filesize % archive->blocksize) ++(file->blockcount);
      if (NULL == 
          (file->buffer = (char *)malloc(sizeof(char) * archive->blocksize))) {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      } else {
        memset(file->buffer, 0, archive->blocksize);
      }
      towrite = file->buffer;
    }
    
    //Set the file position to the point where the file will be stored
    if (PAK_ERROR_NONE == error) {
      //Set the file pointer to file data position
      if (file->rawoffset.quad != archive->currentoffset) {
        file::setpointer(archive->fp, 
                         file->rawoffset.low, 
                         &file->rawoffset.high, 
                         FILE_BEGIN);
        archive->currentoffset = file->rawoffset.quad;
      }
    }
    
    //Write all file blocks
    if (PAK_ERROR_NONE == error) {
      crc32_context crc32_ctx;
      md5_context md5_ctx;
      uint64_t blockcount = 0;
      
      //Initialize CRC32 and MD5 processing
      CRC32_Init(&crc32_ctx);
      MD5_Init(&md5_ctx);
      
      //Move the file pointer to the begin of the file
      file::setpointer(fp, 0, NULL, FILE_BEGIN);
      uint64_t written = 0;
      for (blockcount = 0; blockcount < file->blockcount - 1; ++blockcount) {
        uint64_t inlength = archive->blocksize;
        uint64_t outlength = archive->blocksize;
        //Load the block from the file 
        file::readex(fp, file->buffer, archive->blocksize, &inlength, NULL);
        if (0 == inlength) break;
        //Update CRC32 and MD5 for the file
        if (file->crc32 != NULL) {
          CRC32_Update(&crc32_ctx, 
                       (unsigned char *)file->buffer, 
                       static_cast<int32_t>(inlength));
        }
        if (file->md5 != NULL) {
          MD5_Update(&md5_ctx, 
                     (unsigned char *)file->buffer, 
                     static_cast<int32_t>(inlength));
        }
        //Compress the block, if necessary 
        outlength = inlength;
        if (written + outlength > file->block->compressedsize) Assert(false);
        //Write the block
        file::writeex(archive->fp, towrite, outlength, &transferred, NULL);
        if (transferred != outlength) {
          error = PAK_ERROR_DISK_FULL;
          break;
        }
        //Update the hash table position and the compressed file size
        archive->currentoffset += transferred;
        written += transferred;
        archive->flag |= PAK_FLAG_CHANGED;
      }
      //Finish calculating of CRC32 and MD5
      if (file->crc32 != NULL)
        CRC32_Finish(&crc32_ctx, (uint64_t *)&file->crc32->value);
      if (file->md5 != NULL)
        MD5_Finish(&md5_ctx, (unsigned char *)file->md5->value);
    }

    //If success, we have to change the settings 
    //in pak header. If failed, we have to clean hash entry
    if (PAK_ERROR_NONE == error) {
      archive->blockcache.file = NULL;
      archive->blockcache.blockoffset = 0;
      archive->blockcache.bufferoffset = 0;
    } else {
      //Clear the hash table entry
      if (file != NULL && file->hashkey != NULL)
        memset(file->hashkey, 0xFF, sizeof(hashkey_t));
    }
    if (replace != NULL) *replace = replaced;
    return error;
  __LEAVE_FUNCTION
    return -1;
}

uint64_t internal_addfile(archive_t *archive, const char *filename) {
  __ENTER_FUNCTION
    uint64_t error = PAK_ERROR_NONE;
    hashkey_t *hash = NULL;
    block_t *blockend = NULL;
    block_t *block = NULL;
    bool found_freeentry = false;
    hash = get_hashentry(archive, filename);
    if (NULL == hash) hash = find_free_hashentry(archive, filename);
    if (hash != NULL) {
      blockend = archive->block_table + archive->header.blocktable_size;
      for (block = archive->block_table; block < blockend; ++block) {
        if (0 == (block->flag & PAK_FILE_EXISTS)) {
          found_freeentry = true;
          break;
        }
      }
      //If the block is out of the available entries, return error
      if (block >= archive->block_table + archive->header.hashtable_size)
        return PAK_ERROR_DISK_FULL;
      //If we had to add the file at the end, increment the block table
      if (false == found_freeentry) ++(archive->header.blocktable_size);
      //Fill the block entry
      block->fileposition = archive->hashtable_offset.quad;
      block->uncompressedsize = 0;
      block->compressedsize = 0;
      block->flag = PAK_FILE_EXISTS;
      //Add the node for the file name
      error = listfile_nodeadd(archive, filename);
    } else {
      error = PAK_ERROR_DISK_FULL;
    }
    return error;
  __LEAVE_FUNCTION
    return 0;
}

//When this function is called, 
//it is already ensured that the parameters are valid
//(e.g. the "toread + fileposition" is not greater than the file size)
static uint64_t readfile_singleuint(file_t *file, 
                                    uint64_t fileposition, 
                                    char *buffer, 
                                    uint64_t toread) {
  __ENTER_FUNCTION
    archive_t *archive = file->archive;
    uint64_t bytesread = 0;
    if (archive->currentoffset != file->rawoffset.quad) {
      file::setpointer(archive->fp, 
                       file->rawoffset.low, 
                       &file->rawoffset.high, 
                       FILE_BEGIN);
      archive->currentoffset = file->rawoffset.quad;
    }
    //If the file is really compressed, decompress it.
    //Otherwise, read the data as-is to the caller.
    if (file->block->compressedsize < file->block->uncompressedsize) {
      if (NULL == file->buffer) {
        char *inputbuffer = NULL;
        int32_t output_buffersize = 
          static_cast<int32_t>(file->block->uncompressedsize);
        int32_t input_buffersize =
          static_cast<int32_t>(file->block->compressedsize);
        file->buffer = (char *)malloc(sizeof(char) * output_buffersize);
        inputbuffer = (char *)malloc(sizeof(char) * input_buffersize);
        Assert(file->buffer);
        if (file->buffer) memset(file->buffer, 0, output_buffersize);
        Assert(inputbuffer);
        if (inputbuffer) memset(inputbuffer, 0, input_buffersize);
        if (inputbuffer != NULL && file->buffer != NULL) {
          //Read the compressed file data 
          file::readex(archive->fp, 
                       inputbuffer, 
                       input_buffersize, 
                       &bytesread, 
                       NULL);
          //Is the file compressed with PKWARE Data Compression Library
          if (file->block->flag & PAK_FILE_IMPLODE) {
            compress::de_pklib(file->buffer, 
                               &output_buffersize, 
                               inputbuffer, 
                               input_buffersize);
          }
          //Is it a file compressed by Blizzard's multiple compression
          //Note that Storm.dll v 1.0.9 distributed with Warcraft III
          //passes the full path name of the 
          //opened archive as the new last parameter
          if (file->block->flag & PAK_FILE_COMPRESS) {
            compress::de_smart(file->buffer, 
                               &output_buffersize, 
                               inputbuffer, 
                               input_buffersize);
          }
        }
        SAFE_FREE(inputbuffer);
      }
      //Copy the file data, if any there
      memcpy(archive->attribute, file->buffer + fileposition, toread);
      bytesread = toread;
    } else {
      //Read the uncompressed file data
      file::readex(archive->fp, buffer, toread, &bytesread, NULL);
    }
    return bytesread;
  __LEAVE_FUNCTION
    return 0;
}

static uint64_t internal_readblocks(file_t *file, 
                                    uint64_t blockposition, 
                                    char *buffer, 
                                    uint64_t blockbytes) {
  __ENTER_FUNCTION
    lagreint_t fileposition;
    archive_t *archive = file->archive;
    char *tempbuffer = NULL;
    int64_t _fileposition = blockposition;
    uint64_t toread = 0;
    uint64_t blocknumber = 0;
    uint64_t bytesread = 0;
    uint64_t bytesremain = 0;
    uint64_t blockcount = 0;
    uint64_t i;
    memset(&fileposition, 0, sizeof(fileposition));
    //Test parameters. 
    //Block position and block size must be block-aligned, block size nonzero 
    if (blockposition & (archive->blocksize - 1) && 0 == blockbytes) return 0;
    //Check the end of file
    if (blockposition + blockbytes > file->block->uncompressedsize)
      blockbytes = file->block->uncompressedsize - blockposition;
    bytesremain = file->block->uncompressedsize - blockposition;
    blocknumber = blockposition / archive->blocksize;
    blockcount = blockbytes / archive->blocksize;
    if (blockbytes % archive->blocksize) ++blockcount;
    //If file has variable block positions, we have to load the
    if ((file->block->flag & PAK_FILE_COMPRESSED) && 
        !file->blockposition_loaded) {
      //Move file pointer to the begin of the file in the MPQ
      if (file->rawoffset.quad != archive->currentoffset) {
        file::setpointer(archive->fp, 
                         file->rawoffset.low, 
                         &file->rawoffset.high,
                         FILE_BEGIN);
      }
      //Read block positions from begin of file.
      toread = (file->blockcount + 1) * sizeof(lagreint_t);
      //Read the block pos table and convert the buffer to little endian
      file::readex(archive->fp, file->blockoffset, toread, &bytesread, NULL);
      //If the archive if protected some way, perform additional check
      //Sometimes, the file appears not to be encrypted, but it is.
      //
      //Note: In WoW 1.10+, there's a new flag. With this flag present,
      //there's one additional entry in the block table.
      if (file->blockoffset[0].quad != static_cast<int64_t>(bytesread))
        file->block->flag |= PAK_FILE_ENCRYPTED;
      //Decrypt loaded block positions if necessary
      if (file->block->flag & PAK_FILE_ENCRYPTED) {
        //If we don't know the file seed, try to find it.
        if (0 == file->seed) {
          file->seed = 
            detect_fileseed1((uint64_t *)file->blockoffset, bytesread);
        }
        //If we don't know the file seed, sorry but we cannot extract the file.
        if (0 == file->seed) return 0;
        //Decrypt block positions
        decryptblock((uint64_t *)file->blockoffset, 
                     bytesread, 
                     static_cast<uint32_t>(file->seed - 1));
        //Check if the block positions are correctly decrypted
        //I don't know why, 
        //but sometimes it will result invalid block positions on some files
        if (file->blockoffset[0].quad != static_cast<int64_t>(bytesread)) {
          file::setpointer(archive->fp, 
                           file->rawoffset.low, 
                           &file->rawoffset.high, 
                           NULL);
          file::readex(archive->fp, file->blockoffset, toread, &bytesread, NULL);
          file->seed = detect_fileseed1((uint64_t *)file->blockoffset, bytesread);
          decryptblock((uint64_t *)file->blockoffset, 
                       bytesread, 
                       static_cast<uint32_t>(file->seed - 1));
          //Check if the block positions are correctly decrypted
          if (file->blockoffset[0].quad != static_cast<int32_t>(bytesread)) 
            return 0;
        }
      }
      //Update file's variables 
      archive->currentoffset = file->rawoffset.quad + bytesread;
      file->blockposition_loaded = true;
    }
    //Get file position and number of bytes to read
    _fileposition = blockposition;
    toread = blockbytes;
    if (file->block->flag & PAK_FILE_COMPRESSED) {
      _fileposition = file->blockoffset[blocknumber].quad;
      toread = file->blockoffset[blocknumber + blockcount].quad - _fileposition;
    }
    fileposition.quad = file->rawoffset.quad + _fileposition;
    //Get work buffer for store read data
    tempbuffer = buffer;
    if (file->block->flag & PAK_FILE_COMPRESSED) {
      if (NULL == (tempbuffer = (char *)malloc(sizeof(char) * toread))) {
        set_lasterror(PAK_ERROR_NOT_ENOUGH_MEMORY);
        return 0;
      } else {
        memset(tempbuffer, 0, toread);
      }
    }
    //Set file pointer, if necessary
    if (archive->currentoffset != fileposition.quad) {
      file::setpointer(archive->fp, 
                       fileposition.low, 
                       &fileposition.high, 
                       FILE_BEGIN);
    }
    //15018F87 : Read all requested blocks
    file::readex(archive->fp, tempbuffer, toread, &bytesread, NULL);
    archive->currentoffset = fileposition.quad + bytesread;
    //Block processing part.
    uint64_t blockstart = 0; //Index of block start in work buffer 
    uint64_t blocksize = min(blockbytes, archive->blocksize);
    uint64_t index = blocknumber; //Current block index
    bytesread = 0; //Clear
    //Walk through all blocks
    for (i = 0; i < blockcount; ++i, ++index) {
      char *inputbuffer = tempbuffer + blockstart;
      int32_t outlength = static_cast<int32_t>(archive->blocksize);
      if (bytesremain < static_cast<uint64_t>(outlength)) 
        outlength = static_cast<int32_t>(bytesremain);
      //Get current block length
      if (file->block->flag & PAK_FILE_COMPRESSED) {
        blocksize = 
          file->blockoffset[index + 1].quad - file->blockoffset[index].quad;
      }
      //If block is encrypted, we have to decrypt it.
      if (file->block->flag & PAK_FILE_ENCRYPTED) {
        //If we don't know the seed, try to decode it as WAVE file
        if (0 == file->seed) {
          file->seed = detect_fileseed2((uint64_t *)inputbuffer, 
                                        3, 
                                        ID_WAVE, 
                                        file->block->uncompressedsize - 8, 
                                        0x45564157);
        }
        //Let's try MSVC's standard EXE or header
        if (0 == file->seed) {
          file->seed = detect_fileseed2((uint64_t *)inputbuffer,
                                        2,
                                        0x00905A4D,
                                        0x00000003);
        }
        if (0 == file->seed) return 0;
        decryptblock((uint64_t *)inputbuffer, 
                     blocksize, 
                     static_cast<uint32_t>(file->seed + index));
      }
      //If the block is really compressed, decompress it.
      //WARNING : Some block may not be compressed, it can be determined only 
      //by comparing uncompressed and compressed size !!!
      if (blocksize < static_cast<uint64_t>(outlength)) {
        //Is the file compressed with PKWARE Data Compression Library 
        if (file->block->flag & PAK_FILE_IMPLODE) {
          compress::de_pklib(buffer, 
                             &outlength, 
                             inputbuffer, 
                             static_cast<int32_t>(blocksize));
        }
        //Is it a file compressed by Blizzard's multiple compression
        //Note that Storm.dll v 1.0.9 distributed with Warcraft III
        //passes the full path name of the opened archive as the new last parameter
        if (file->block->flag & PAK_FILE_COMPRESS) {
          compress::de_smart(buffer, 
                             &outlength, 
                             inputbuffer, 
                             static_cast<int32_t>(blocksize));
        }
        bytesread += outlength;
        buffer += outlength;
      } else {
        if (buffer != inputbuffer) memcpy(buffer, inputbuffer, blocksize);
        bytesread += blocksize;
        buffer += blocksize;
      }
      blockstart += blocksize;
      bytesremain -= outlength;
    } //for
    //Delete input buffer, if necessary
    if (file->block->flag & PAK_FILE_COMPRESSED) SAFE_FREE(tempbuffer);
    return bytesread;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t internal_readfile(file_t *file, 
                           uint64_t fileposition, 
                           char *buffer, 
                           uint64_t toread) {
  __ENTER_FUNCTION
    archive_t *archive = file->archive;
    block_t *block = file->block;
    uint64_t bytesread = 0;
    uint64_t blockposition = 0;
    uint64_t loaded = 0;
    //File position is greater or equal to file size
    if (fileposition >= block->uncompressedsize) return bytesread;
    //If too few bytes in the file remaining, cut them
    if (block->uncompressedsize - fileposition < toread)
      toread = block->uncompressedsize - fileposition;
    //If the file is stored as single unit, handle it separately
    if (block->flag & PAK_FILE_SINGLE_UNIT) {
      uint64_t result = 0;
      result = readfile_singleuint(file, fileposition, buffer, toread);
      return result;
    }
    //Block position in the file
    blockposition = (fileposition & ~(archive->blocksize - 1));
    //Load the first block, if incomplete. It may be loaded in the cache buffer.
    //We have to check if this block is loaded. If not, load it.
    if (fileposition % archive->blocksize != 0) {
      //Number of bytes remaining in the buffer
      uint64_t tocopy = 0;
      uint64_t loaded = archive->blocksize;
      //Check if data are loaded in the cache
      if (file != archive->blockcache.file ||
          blockposition != archive->blockcache.blockoffset) {
        //Load one MPQ block into archive buffer
        loaded = internal_readblocks(file, 
                                     blockposition, 
                                     archive->blockcache.buffer, 
                                     archive->blocksize);
        if (0 == loaded) return (uint64_t)-1;
        //Save lastly accessed file and block position for later use
        archive->blockcache.file = file;
        archive->blockcache.blockoffset = blockposition;
        archive->blockcache.blockoffset = fileposition % archive->blocksize;
      }
      tocopy = loaded - archive->blockcache.blockoffset;
      if (tocopy > toread) tocopy = toread;
      //Copy data from block buffer into target buffer
      memcpy(buffer, 
             archive->blockcache.buffer + archive->blockcache.blockoffset, 
             tocopy);
      //Update pointers
      toread -= tocopy;
      bytesread += tocopy;
      buffer += tocopy;
      blockposition += archive->blocksize;
      archive->blockcache.blockoffset += tocopy;
      //If all, return.
      if (0 == toread) return bytesread;
    }
    //Load the whole ("middle") blocks only if there are more or equal one block
    if (toread > archive->blocksize) {
      uint64_t blockbytes = toread & ~(archive->blocksize - 1);
      loaded = internal_readblocks(file, blockposition, buffer, blockbytes);
      if (0 == loaded) return (uint64_t)-1;
      //Update pointers
      toread -= loaded;
      bytesread += loaded;
      buffer += loaded;
      blockposition += loaded;
      if (0 == toread) return bytesread;
    }
    //Load the terminating block
    if (toread > 0) {
      uint64_t tocopy = archive->blocksize;
      //Check if data are loaded in the cache
      if (file != archive->blockcache.file || 
          blockposition != archive->blockcache.blockoffset) {
        //Load one MPQ block into archive buffer
        tocopy = internal_readblocks(file, 
                                     blockposition, 
                                     archive->blockcache.buffer, 
                                     archive->blocksize);
        if (0 == tocopy) return (uint64_t)-1;
        //Save lastly accessed file and block position for later use
        archive->blockcache.file = file;
        archive->blockcache.blockoffset = blockposition;
      }
      archive->blockcache.blockoffset = 0;
      //Check number of bytes read 
      if (tocopy > toread) tocopy = toread;
      memcpy(buffer, archive->blockcache.buffer, tocopy);
      bytesread += tocopy;
      archive->blockcache.blockoffset = tocopy;
    }
    return bytesread;
  __LEAVE_FUNCTION
    return (uint64_t)-1;
}

bool isvalid_searchpointer(search_t *search) {
  __ENTER_FUNCTION
    if (NULL == search) return false;
    if (!isvalid_archivepointer(search->archive)) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace util

} //namespace pak
