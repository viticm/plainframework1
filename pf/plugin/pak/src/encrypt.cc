#include "pak/encrypt.h"

namespace pak {

bool encrypt_buffer_created = false;
uint64_t encrypt_buffer[HELPER_BUFFER_SIZE];

int32_t preparebuffers() {
  __ENTER_FUNCTION
    uint64_t seed = 0x00100001;
    uint64_t index1 = 0, index2 = 0;
    int32_t i;
    //Initialize the decryption buffer.
    //Do nothing if already done.
    if (false == encrypt_buffer_created) {
      for (index1 = 0; index1 < 0x100; ++index1) {
        for (index2 = index1, i = 0; i < 5; ++i, index2 += 0x100) {
          uint64_t temp1, temp2;
          seed = (seed * 125 + 3) % 0x2AAAAB;
          temp1  = (seed & 0xFFFF) << 0x10;
          seed = (seed * 125 + 3) % 0x2AAAAB;
          temp2  = (seed & 0xFFFF);
          encrypt_buffer[index2] = (temp1 | temp2);
        }
      }
      encrypt_buffer_created = true;
    }
    return PAK_ERROR_NONE;
  __LEAVE_FUNCTION
    return -1;
}

void encrypt(uint64_t *pointer, char *key, uint64_t length) {
  __ENTER_FUNCTION
    uint64_t seed1 = 0x7FED7FED;
    uint64_t seed2 = 0xEEEEEEEE;
    uint64_t _char = 0;
    //Prepare seeds
    while (*key != 0) {
      _char = toupper(*key++);
      seed1 = encrypt_buffer[0x300 + _char] ^ (seed1 + seed2);
      seed2 = _char + seed1 + seed2 + (seed2 << 5) + 3;
    }
    //Encrypt it
    seed2 = 0xEEEEEEEE;
    while (length-- > 0) {
      seed2 += encrypt_buffer[0x400 + (seed1 & 0xFF)];
      _char = *pointer;
      *pointer++ = _char ^ (seed1 + seed2);
      seed1  = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
      seed2  = _char + seed2 + (seed2 << 5) + 3;
    }
  __LEAVE_FUNCTION
}

void decrypt(uint64_t *pointer, char *key, uint64_t length) {
  __ENTER_FUNCTION
    uint64_t seed1 = 0x7FED7FED;
    uint64_t seed2 = 0xEEEEEEEE;
    uint64_t _char = 0;
    //Prepare seeds
    while (*key != 0) {
      _char = toupper(*key++);
      seed1 = encrypt_buffer[0x300 + _char] ^ (seed1 + seed2);
      seed2 = _char + seed1 + seed2 + (seed2 << 5) + 3;
    }
    //Decrypt it
    seed2 = 0xEEEEEEEE;
    while (length-- > 0) { 
      seed2 += encrypt_buffer[0x400 + (seed1 & 0xFF)];
      _char = *pointer ^ (seed1 + seed2);
      seed1  = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
      seed2  = _char + seed2 + (seed2 << 5) + 3;
      *pointer++ = _char;
    }
  __LEAVE_FUNCTION
}

void encrypt_blocktable(uint64_t *pointer, char *key, uint64_t length) {
  __ENTER_FUNCTION
    uint64_t seed1 = 0x7FED7FED;
    uint64_t seed2 = 0xEEEEEEEE;
    uint64_t _char;
    //Prepare seeds
    while (*key != 0) {
      _char = toupper(*key++);
      seed1 = encrypt_buffer[0x300 + _char] ^ (seed1 + seed2);
      seed2 = _char + seed1 + seed2 + (seed2 << 5) + 3;
    }
    //Encrypt it
    seed2 = 0xEEEEEEEE;
    while (length-- > 0) {
      seed2 += encrypt_buffer[0x400 + (seed1 & 0xFF)];
      _char = *pointer;
      *pointer++ = _char + seed2 + (seed2 << 5) + 3;
      seed1  = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
      seed2  = _char + seed2 + (seed2 << 5) + 3;
    }
  __LEAVE_FUNCTION
}

void decrypt_blocktable(uint64_t *pointer, char *key, uint64_t length) {
  __ENTER_FUNCTION
    uint64_t seed1 = 0x7FED7FED;
    uint64_t seed2 = 0xEEEEEEEE;
    uint64_t _char;
    //Prepare seeds
    while (*key != 0) {
      _char = toupper(*key++);
      seed1 = encrypt_buffer[0x300 + _char] ^ (seed1 + seed2);
      seed2 = _char + seed1 + seed2 + (seed2 << 5) + 3;
    }
    //Decrypt it
    seed2 = 0xEEEEEEEE;
    while (length-- > 0) { 
      seed2 += encrypt_buffer[0x400 + (seed1 & 0xFF)];
      _char = *pointer ^ (seed1 + seed2);
      seed1  = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
      seed2  = _char + seed2 + (seed2 << 5) + 3;
      *pointer = _char;
    }
  __LEAVE_FUNCTION
}

uint64_t decrypt_hashindex(archive_t *archive, const char *filename) {
  __ENTER_FUNCTION
    char *key = (char *)filename;
    uint64_t seed1 = 0x7FED7FED;
    uint64_t seed2 = 0xEEEEEEEE;
    uint64_t _char;
    while (*key != 0) {
      _char = toupper(*key++);
      seed1 = encrypt_buffer[0x000 + _char] ^ (seed1 + seed2);
      seed2 = _char + seed1 + seed2 + (seed2 << 5) + 3;
    }
    uint64_t result = (seed1 & (archive->header.hashtable_size -1));
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t decrypt_fileseed(const char *filename) {
  __ENTER_FUNCTION
    char *key = (char *)filename;
    uint64_t seed1 = 0x7FED7FED; //EBX
    uint64_t seed2 = 0xEEEEEEEE; //ESI
    uint64_t _char = 0;
    while (*key != 0) {
      _char = toupper(*key++);
      seed1 = encrypt_buffer[0x300 + _char] ^ (seed1 + seed2);
      seed2 = _char + seed1 + seed2 + (seed2 << 5) + 3;
    }
    return seed1;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t detect_fileseed1(uint64_t *block, uint64_t decrypted) {
  __ENTER_FUNCTION
    uint64_t saveseed1 = 0;
    uint64_t temp = *block ^ decrypted;
    temp -= 0xEEEEEEEE;
    int32_t i;
    for (i = 0; i < 0x100; ++i) {
      uint64_t seed1 = 0;
      uint64_t seed2 = 0xEEEEEEEE;
      uint64_t _char;
      //Try the first uint64_t (We exactly know the value)
      seed1 = temp - encrypt_buffer[0x400 + i];
      seed2 += encrypt_buffer[0x400 + (seed1 & 0xFF)];
      _char = block[0] ^ (seed1 + seed2);
      if (_char != decrypted) continue;
      //Add 1 because we are decrypting block positions
      saveseed1 = seed1 + 1;
      //If OK, continue and test the second value. We don't know exactly the value,
      //but we know that the second one has lower 16 bits set to zero
      //(no compressed block is larger than 0xFFFF bytes) 
      seed1  = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
      seed2  = _char + seed2 + (seed2 << 5) + 3;
      seed2 += encrypt_buffer[0x400 + (seed1 & 0xFF)];
      _char = block[1] ^ (seed1 + seed2);
      if (0 == (_char & 0xFFFF0000)) return saveseed1;
    }
    return 0;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t detect_fileseed2(uint64_t *block, uint32_t dwords, ...) {
  __ENTER_FUNCTION
    va_list arglist;
    uint64_t decrypted[0x10] = {0};
    uint64_t saveseed1;
    uint64_t temp;
    uint64_t i, j;
    //We need at least two DWORDS to detect the seed
    if (dwords < 0x02 || dwords > 0x10) return 0;
    va_start(arglist, dwords);
    temp = (*block ^ decrypted[0]) - 0xEEEEEEEE;
    for (i = 0; i < 0x100; ++i) {
      uint64_t seed1 = 0;
      uint64_t seed2 = 0xEEEEEEEE;
      uint64_t _char;
      //Try the first DWORD
      seed1 = temp - encrypt_buffer[0x400 + i];
      seed2 += encrypt_buffer[0x400 + (seed1 & 0xFF)];
      _char = block[0] ^ (seed1 + seed2);
      if (_char != decrypted[0]) continue;
      saveseed1 = seed1;
      //If OK, continue and test all bytes.
      for (j = 1; j < dwords; ++j) {
        seed1 = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
        seed2 = _char + seed2 + (seed2 << 5) + 3;
        seed2 += encrypt_buffer[0x400 + (seed1 & 0xFF)];
        _char = block[j] ^ (seed1 + seed2);
        if (_char == decrypted[j] && j == dwords - 1) return saveseed1;
      }
    }
    return 0;
  __LEAVE_FUNCTION
    return 0;
}

void encryptblock(uint64_t *pointer, uint64_t length, uint32_t seed1) {
  __ENTER_FUNCTION
    uint64_t seed2 = 0xEEEEEEEE;
    uint64_t _char;
    //Round to DWORDs 
    length >>= 4;
    while (length-- > 0) {
      seed2 += encrypt_buffer[0x400 + (seed1 & 0xFF)];
      _char = *pointer;
      *pointer++ = _char ^ (seed1 + seed2);
      seed1 = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
      seed2 = _char + seed2 + (seed2 << 5) + 3;
    }
  __LEAVE_FUNCTION
}

void decryptblock(uint64_t *pointer, uint64_t length, uint32_t seed1) {
  __ENTER_FUNCTION
    uint64_t seed2 = 0xEEEEEEEE;
    uint64_t _char;
    length >>= 4;
    while (length-- > 0) {
      seed2 += encrypt_buffer[0x400 + (seed1 & 0xFF)];
      _char = *pointer ^ (seed1 + seed2);
      seed1 = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
      seed2 = _char + seed2 + (seed2 << 5) + 3;
      *pointer++ = _char;
    }
  __LEAVE_FUNCTION
}

uint64_t decryptname1(const char *filename) {
  __ENTER_FUNCTION
    char *key = (char *)filename;
    uint64_t seed1 = 0x7FED7FED;
    uint64_t seed2 = 0xEEEEEEEE;
    uint64_t _char;
    while (*key != 0) {
      _char = toupper(*key++);
      seed1 = encrypt_buffer[0x100 + _char] ^ (seed1 + seed2);
      seed2 = _char + seed1 + seed2 + (seed2 << 5) + 3;
    }
    return seed1;
  __LEAVE_FUNCTION
    return 0;
}

uint64_t decryptname2(const char *filename) {
  __ENTER_FUNCTION
    char *key = (char *)filename;
    uint64_t seed1 = 0x7FED7FED;
    uint64_t seed2 = 0xEEEEEEEE;
    uint64_t _char;
    while (*key != 0) {
      _char = toupper(*key++);
      seed1 = encrypt_buffer[0x200 + _char] ^ (seed1 + seed2);
      seed2 = _char + seed1 + seed2 + (seed2 << 5) + 3;
    }
    return seed1;
  __LEAVE_FUNCTION
    return 0;
}

hashkey_t *get_hashentry(archive_t *archive, const char *filename) {
  __ENTER_FUNCTION
    hashkey_t *hashend = 
      archive->hashkey_table + archive->header.hashtable_size;
    hashkey_t *hash0 = NULL;
    hashkey_t *hash = NULL;
    uint64_t index = (uint64_t)(uint64_t *)filename;
    uint64_t name1 = 0, name2 = 0;
    //If filename is given by index, 
    //we have to search all hash entries for the right index. 
    if (index <= archive->header.blocktable_size) {
      //Pass all the hash entries and find the 
      for (hash = archive->hashkey_table; hash < hashend; ++hash) {
        if (hash->blockindex == index) return hash;
      }
      return NULL;
    }
    //Decrypt name and block index 
    index = decrypt_hashindex(archive, filename);
    name1 = decryptname1(filename);
    name2 = decryptname2(filename);
    hash = hash0 = archive->hashkey_table + index;
    //Look for hash index
    while (hash->blockindex != kBlockStatusHashEntryFree) {
      if (hash->name1 == name1 &&
          hash->name2 == name2 &&
          hash->blockindex != kBlockStatusHashEntryDeleted) {
        return hash;
      }
      //Move to the next hash entry
      if (++hash >= hashend) hash = archive->hashkey_table;
      if (hash == hash0) break;
    }
    return NULL;
  __LEAVE_FUNCTION
    return NULL;
}

hashkey_t *find_free_hashentry(archive_t *archive, const char *filename) {
  __ENTER_FUNCTION
    //hashkey_t *hashend = 
    //  archive->hashkey_table + archive->header.hashtable_size;

    hashkey_t *hash0 = NULL;
    hashkey_t *hash = NULL;
    uint64_t index = decrypt_hashindex(archive, filename);
    uint64_t name1 = decryptname1(filename);
    uint64_t name2 = decryptname2(filename);
    uint64_t blockindex = 0xFFFFFFFF;

    //Save the starting hash position
    hash = hash0 = archive->hashkey_table + index;

    //Fill the hash entry with the informations about the file name
    hash->name1 = name1;
    hash->name2 = name2;

    //Now we have to find a free block entry 
    for (index = 0; index < archive->header.blocktable_size; ++index) {
      block_t *block = archive->block_table + index;
      if (0 == (block->flag & PAK_FILE_EXISTS)) {
        blockindex = index;
        break;
      }
    }

    //If no free block entry found, we have to use the index
    //at the end of the current block table
    if (0xFFFFFFFF == blockindex) blockindex = archive->header.blocktable_size;
    hash->blockindex = blockindex;
    return hash;
  __LEAVE_FUNCTION
    return NULL;
}

} //namespace pak
