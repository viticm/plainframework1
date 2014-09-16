/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id file.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/08/19 19:54
 * @uses pak file module
 *       cn: 封装windows和linux统一的文件操作方法
*/
#ifndef PAK_FILE_H_
#define PAK_FILE_H_

#include "pak/config.h"

#if __LINUX__ /* { */

#define FILE_BEGIN SEEK_SET
#define FILE_CURRENT SEEK_CUR
#define FILE_END SEEK_END

#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4

#define FILE_SHARE_READ 0x00000001L
#define GENERIC_WRITE 0x40000000
#define GENERIC_READ 0x80000000

#define FILE_FLAG_DELETE_ON_CLOSE 1
#define FILE_FLAG_SEQUENTIAL_SCAN 2

#endif /* } */

namespace pak {

namespace file {

void *createex(const char *filename, 
               uint64_t mode, 
               uint64_t sharing, 
               void *secondattribute, 
               uint64_t creation, 
               uint64_t flag, 
               void *file);
bool closeex(handle_t fp);
uint64_t getszie(void *fp, uint64_t *offsethigh);
uint64_t setpointer(void *fp, 
                    int64_t offsetlow, 
                    int64_t *offsethigh, 
                    uint64_t method);
bool setend(void *fp);
bool readex(void *fp, 
            void *buffer, 
            uint64_t length, 
            uint64_t *read, 
            void *overlapped);
bool writeex(void *fp, 
             const void *buffer, 
             uint64_t length, 
             uint64_t *write, 
             void *overlapped);
uint64_t getattribute(const char *filename);
void get_temppath(uint64_t length, char *temp);
void get_temp_filename(const char *temp_directorypath, 
                       const char *filename, 
                       uint64_t something, 
                       char *delimiter);
bool removeex(const char *filename);
bool move(const char *source, const char *target);

}; //namespace file

}; //namespace pak

#endif //PAK_FILE_H_
