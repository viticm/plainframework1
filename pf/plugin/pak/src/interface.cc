#include "pf/base/string.h"
#include "pf/base/util.h"
#include "pf/base/md5.h"
#include "pf/sys/thread.h"
#include "pak/encrypt.h"
#include "pak/file.h"
#include "pak/listfile.h"
#include "pak/util.h"
#include "pak/interface.h"

#define HASH_TABLE_SIZE_MIN 0x00004
#define HASH_TABLE_SIZE_MAX 0x40000

namespace pak {

archive_t *archivecreate(const char *filename, 
                         uint64_t &result, 
                         uint64_t hashtable_size, 
                         uint64_t usetype) {
  __ENTER_FUNCTION
    result = PAK_ERROR_NONE; //this result that we want
    archive_t *archive = NULL;
    handle_t fp = HANDLE_INVALID_VALUE;
    bool is_fileexists = file::getattribute(filename) != 0xFFFFFFFF; //!= -1
    if (is_fileexists) {} //文件存在的处理
    if (PAK_ERROR_NONE == result) preparebuffers();
    fp = file::createex(filename, 
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ,
                        NULL,
                        CREATE_ALWAYS,
                        0,
                        NULL);
    if (HANDLE_INVALID_VALUE == fp) {
      result = util::get_lasterror();
      return NULL;
    }
    if (PAK_ERROR_NONE == result) {
      archive = (archive_t *)malloc(sizeof(archive_t));
      Assert(archive);
      if (archive != NULL) {
        memset(archive, 0, sizeof(archive_t));
      } else {
        result = PAK_ERROR_NOT_ENOUGH_MEMORY;
        return NULL;
      }
    }
    int64_t filepointer_high = 0;
    if (0xFFFFFFFF == file::setpointer(fp, 0, &filepointer_high, FILE_BEGIN)) {
      result = util::get_lasterror();
      return NULL;
    }
    if (hashtable_size > HASH_TABLE_SIZE_MAX) 
      hashtable_size = HASH_TABLE_SIZE_MAX;
    if (hashtable_size < HASH_TABLE_SIZE_MIN)
      hashtable_size = HASH_TABLE_SIZE_MIN;
    //fill archive struct
    if (PAK_ERROR_NONE == result) {
      memset(archive, 0, sizeof(archive_t));
      pf_base::string::safecopy(archive->filename, filename, FILENAME_MAX);
      archive->fp = fp;
      archive->blocksize = 1024 * 4;
      archive->headeroffset = ((sizeof(header_t) - 1)/ 512 + 1) * 512;
      archive->hashtable_offset.quad = ((sizeof(header_t) - 1)/ 512 + 1) * 512;
      archive->currentoffset = 0;

      archive->hashkey_table = 
        (hashkey_t *)malloc(sizeof(hashkey_t) * hashtable_size);
      Assert(archive->hashkey_table);
      if (archive->hashkey_table)
        memset(archive->hashkey_table, 0xFF, sizeof(hashkey_t) * hashtable_size);
      archive->block_table = 
        (block_t *)malloc(sizeof(block_t) * hashtable_size);
      Assert(archive->block_table);
      if (archive->block_table)
        memset(archive->block_table, 0, sizeof(block_t) * hashtable_size);
      archive->blockcache.buffer = 
        (char *)malloc(sizeof(char) * archive->blocksize);
      Assert(archive->blockcache.buffer);
      if (archive->blockcache.buffer)
        memset(archive->blockcache.buffer, 0, archive->blocksize);
      archive->flag |= PAK_FLAG_CHANGED;
    }
    if (NULL == archive->hashkey_table ||
        NULL == archive->block_table ||
        NULL == archive->blockcache.buffer) {
      result = util::get_lasterror();
    }
    fp = HANDLE_INVALID_VALUE;
    //fill all header and buffers
    if (PAK_ERROR_NONE == result) {
      header_t *header = &archive->header;
      memset(header, 0, sizeof(header_t));
      header->magic = PAK_MAGIC;
      header->headersize = sizeof(header_t);
      header->archivesize = 0;
      header->version = PAK_VERSION;
      header->hashtable_size = hashtable_size;
      header->blocktable_size = hashtable_size;
      header->blocksize = 3;
      header->type = static_cast<uint8_t>(usetype);
      header->listfile_cache_max = 1024 * 1024;

      archive->paksize = header->archivesize;
      archive->hashtable_offset.quad = archive->headeroffset;

      header->hashtable_position = 0;

      archive->blocktable_offset.quad = 
        archive->hashtable_offset.quad + 
        header->hashtable_size * sizeof(hashkey_t);
      header->blocktable_position = header->hashtable_size * sizeof(hashkey_t);
    }
    
    if (PAK_ERROR_NONE == result) {
#if __WINDOWS__
      //InitializeCriticalSection(&archive->section);
#endif 
    }
    uint64_t transferred = 0;
    //write header to pak files
    if (PAK_ERROR_NONE == result) {
      file::writeex(archive->fp,
                    &archive->header,
                    archive->header.headersize,
                    &transferred,
                    NULL);
      if (transferred != archive->header.headersize) 
        result = PAK_ERROR_DISK_FULL;
      archive->currentoffset = archive->header.headersize;
    }
    if (PAK_ERROR_NONE == result) util::savetables(archive);
    if (PAK_ERROR_NONE == result) listfile_create(archive);
    if (PAK_ERROR_NONE == result) 
      util::internal_addfile(archive, PAK_LISTFILE_NAME);
    if (PAK_ERROR_NONE == result &&
        PAK_TYPE_NORMAL == archive->header.type) {
      util::save_listfile(archive, false);
    }
    if (PAK_ERROR_NONE == result) attributefile_create(archive);
    if (PAK_ERROR_NONE == result)
      util::internal_addfile(archive, PAK_ATTRIBUTES_NAME);
    if (PAK_ERROR_NONE == result &&
        PAK_TYPE_NORMAL == archive->header.type) {
      util::save_attributefile(archive, false);
    }
    if (result != PAK_ERROR_NONE) util::freearchive(archive);
    return archive;
  __LEAVE_FUNCTION
    return NULL;
}

static int32_t relocate_tableposition(archive_t *archive) {
  __ENTER_FUNCTION
    header_t *header = &archive->header;
    lagreint_t _filesize;
    lagreint_t tempsize;
    //Get the size of the file
    _filesize.low = file::getszie(archive->fp, (uint64_t *)&_filesize.high);
    //Set the proper hash table position
    archive->hashtable_offset.quad = header->hashtable_position;
    archive->hashtable_offset.quad += archive->headeroffset;
    if (archive->hashtable_offset.quad > _filesize.quad) 
      return PAK_ERROR_BAD_FORMAT;
    //Set the proper block table position
    archive->blocktable_offset.quad = header->blocktable_position;
    archive->blocktable_offset.quad += archive->headeroffset;
    if (archive->blocktable_offset.quad > _filesize.quad)
      return PAK_ERROR_BAD_FORMAT;
    //Size of PAK archive is computed as the biggest of 
    //(EndOfBlockTable, EndOfHashTable, EndOfExtBlockTable)
    tempsize.quad = archive->hashtable_offset.quad + 
                    (sizeof(hashkey_t) * header->hashtable_size);
    if (tempsize.quad > archive->paksize)
      archive->paksize = tempsize.quad;
    tempsize.quad = archive->blocktable_offset.quad +
                    (sizeof(block_t) * header->blocktable_size);
    if (tempsize.quad > archive->paksize)
      archive->paksize = tempsize.quad;
    return PAK_ERROR_NONE;
  __LEAVE_FUNCTION
    return PAK_ERROR_UNKOWN;
}

archive_t *archiveopen(const char *filename, 
                       uint64_t &result, 
                       bool writeflag) {
  __ENTER_FUNCTION
    archive_t *archive = NULL;
    handle_t fp = HANDLE_INVALID_VALUE;
    //uint64_t blockindex_max = 0;
    uint64_t blocktable_size = 0;
    uint64_t transferred = 0;
    uint64_t bytes = 0;
    lagreint_t temp_position;
    result = PAK_ERROR_NONE;
    //Check the right parameters
    if (NULL == filename || 0 == *filename) {
      result = PAK_ERROR_INVALID_PARAMETER;
      return archive;
    }
    //Ensure that StormBuffer is allocated
    if (PAK_ERROR_NONE == result) preparebuffers();
    //Open the MPQ archive file
    if (PAK_ERROR_NONE == result) {
      if (writeflag) {
        fp = file::createex(filename, 
                            GENERIC_READ | GENERIC_WRITE, 
                            FILE_SHARE_READ, 
                            NULL, 
                            OPEN_EXISTING, 
                            0, 
                            NULL);
      } else {
        fp = file::createex(filename,
                            GENERIC_READ,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL);
      }
      if (HANDLE_INVALID_VALUE == fp) result = util::get_lasterror();
    }
    //Allocate the pak handle
    if (PAK_ERROR_NONE == result) {
      if (NULL == (archive = (archive_t *)malloc(sizeof(archive_t)))) {
        result = PAK_ERROR_NOT_ENOUGH_MEMORY;
      } else {
        memset(archive, 0, sizeof(archive_t));
      }
    }
    //Initialize handle structure and allocate structure for MPQ header
    if (PAK_ERROR_NONE == result) {
      memset(archive, 0, sizeof(archive_t));
      pf_base::string::safecopy(archive->filename, filename, FILENAME_MAX);
      archive->fp = fp;
      archive->blockcache.file = NULL;
      fp = HANDLE_INVALID_VALUE;
    }
    //Find the offset of pak header within the file
    if (PAK_ERROR_NONE == result) {
      //lagreint_t search_position;
      lagreint_t mpq_position;
      memset(&mpq_position, 0, sizeof(mpq_position));
      uint64_t headerid = 0;
      for (;;) {
        //Invalidate the pak ID and read the eventual header
        file::setpointer(archive->fp, 
                         mpq_position.low, 
                         &mpq_position.high, 
                         FILE_BEGIN);
        file::readex(archive->fp, 
                     &archive->header, 
                     sizeof(header_t), 
                     &transferred, 
                     NULL);
        headerid = archive->header.magic;
        archive->paksize = archive->header.archivesize;
        //If different number of bytes read, break the loop
        if (transferred != sizeof(header_t)) {
          result = PAK_ERROR_BAD_FORMAT;
          break;
        }
        //There must be MPQ header signature
        if (PAK_MAGIC == headerid) {
          //Save the position where the MPQ header has been found
          archive->headeroffset = ((sizeof(header_t) - 1) / 512 + 1) * 512;
          if (PAK_VERSION == archive->header.version) break;
          result = PAK_ERROR_NOT_SUPPORTED;
          break;
        }
        result = PAK_ERROR_BAD_FORMAT;
        return NULL;
      }
    }
    //Relocate tables position
    if (PAK_ERROR_NONE == result) {
      archive->blocksize = (0x200 << archive->header.blocksize);
      result = relocate_tableposition(archive);
    }

    //Allocate buffers 
    if (PAK_ERROR_NONE == result) {
      //Note that the block table should be as large as the hash table
      //(For later file additions).
      //
      //I have found a PAK which has the block table larger than 
      //the hash table. We should avoid buffer overruns caused by that.
      if (archive->header.blocktable_size > archive->header.hashtable_size)
        archive->header.blocktable_size = archive->header.hashtable_size;
      blocktable_size = archive->header.hashtable_size;
      archive->hashkey_table = 
        (hashkey_t *)malloc(sizeof(hashkey_t) * archive->header.hashtable_size);
      archive->block_table = 
        (block_t *)malloc(sizeof(block_t) * blocktable_size);
      archive->blockcache.buffer = (char *)malloc(archive->blocksize);
      Assert(archive->hashkey_table);
      if (archive->hashkey_table) {
        memset(archive->hashkey_table, 
               0, 
               sizeof(hashkey_t) * archive->header.hashtable_size);
      }
      Assert(archive->block_table);
      if (archive->block_table) {
        memset(archive->block_table,
               0,
               sizeof(block_t) * blocktable_size);
      }
      Assert(archive->blockcache.buffer);
      if (archive->blockcache.buffer) {
        memset(archive->blockcache.buffer, 0, archive->blocksize);
      }
      if (!archive->hashkey_table ||
          !archive->block_table ||
          !archive->blockcache.buffer) {
        result = PAK_ERROR_NOT_ENOUGH_MEMORY;
      }
    }

    //Read the hash table into memory
    if (PAK_ERROR_NONE == result) {
      bytes = sizeof(hashkey_t) * archive->header.hashtable_size;
      file::setpointer(archive->fp, 
                       archive->hashtable_offset.low, 
                       &archive->hashtable_offset.high, 
                       FILE_BEGIN);
      file::readex(archive->fp,
                   archive->hashkey_table,
                   bytes,
                   &transferred,
                   NULL);
      if (transferred != bytes) result = PAK_ERROR_FILE_CORRUPT;
    }
    //Decrypt hash table and check if it is correctly decrypted
    if (PAK_ERROR_NONE == result) {
      //We have to convert the hash table from LittleEndian
      decrypt((uint64_t *)archive->hashkey_table, 
              (char *)"(hashkey table)", 
              bytes >> 4);
    }
    //Now, read the block table
    if (PAK_ERROR_NONE == result) {
      memset(archive->block_table, 
             0, 
             sizeof(block_t) * archive->header.blocktable_size);
      //Carefully check the block table size
      bytes = sizeof(block_t) * archive->header.blocktable_size;
      file::setpointer(archive->fp, 
                       archive->blocktable_offset.low, 
                       &archive->blocktable_offset.high, 
                       FILE_BEGIN);
      file::readex(archive->fp, archive->block_table, bytes, &transferred, NULL);
      //I have found a MPQ which claimed 0x200 entries in the block table,
      //but the file was cut and there was only 0x1A0 entries.
      //We will handle this case properly, even if that means
      //omiting another integrity check of the MPQ
      if (transferred < bytes) bytes = transferred;
      //If nothing was read, we assume the file is corrupt
      if (0 == transferred) result = PAK_ERROR_FILE_CORRUPT;
    }
    //Decrypt block table.
    //Some PAKs don't have Decrypted block table, e.g. cracked Diablo version
    //We have to check if block table is really encrypted
    if (PAK_ERROR_NONE == result) {
      block_t *blockend = 
        archive->block_table + archive->header.blocktable_size;
      block_t *block = archive->block_table;
      bool blocktable_encrypted = false;
      //Verify all blocks entries in the table
      //The loop usually stops at the first entry
      while (block < blockend) {
        //The lower 8 bits of the PAK flags are always zero.
        //Note that this may change in next KPM versions
        if (block->flag & 0x000000FF) {
          blocktable_encrypted = true;
          break;
        }
        //Move to the next block table entry
        ++block;
      }
      if (blocktable_encrypted) {
        bytes = sizeof(block_t) * archive->header.blocktable_size;
        decrypt((uint64_t *)archive->block_table, 
                (char *)"(block table)", 
                bytes >> 4);
      }
    }
    //Verify the both block tables (If the PAK file is not protected) 
    if (PAK_ERROR_NONE == result && 
        0 == (archive->flag & PAK_FLAG_PROTECTED)) {
      block_t *blockend = 
        archive->block_table + archive->header.blocktable_size;
      block_t *block = archive->block_table;
      //If the PAK file is not protected,
      //we will check if all sizes in the block table is correct.
      //Note that we will not relocate the block table 
      //(change from previous versions)
      for (; block < blockend; ++block) {
        if (block->flag & PAK_FILE_EXISTS) {
          //Get the 64-bit file position
          temp_position.quad = block->fileposition;
          if (temp_position.quad > archive->paksize ||
              static_cast<int64_t>(block->compressedsize) > archive->paksize) {
            result = PAK_ERROR_BAD_FORMAT;
            break;
          }
        }
      } //for
    }
    //If the caller didn't specified otherwise,
    //include the internal listfile to the TMPQArchive structure
    if (PAK_ERROR_NONE == result) {
      result = listfile_create(archive);
      if (archive) {
#if __WINDOWS__
        //InitializeCriticalSection(archive->section);
#endif
      }
      //Add the internal listfile
      if (PAK_ERROR_NONE == result) {
        if (PAK_TYPE_NORMAL == archive->header.type) {
          listfile_normaladd(archive, NULL);
        } else {
          listfile_patchadd(archive, NULL);
        }
      }
    }
    //If the caller didn't specified otherwise,
    //load the "(attr)" file 
    if (PAK_ERROR_NONE == result) {
      //Ignore the result here. Attrobutes are not necessary, 
      //if they are not there, we will just ignore them
      if (PAK_TYPE_NORMAL == archive->header.type) {
        result = attributefile_normalload(archive);
      } else {
        result = attributefile_patchload(archive);
      }
    }
    //Cleanup and exit
    if (result != PAK_ERROR_NONE) {
      util::freearchive(archive);
      if (fp != HANDLE_INVALID_VALUE) file::closeex(fp);
      util::set_lasterror(static_cast<int32_t>(result));
      archive = NULL;
    }
    return archive;
  __LEAVE_FUNCTION
    return NULL;
}

bool archiveclose(archive_t *archive) {
  __ENTER_FUNCTION
    if (!util::isvalid_archivepointer(archive)) {
      util::set_lasterror(PAK_ERROR_INVALID_PARAMETER);
      return false;
    }
    if (archive->flag & PAK_FLAG_CHANGED) {
      if (PAK_TYPE_NORMAL == archive->header.type) {
        util::save_listfile(archive, true);
      } else {
        util::save_listfile(archive);
      }
      if (PAK_TYPE_NORMAL == archive->header.type) {
        util::save_attributefile(archive, true);
      } else {
        util::save_attributefile(archive);
      }
      util::savetables(archive);
    }
#if __WINDOWS__
    //DeleteCriticalSection(&archive->section);
#endif
    util::freearchive(archive);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool fileadd(archive_t *archive, 
             const char *realname, 
             const char *aliasname,
             uint64_t flags,
             uint64_t quality,
             uint64_t filetype) {
  __ENTER_FUNCTION
    handle_t fp = HANDLE_INVALID_VALUE;
    bool replaced = false;
    int32_t error = PAK_ERROR_NONE;
    if (PAK_ERROR_NONE == error) {
      //Check valid parameters
      if (!util::isvalid_archivepointer(archive) ||
          NULL == realname ||
          0 == *realname ||
          NULL == aliasname ||
          0 == *aliasname) {
        error = PAK_ERROR_INVALID_PARAMETER;
      }
      //Check the values of flags
      if (flags & PAK_FILE_IMPLODE && flags & PAK_FILE_COMPRESS)
        error = PAK_ERROR_INVALID_PARAMETER;
    }
    //If anyone is trying to add listfile, and the archive already has a listfile,
    //deny the operation, but return success.
    if (PAK_ERROR_NONE == error) {
      if (archive->listfiles != NULL &&
          !stricmp(aliasname, PAK_LISTFILE_NAME)) return true;
    }
    //Open added file
    if (PAK_ERROR_NONE == error) {
      fp = file::createex(realname, 
                          GENERIC_READ, 
                          FILE_SHARE_READ, 
                          0, 
                          OPEN_EXISTING, 
                          0, 
                          NULL);
      if (HANDLE_INVALID_VALUE == fp) error = util::get_lasterror();
    }
    if (PAK_ERROR_NONE == error) {
      error = util::addfile_toarchive(archive,
                                      fp,
                                      aliasname,
                                      flags,
                                      quality,
                                      static_cast<int32_t>(filetype),
                                      &replaced);
    }
    //Add the file into listfile also
    if (PAK_ERROR_NONE == error && false == replaced)
      error = static_cast<int32_t>(listfile_nodeadd(archive, aliasname));
    //Cleanup and exit
    if (fp != HANDLE_INVALID_VALUE) file::closeex(fp);
    if (error != PAK_ERROR_NONE) util::set_lasterror(error);
    bool result = PAK_ERROR_NONE == error;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool fileadd_frommemory(archive_t *archive,
                        const char *aliasname,
                        const char *content,
                        uint64_t size,
                        uint64_t flags,
                        uint64_t quality,
                        uint64_t filetype) {
  __ENTER_FUNCTION
    bool replaced = false; //true if replacing file in the archive 
    int32_t error = PAK_ERROR_NONE;
    if (PAK_ERROR_NONE == error) {
      //Check valid parameters
      if (!util::isvalid_archivepointer(archive) ||
          NULL == aliasname ||
          0 == *content) error = PAK_ERROR_INVALID_PARAMETER;
      if (flags & PAK_FILE_IMPLODE && flags && PAK_FILE_COMPRESS)
        error = PAK_ERROR_INVALID_PARAMETER;
    }
    //If anyone is trying to add listfile, 
    //and the archive already has a listfile,
    //deny the operation, but return success.
    if (PAK_ERROR_NONE == error) {
      if (archive->listfiles != NULL &&
          !stricmp(aliasname, PAK_LISTFILE_NAME)) return true; 
    }
    if (PAK_ERROR_NONE == error) {
      error = util::addfile_toarchive(archive, 
                                      content, 
                                      size, 
                                      aliasname, 
                                      flags, 
                                      quality, 
                                      static_cast<int32_t>(filetype), 
                                      &replaced);
    }
    //Add the file into listfile also
    if (PAK_ERROR_NONE == error && false == replaced)
      error = static_cast<int32_t>(listfile_nodeadd(archive, aliasname));
    //Cleanup and exit
    if (error != PAK_ERROR_NONE) util::set_lasterror(error);
    bool result = PAK_ERROR_NONE == error;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool fileremove(archive_t *archive, const char *aliasname) {
  __ENTER_FUNCTION
    block_t *block = NULL;
    hashkey_t *hash = NULL;
    uint64_t blockindex = 0;
    uint64_t error = PAK_ERROR_NONE;
    //Check the parameters
    if (!util::isvalid_archivepointer(archive) ||
        NULL == aliasname ||
        0 == *aliasname) {
      error = PAK_ERROR_INVALID_PARAMETER;
    }
    //Do not allow to remove listfile 
    if (PAK_ERROR_NONE == error) {
      if (!stricmp(aliasname, PAK_LISTFILE_NAME)) 
        error = PAK_ERROR_ACCESS_DENIED;
    }
    //Get hash entry belonging to this file
    if (PAK_ERROR_NONE == error) {
      if (NULL == (hash = get_hashentry(archive, (char *)aliasname))) {
        error = PAK_ERROR_FILE_NOT_FOUND;
      }
    }
    //If index was not found, or is greater than number of files, exit.
    if (PAK_ERROR_NONE == error) {
      if ((blockindex = hash->blockindex) > archive->header.blocktable_size) {
        error = PAK_ERROR_FILE_NOT_FOUND;
      }
    }
    //Get block and test if the file is not already deleted
    if (PAK_ERROR_NONE == error) {
      block = archive->block_table + blockindex;
      if (0 == (block->flag & PAK_FILE_EXISTS)) 
        error = PAK_ERROR_FILE_NOT_FOUND;
    }
    //Remove the file from the list file
    if (PAK_ERROR_NONE == error) listfile_noderemove(archive, aliasname);
    //Now invalidate the block entry and the hash entry. Do not make any
    //relocations and file copying, use SFileCompactArchive for it.
    if (PAK_ERROR_NONE == error) {
      block->fileposition = 0;
      block->uncompressedsize = 0;
      block->compressedsize = 0;
      block->flag = 0;
      hash->name1 = 0xFFFFFFFF;
      hash->name2 = 0xFFFFFFFF;
      hash->blockindex = kBlockStatusHashEntryDeleted;
      //Update PAK archive
      archive->flag |= PAK_FLAG_CHANGED;
    }
    //Resolve error and exit
    if (error != PAK_ERROR_NONE) util::set_lasterror(static_cast<int32_t>(error));
    bool result = PAK_ERROR_NONE == error;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool filerename(archive_t *archive, const char *oldname, const char* newname) {
  __ENTER_FUNCTION
    hashkey_t *oldhash = NULL;
    hashkey_t *newhash = NULL;
    uint64_t blockindex = 0;
    int32_t error = PAK_ERROR_NONE;
    //Test the valid parameters
    if (!util::isvalid_archivepointer(archive) ||
        NULL == oldname ||
        0 == *oldname ||
        NULL == newname ||
        0 == *newname) error = PAK_ERROR_INVALID_PARAMETER;
    //Do not allow to rename listfile
    if (PAK_ERROR_NONE == error) {
      if (!stricmp(oldname, PAK_LISTFILE_NAME)) error = PAK_ERROR_ACCESS_DENIED;
    }
    //Test if the file already exists in the archive
    if (PAK_ERROR_NONE == error) { 
      if ((newhash = get_hashentry(archive, newname)) != NULL) 
        error = PAK_ERROR_ALREADY_EXISTS;
    }
    //Get the hash table entry for the original file
    if (PAK_ERROR_NONE == error) {
      if (NULL == (oldhash = get_hashentry(archive, oldname)))
        error = PAK_ERROR_FILE_NOT_FOUND;
    }
    //Get the hash table entry for the renamed file
    if (PAK_ERROR_NONE == error) {
      //Save block table index and remove the hash table entry
      blockindex = oldhash->blockindex;
      oldhash->name1 = 0xFFFFFFFF;
      oldhash->name2 = 0xFFFFFFFF;
      oldhash->blockindex = kBlockStatusHashEntryDeleted;
      if (NULL == (newhash = find_free_hashentry(archive, newname)))
        error = PAK_ERROR_CAN_NOT_COMPLETE;
    }
    //Save the block index and clear the hash entry
    if (PAK_ERROR_NONE == error) {
      //Copy the block table index
      newhash->blockindex = blockindex;
      archive->flag |= PAK_FLAG_CHANGED;
    }
    //Rename the file in the list file 
    if (PAK_ERROR_NONE == error)
      error = static_cast<int32_t>(listfile_noderename(archive, oldname, newname));
    if (error != PAK_ERROR_NONE) util::set_lasterror(error);
    bool result = PAK_ERROR_NONE == error;
    return result;
  __LEAVE_FUNCTION
    return false;
}

file_t *fileopen(archive_t *archive, const char *name, uint64_t &result) {
  __ENTER_FUNCTION
    g_thread_lock.lock(); /* use global thread lock { */
    lagreint_t fileposition;

    file_t *file = NULL;
    hashkey_t *hash = NULL;
    block_t *block = NULL;
    uint64_t hashindex = 0;
    uint64_t blockindex = (uint64_t)-1;
    size_t handlesize = 0; //Memory space necessary to allocate TMPQHandle
    int32_t error = PAK_ERROR_NONE;

    memset(&fileposition, 0, sizeof(fileposition));

#if _DEBUG
    //Due to increasing numbers of files in PAKs, I had to change the behavior
    //of opening by file index. Now, 
    //the SFILE_OPEN_BY_INDEX value of dwSearchScopemust be entered. 
    //This check will allow to find code places that are incompatible
    //with the new behavior.
    if (name != NULL) Assert((uint64_t *)name > 0x10000);
#endif
    //check parameters
    if (!util::isvalid_archivepointer(archive) ||
        NULL == name ||
        0 == *name) error = PAK_ERROR_INVALID_PARAMETER;
    //Prepare the file opening
    if (PAK_ERROR_NONE == error) {
      //If we have to open a disk file
      handlesize = sizeof(file_t) + strlen(name) + 1;
      if ((hash = get_hashentry(archive, name)) != NULL) {
        hashindex = (uint64_t)(hash - archive->hashkey_table);
        blockindex = hash->blockindex;
      }
    }
    //Get block index from file name and test it
    if (PAK_ERROR_NONE == error) {
      //If index was not found, or is greater than number of files, exit
      //This also covers the deleted files and free entries
      if (blockindex > archive->header.blocktable_size)
        error = PAK_ERROR_FILE_NOT_FOUND;
    }
    //Get block and test if the file was not already deleted.
    if (PAK_ERROR_NONE == error) {
      //Get both block tables and file position
      block = archive->block_table + blockindex;
      fileposition.quad = block->fileposition;
      if (fileposition.quad > archive->paksize ||
          static_cast<int64_t>(block->compressedsize) > archive->paksize) {
        error = PAK_ERROR_FILE_CORRUPT;
      }
      if (0 == (block->flag & PAK_FILE_EXISTS))
        error = PAK_ERROR_FILE_NOT_FOUND;
      if (block->flag & ~PAK_FILE_VALID_FLAGS)
        error = PAK_ERROR_NOT_SUPPORTED;
    }
    //Allocate file handle
    if (PAK_ERROR_NONE == error) {
      file = (file_t *)malloc(handlesize);
      if (NULL == file) {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      } else {
        memset(file, 0, handlesize);
      }
    }
    //Initialize file handle
    if (PAK_ERROR_NONE == error) {
      memset(file, 0, handlesize);
      file->fp = HANDLE_INVALID_VALUE;
      file->archive = archive;
      file->block = block;
      file->blockcount = 
        (file->block->uncompressedsize + archive->blocksize - 1) / 
        archive->blocksize;
      file->hashkey = hash;
      file->mpqoffset.quad = block->fileposition;
      file->rawoffset.quad = file->mpqoffset.quad + archive->headeroffset;
      file->hashindex = hashindex;
      file->blockindex = blockindex;
      //Allocate buffers for decompression.
      if (file->block->flag & PAK_FILE_COMPRESSED) {
        //Allocate buffer for block positions. At the begin of file are stored
        //DWORDs holding positions of each block relative from begin of file in the archive
        //As for newer PAKs, there may be one additional entry in the block table 
        file->blockoffset =
          (lagreint_t *)malloc(sizeof(lagreint_t) * (file->blockcount + 2));
        if (NULL == file->blockoffset) {
          error = PAK_ERROR_NOT_ENOUGH_MEMORY;
        } else {
          memset(file->blockoffset, 
                 0, 
                 sizeof(lagreint_t) * (file->blockcount + 2));
        }
        //Decrypt file seed. Cannot be used if the file is given by index
        if (file->block->flag & PAK_FILE_ENCRYPTED) {
          const char *temp = strchr(name, '\\');
          strcpy(file->name, name);
          if (temp != NULL) name = temp + 1;
          file->seed = decrypt_fileseed(name);
          if (file->block->flag & PAK_FILE_FIXSEED) {
            lagreint_t temp_position;
            temp_position.quad = file->block->fileposition;
            file->seed = 
              (file->seed + temp_position.low) ^ file->block->uncompressedsize;
          }
        }
      }
    }
    //Resolve pointers to file's attributes
    if (PAK_ERROR_NONE == error && archive->attribute != NULL) {
      if (archive->attribute->crc32 != NULL)
        file->crc32 = archive->attribute->crc32 + blockindex;
      if (archive->attribute->filetime != NULL)
        file->filetime = archive->attribute->filetime + blockindex;
      if (archive->attribute->md5 != NULL)
        file->md5 = archive->attribute->md5 + blockindex;
    }
    if (error != PAK_ERROR_NONE) {
      util::freefile(file);
      util::set_lasterror(error);
    }
    result = error;
    g_thread_lock.unlock(); /* } use global thread lock */
    return file;
  __LEAVE_FUNCTION
    return NULL;
}

uint64_t fileclose(file_t *file) {
  __ENTER_FUNCTION
    if (NULL == file) {
      util::set_lasterror(PAK_ERROR_INVALID_PARAMETER);
      return PAK_ERROR_INVALID_PARAMETER;
    }
    //Set the last accessed file in the archive
    if (file->archive != NULL)
      file->archive->blockcache.file = NULL;
    //Free the structure
    util::freefile(file);
    return PAK_ERROR_NONE;
  __LEAVE_FUNCTION
    return 0;
}

bool fileread(file_t *file, 
              void *buffer, 
              uint64_t toread, 
              uint64_t *readed) {
  __ENTER_FUNCTION
    uint64_t bytes = 0;
    int32_t error = PAK_ERROR_NONE;
    //Zero the number of bytes read
    if (readed != NULL) *readed = 0;
    //Check valid parameters
    if (NULL == file ||
        NULL == buffer) error = PAK_ERROR_INVALID_PARAMETER;
    //If direct access to the file, use system for reading
    if (PAK_ERROR_NONE == error && file->fp != HANDLE_INVALID_VALUE) {
      uint64_t transferred = 0;
      file::readex(file->fp, buffer, toread, &transferred, NULL);
      if (transferred < toread) {
        util::set_lasterror(PAK_ERROR_HANDLE_EOF);
        return false;
      }
      if (readed != NULL) *readed = transferred;
      return true;
    }
    //Read all the bytes available in the buffer (If any)
    if (PAK_ERROR_NONE == error) {
      if (toread > 0) {
        bytes = util::internal_readfile(file, 
                                        file->offset, 
                                        (char *)buffer, 
                                        toread);
        if (bytes == (uint64_t)-1) {
          util::set_lasterror(PAK_ERROR_CAN_NOT_COMPLETE);
          return false;
        }
        //there have a bug of memory lost
        //char _buffer[512] = {0};
        //snprintf(_buffer, sizeof(_buffer) - 1, "%s", "string..."); //#20141024 remove this will get a hard bug of memory lost
        file->archive->blockcache.file = file;
        file->offset += bytes;
      }
      if (readed != NULL) *readed = bytes;
    }
    //Check number of bytes read. If not OK, return FALSE.
    if (bytes < toread) {
      util::set_lasterror(PAK_ERROR_HANDLE_EOF);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint64_t filesize(file_t *file) {
  __ENTER_FUNCTION
    if (NULL == file) {
      util::set_lasterror(PAK_ERROR_INVALID_PARAMETER);
      return (uint64_t)-1;
    }
    uint64_t result = file->block->uncompressedsize;
    return result;
  __LEAVE_FUNCTION
    return (uint64_t)-1;
}

uint64_t file_setpointer(file_t *file, int64_t offset, uint64_t method) {
  __ENTER_FUNCTION
    archive_t *archive = NULL;
    if (NULL == file) {
      util::set_lasterror(PAK_ERROR_INVALID_PARAMETER);
      return (uint64_t)-1;
    }
    archive = file->archive;
    switch (method) {
      case FILE_BEGIN: {
        //Cannot set pointer before begin of file
        if (-offset > static_cast<int64_t>(file->offset)) {
          file->offset = 0;
        } else {
          file->offset = offset;
        }
        break;
      }
      case FILE_CURRENT: {
        //Cannot set pointer before begin of file
        if (-offset > static_cast<int64_t>(file->offset)) {
          file->offset = 0;
        } else {
          file->offset += offset;
        }
        break;
      }
      case FILE_END: {
        //Cannot set file position before begin of file
        if (-offset > static_cast<int64_t>(file->block->uncompressedsize)) {
          file->offset = 0;
        } else {
          file->offset = file->block->uncompressedsize + offset;
        }
        break;
      }
      default:
        return PAK_ERROR_INVALID_PARAMETER;
    }
    if (file == archive->blockcache.file &&
        (file->offset & ~(archive->blocksize - 1)) == 
        archive->blockcache.blockoffset) {
      archive->blockcache.file = NULL;
      archive->blockcache.blockoffset = 0;
    }
    uint64_t result = file->offset;
    return result;
  __LEAVE_FUNCTION
    return (uint64_t)-1;
}

uint64_t fileposition(file_t *file) {
  __ENTER_FUNCTION
    if (NULL == file) {
      util::set_lasterror(PAK_ERROR_INVALID_PARAMETER);
      return (uint64_t)-1;
    }
    uint64_t result = file->offset;
    return result;
  __LEAVE_FUNCTION
    return (uint64_t)-1;
}

uint64_t fileskip(file_t *file, int64_t count) {
  __ENTER_FUNCTION
    uint64_t result = file_setpointer(file, count, FILE_CURRENT);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t fileseek(file_t *file, int64_t count) {
  __ENTER_FUNCTION
    uint64_t result = file_setpointer(file, count, FILE_BEGIN);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t filetell(file_t *file) {
  __ENTER_FUNCTION
    uint64_t result = fileposition(file);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

bool fileeof(file_t *file) {
  __ENTER_FUNCTION
    bool result = fileposition(file) >= filesize(file);
    return result;
  __LEAVE_FUNCTION
    return false;
}

//This function compares a string with a wildcard search string.
//returns true, when the string matches with the wildcard.
bool check_wildcard(const char *str, const char *wildcard) {
  __ENTER_FUNCTION
    char *temp = NULL;
    int32_t result = 0;
    int32_t must_notmatch = 0; //Number of following chars int str,
                               //which must not match with wildcard
    int32_t mustmatch = 0; //Number of the following characters, which must match
    //When the string is empty, it does not match with every wildcard
    if (NULL == str || 0 == *str) return false;
    //When the mask is empty, it matches to every wildcard
    if (NULL == wildcard || 0 == *wildcard) return false;
    //Do normal test
    for (;;) {
      switch (*wildcard) {
        case '*': { //Means "every number of characters"
          while ('*' == *wildcard) ++wildcard;
          //When no more characters in wildcard, it means that the strings match
          if (0 == *wildcard) return false;
          //The next N characters must not agree
          must_notmatch |= 0x70000000;
          break;
        }
        case '?': {
          while ('?' == *wildcard) {
            ++must_notmatch;
            ++wildcard;
          }
          break;
        }
        default: {
          //If the two characters match
          if (toupper(*str) == toupper(*wildcard)) {
            //When end of string, they agree
            if (0 == *str) return true;
            ++must_notmatch;
            ++wildcard;
            ++str;
            break;
          }
          //If the next character must match, the string does not match
          if (0 == must_notmatch) return false;
          //Count the characters which must match after characters 
          //that must not match
          temp = (char *)wildcard;
          mustmatch = 0;
          while (*temp != 0 && *temp != '*' && *temp != '?') {
            ++mustmatch;
            ++temp;
          }
          //Now skip characters from szString up to number of chars 
          //that must not match
          result = -1;
          while (must_notmatch > 0 && *str != 0) {
            result = strnicmp(str, wildcard, mustmatch); 
            if (0 == result) break;
            ++str;
            --must_notmatch;
          }
          //Make one more comparison 
          if (0 == must_notmatch) result = strnicmp(str, wildcard, mustmatch);
          //If a match has been found, continue the search
          if (0 == result) {
            must_notmatch = 0;
            wildcard += mustmatch;
            str += mustmatch;
            break;
          }
          return false;
        }
      } //switch
    } //for
  __LEAVE_FUNCTION
    return false;
}

static int32_t dosearch(search_t *search, findfile_data_t *findfile_data) {
  __ENTER_FUNCTION
    archive_t *archive = search->archive;
    filenode_t *node = NULL;
    hashkey_t *hashend = archive->hashkey_table + archive->header.hashtable_size;
    hashkey_t *hash = archive->hashkey_table + search->nextindex;
    //Do until some file found or no more files 
    while (hash < hashend) {
      node = archive->listfiles[search->nextindex++];
      //If this entry is free, do nothing
      if (hash->blockindex < kBlockStatusHashEntryFree &&
          (uint64_t)(uint64_t *)hash < kBlockStatusHashEntryFree) {
        //Check the file name.
        if (check_wildcard(node->filename, search->mask)) {
          block_t *block = archive->block_table + hash->blockindex;
          findfile_data->size = block->uncompressedsize;
          findfile_data->flag = block->flag;
          findfile_data->blockindex = hash->blockindex;
          findfile_data->compressedsize = block->compressedsize;
          //Fill the file name and plain file name
          findfile_data->plainname = strchr(findfile_data->name, '\\');
          if (NULL == findfile_data->plainname) {
            findfile_data->plainname = findfile_data->name;
          } else {
            ++(findfile_data->plainname);
          }
          return PAK_ERROR_NONE;
        }
      }
      ++hash;
    }
    return PAK_ERROR_NO_MORE_FILES;
  __LEAVE_FUNCTION
    return PAK_ERROR_NO_MORE_FILES;
}

search_t *search_firstfile(archive_t *archive, 
                           const char *mask, 
                           findfile_data_t *findfile_data) {
  __ENTER_FUNCTION
    search_t *search = NULL;
    size_t size = 0;
    uint64_t error = PAK_ERROR_NONE;
    //Check for the valid parameters
    if (!util::isvalid_archivepointer(archive) ||
        NULL == mask ||
        NULL == findfile_data) {
      error = PAK_ERROR_INVALID_PARAMETER;
    }
    //Allocate the structure for PAK search 
    if (PAK_ERROR_NONE == error) {
      size = sizeof(search_t) + strlen(mask) + 1;
      search = (search_t *)malloc(size);
      if (NULL == search) {
        error = PAK_ERROR_NOT_ENOUGH_MEMORY;
      } else {
        memset(search, 0, size);
      }
    }
    //Perform the first search
    if (PAK_ERROR_NONE == error) {
      memset(search, 0, size);
      search->archive = archive;
      search->nextindex = 0;
      strcpy(search->mask, mask);
      error = dosearch(search, findfile_data);
    }
    //Cleanup 
    if (error != PAK_ERROR_NONE) {
      SAFE_FREE(search);
      util::set_lasterror(static_cast<int32_t>(error));
    }
    return search;
  __LEAVE_FUNCTION
    return NULL;
}

bool search_nextfile(search_t *search, findfile_data_t *findfile_data) {
  __ENTER_FUNCTION
    int32_t error = PAK_ERROR_NONE;
    if (!util::isvalid_searchpointer(search) || NULL == findfile_data)
      error = PAK_ERROR_INVALID_PARAMETER;
    if (PAK_ERROR_NONE == error)
      error = dosearch(search, findfile_data);
    if (error != PAK_ERROR_NONE) {
      util::set_lasterror(error);
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool search_close(search_t *search) {
  __ENTER_FUNCTION
    if (!util::isvalid_searchpointer(search)) return false;
    SAFE_FREE(search);
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint64_t set_compressiontype(int32_t type) {
  __ENTER_FUNCTION
    int32_t validmask = 
      (PAK_COMPRESSION_ZLIB | PAK_COMPRESSION_PKWARE | PAK_COMPRESSION_BZIP2);
    uint64_t error = PAK_ERROR_NONE;
    if ((type & validmask) != type) {
      error = PAK_ERROR_INVALID_PARAMETER;
      return error;
    }
    g_pak_data_compressiontype = type;
    return error;
  __LEAVE_FUNCTION
    return PAK_ERROR_UNKOWN;
}

uint64_t get_compressiontype() {
  return g_pak_data_compressiontype;
}

} //namespace pak
