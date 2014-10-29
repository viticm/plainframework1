/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id api.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/19 14:12
 * @uses file extend apis
 */
#ifndef PF_FILE_API_H_
#define PF_FILE_API_H_

#include "pf/file/config.h"

#if __LINUX__ /* { */
#include <sys/types.h>  // for open()
#include <sys/stat.h>   // for open()
#include <unistd.h>     // for fcntl()
#include <fcntl.h>      // for fcntl()
#include <sys/ioctl.h>  // for ioctl()
#include <errno.h>      // for errno
#elif __WINDOWS__ /* }{ */
#include <io.h>         // for _open()
#include <fcntl.h>      // for _open()/_close()/_read()/_write()...
#endif /* } */

namespace pf_file {

namespace api {

PF_API int32_t openex(const char *filename, int32_t flag);
PF_API int32_t openmode_ex(const char *filename, int32_t flag, int32_t mode);
PF_API void closeex(int32_t fd);
PF_API uint32_t readex(int32_t fd, void *buffer, uint32_t length);
PF_API uint32_t writeex(int32_t fd, const void *buffer, uint32_t length);
PF_API int32_t fcntlex(int32_t fd, int32_t cmd);
PF_API int32_t fcntlarg_ex(int32_t fd, int32_t cmd, int32_t arg);
PF_API bool get_nonblocking_ex(int32_t socketid);
PF_API void set_nonblocking_ex(int32_t socketid, bool on);
PF_API void ioctlex(int32_t fd, int32_t request, void *argp);
PF_API uint32_t availableex(int32_t fd);
PF_API int32_t dupex(int32_t fd);
PF_API int64_t lseekex(int32_t fd, uint64_t offset, int32_t whence);
PF_API int64_t tellex(int32_t fd);
PF_API bool truncate(const char *filename);

}; //namespace api

}; //namespace pf_file

#endif //PF_FILE_API_H_
