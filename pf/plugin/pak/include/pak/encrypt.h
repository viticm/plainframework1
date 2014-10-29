/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id encrypt.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/08/20 14:09
 * @uses pak encrypt module
*/
#ifndef PAK_ENCRYPT_H_
#define PAK_ENCRYPT_H_

#include "pak/config.h"

#define HELPER_BUFFER_SIZE 0x500

namespace pak {

int32_t preparebuffers();
void encrypt(uint64_t *pointer, char *key, uint64_t length);
void decrypt(uint64_t *pointer, char *key, uint64_t length);
void encrypt_blocktable(uint64_t *pointer, char *key, uint64_t length);
void decrypt_blocktable(uint64_t *pointer, char *key, uint64_t length);
uint64_t decrypt_hashindex(archive_t *archive, const char *filename);
uint64_t detect_fileseed1(uint64_t *block, uint64_t decrypted);
uint64_t detect_fileseed2(uint64_t *block, uint32_t dwords, ...);
uint64_t decrypt_fileseed(const char *filename);
void encryptblock(uint64_t *pointer, uint64_t length, uint32_t seed);
void decryptblock(uint64_t *pointer, uint64_t length, uint32_t seed);
uint64_t decryptname1(const char *filename);
uint64_t decryptname2(const char *filename);
hashkey_t *get_hashentry(archive_t *archive, const char *filename);
hashkey_t *find_free_hashentry(archive_t *archive, const char *filename);
extern bool encrypt_buffer_created;
extern uint64_t encrypt_buffer[HELPER_BUFFER_SIZE];

}; //namespace pak

#endif //PAK_ENCRYPT_H_
