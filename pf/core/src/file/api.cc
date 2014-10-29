#include "pf/file/api.h"

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

#if __LINUX__
extern int32_t errno;
#endif

namespace pf_file {

namespace api {

int32_t openex(const char *filename, int32_t flag) {
#if __LINUX__
  int32_t fd = open(filename, flag);
#elif __WINDOWS__
  int32_t fd = _open(filename, flag);
#endif
  if (fd < 0) {
#if __LINUX__
    switch (errno) {
      case EEXIST : 
      case ENOENT : 
      case EISDIR : 
      case EACCES : 
      case ENAMETOOLONG : 
      case ENOTDIR : 
      case ENXIO : 
      case ENODEV : 
      case EROFS : 
      case ETXTBSY : 
      case EFAULT : 
      case ELOOP : 
      case ENOSPC : 
      case ENOMEM : 
      case EMFILE : 
      case ENFILE : 
      default : {
        break;
      }
    }//end of switch
#elif __WINDOWS__
  // ...
#endif
  }
  return fd;
}

int32_t openmode_ex(const char * filename, int32_t flag, int32_t mode) {
#if __LINUX__
  int32_t fd = open(filename, flag, mode);
#elif __WINDOWS__
  int32_t fd = _open(filename, flag, mode);
#endif

  if (fd < 0) {
#if __LINUX__
    switch (errno) {
      case EEXIST : 
      case EISDIR : 
      case EACCES : 
      case ENAMETOOLONG : 
      case ENOENT : 
      case ENOTDIR : 
      case ENXIO : 
      case ENODEV : 
      case EROFS : 
      case ETXTBSY : 
      case EFAULT : 
      case ELOOP : 
      case ENOSPC : 
      case ENOMEM : 
      case EMFILE : 
      case ENFILE : 
      default : {
        break;
      }
    }//end of switch
#elif __WINDOWS__
  // ...
#endif
  }
  return fd;
}


uint32_t readex(int32_t fd, void *buffer, uint32_t length) {
#if __LINUX__
  int32_t result = read(fd, buffer, length);
#elif __WINDOWS__
  int32_t result = _read (fd, buffer, length);
#endif
  if (result < 0) {

#if __LINUX__
    switch (errno) {
      case EINTR : 
      case EAGAIN : 
      case EBADF : 
      case EIO : 
      case EISDIR : 
      case EINVAL : 
      case EFAULT : 
      case ECONNRESET :
      default : {
        break;
      }
    }
#elif __WINDOWS__
  // ...
#endif
  } 
  else if (0 == result) {

  }
  return result;
}

uint32_t writeex(int32_t fd, const void *buffer, uint32_t length) {
#if __LINUX__
  int32_t result = write(fd, buffer, length);
#elif __WINDOWS__
  int32_t result = _write(fd, buffer, length);
#endif

  if (result < 0) {
    
#if __LINUX__
    switch (errno) {
      case EAGAIN : 
      case EINTR : 
      case EBADF : 
      case EPIPE : 
      case EINVAL: 
      case EFAULT: 
      case ENOSPC : 
      case EIO : 
      case ECONNRESET :
      default : {
          break;
      }
    }
#elif __WINDOWS__
  //...
#endif
  }
  return result;
}


void closeex(int32_t fd) {
 
#if __LINUX__
  close(fd);
  switch ( errno ) {
    case EBADF : 
    default : {
        break;
    }
  }
#elif __WINDOWS__
  _close(fd);
#endif
}

int32_t fcntlex(int32_t fd, int32_t cmd) {
#if __LINUX__
  int32_t result = fcntl(fd, cmd);
  if (result < 0) {
    switch (errno) {
      case EINTR : 
      case EBADF : 
      case EACCES : 
      case EAGAIN : 
      case EDEADLK : 
      case EMFILE : 
      case ENOLCK : 
      default : {
        break;
      }
    }
  }
  return result;
#elif __WINDOWS__
  USE_PARAM(fd);
  USE_PARAM(cmd);
  return 0 ;
#endif
}

int32_t fcntlarg_ex(int32_t fd, int32_t cmd, int32_t arg) {
#if __LINUX__
  int32_t result = fcntl(fd, cmd, arg);
  if (result < 0) {
    switch (errno) {
      case EINTR : 
      case EINVAL : 
      case EBADF : 
      case EACCES : 
      case EAGAIN : 
      case EDEADLK : 
      case EMFILE : 
      case ENOLCK : 
      default : {
        break;
      }
    }
  }
  return result;
#elif __WINDOWS__
  USE_PARAM(fd);
  USE_PARAM(cmd);
  USE_PARAM(arg);
  return 0 ;
#endif
}

bool get_nonblocking_ex(int32_t fd) {
#if __LINUX__
  int32_t flag = fcntlarg_ex(fd, F_GETFL, 0);
  return flag | O_NONBLOCK;
#elif __WINDOWS__
  USE_PARAM(fd);
  return false;
#endif
}

void set_nonblocking_ex(int32_t fd, bool on) {
#if __LINUX__
  int32_t flag = fcntlarg_ex(fd, F_GETFL, 0);
  if (on)
    // make nonblocking fd
    flag |= O_NONBLOCK;
  else
    // make blocking fd
    flag &= ~O_NONBLOCK;
  fcntlarg_ex(fd, F_SETFL, flag);
#elif __WINDOWS__
  USE_PARAM(fd);
  USE_PARAM(on);
  //do nothing
#endif
}

void ioctlex(int32_t fd, int32_t request, void *argp) {
#if __LINUX__
  if (ioctl(fd,request,argp) < 0) {
    switch (errno) {
      case EBADF : 
      case ENOTTY : 
      case EINVAL : 
      default :
      {
        break;
      }
    }
  }
#elif __WINDOWS__
  USE_PARAM(fd);
  USE_PARAM(request);
  USE_PARAM(argp);
  //do nothing
#endif
}



void setnonblocking_ex(int32_t fd, bool on) {
#if __LINUX__
  uint64_t arg = (true == on ? 1 : 0 );
  ioctlex(fd, FIONBIO, &arg);
#elif __WINDOWS__
  USE_PARAM(fd);
  USE_PARAM(on);
  //do nothing
#endif
}


uint32_t availableex(int32_t fd) {
#if __LINUX__
  uint32_t arg = 0;
  ioctlex(fd, FIONREAD, &arg);
  return arg;
#elif __WINDOWS__
  USE_PARAM(fd);
  return 0;
#endif
}


int32_t dupex(int32_t fd) {
#if __LINUX__
  int32_t newfd = dup(fd);
#elif __WINDOWS__
  int32_t newfd = _dup(fd);
#endif

  if (newfd < 0) {
#if __LINUX__
    switch (errno) {
      case EBADF : 
      case EMFILE : 
      default : {
        break;
      }
    }
#elif __WINDOWS__
    //do nothing
#endif
  }
  return newfd;
}


int64_t lseekex(int32_t fd, uint64_t offset, int32_t whence) {
#if __LINUX__
  int64_t result = lseek(fd, offset, whence);
  if (result < 0) {
    switch (errno) {
      case EBADF : 
      case ESPIPE : 
      case EINVAL : 
      default : {
        break;
      }
    }
  }
#elif __WINDOWS__
  uint64_t result = _lseek(fd, (long)offset, whence);
  if ( result < 0 ) {
  }
#endif
  return result;
}

int64_t tellex(int32_t fd) {
  int64_t result = 0;
#if __LINUX__
  //do nothing
#elif __WINDOWS__
  result = _tell(fd);
  if (result < 0) {
  }
#endif
  return result ;
}

bool truncate(const char *filename) {
  FILE *fp = fopen(filename, "w");
  if (NULL == fp) return false;
  fclose(fp);
  return true;
}

} //namespace api

} //namespace pf_file
