#include "pf/file/api.h"
#include "pf/net/socket/api.h"

int32_t sys_socket(int32_t domain, int32_t type, int32_t protocol) {
  int32_t result = static_cast<int32_t>(socket(domain, type, protocol));
  return result;
}

namespace pf_net {

namespace socket {

namespace api {

#if __LINUX__
//extern int32_t errno;
#elif __WINDOWS__
int32_t error;
#endif
char errormessage[FILENAME_MAX] = {'\0'};

int32_t socketex(int32_t domain, int32_t type, int32_t protocol) {
  
  int32_t socketid = sys_socket(domain, type, protocol); //remember it

  if (socketid == ID_INVALID) {
#if __LINUX__
    switch (errno) {
      case EPROTONOSUPPORT :
      case EMFILE : 
      case ENFILE : 
      case EACCES : 
      case ENOBUFS : 
      default : {
          break;
      }
    }
#elif __WINDOWS__
    error = WSAGetLastError();
    switch (error) {
      case WSANOTINITIALISED : { 
        strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN : {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEAFNOSUPPORT : {
        strncpy(errormessage, "WSAEAFNOSUPPORT", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS : {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAEMFILE : {
        strncpy(errormessage, "WSAEMFILE", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOBUFS : {
        strncpy(errormessage, "WSAENOBUFS", sizeof(errormessage) - 1);
        break;
      }
      case WSAEPROTONOSUPPORT : {
        strncpy(errormessage, "WSAEPROTONOSUPPORT", sizeof(errormessage) - 1);
        break;
      }
      case WSAEPROTOTYPE : {
        strncpy(errormessage, "WSAEPROTOTYPE", sizeof(errormessage) - 1);
        break;
      }
      case WSAESOCKTNOSUPPORT : {
        strncpy(errormessage, "WSAESOCKTNOSUPPORT", sizeof(errormessage) - 1);
        break;
      }
      default : {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
#endif
  }
  return socketid;
}

bool bindex(int32_t socketid, 
            const struct sockaddr *addr, 
            uint32_t addrlength) {
  if (SOCKET_ERROR == bind(socketid, addr, addrlength)) {
#if __LINUX__
    switch (errno) 
    {
      case EADDRINUSE :
      case EINVAL : 
      case EACCES : 
      case ENOTSOCK : 
      case EBADF : 
      case EROFS : 
      case EFAULT : 
      case ENAMETOOLONG : 
      case ENOENT : 
      case ENOMEM : 
      case ENOTDIR : 
      case ELOOP : 
      default : {
          break;
      }
    }
#elif __WINDOWS__
    error = WSAGetLastError();
    switch (error) {
      case WSANOTINITIALISED : { 
        strncpy(errormessage, "WSAESOCKTNOSUPPORT", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN : {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEADDRINUSE : {
        strncpy(errormessage, "WSAEADDRINUSE", sizeof(errormessage) - 1);
        break;
      }
      case WSAEADDRNOTAVAIL : {
        strncpy(errormessage, "WSAEADDRNOTAVAIL", sizeof(errormessage) - 1);
        break;
      }
      case WSAEFAULT : {
        strncpy(errormessage, "WSAEFAULT", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS : {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINVAL : {
        strncpy(errormessage, "WSAEINVAL", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOBUFS : {
        strncpy(errormessage, "WSAENOBUFS", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTSOCK : {
        strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      default : {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
#endif
    return false;
  }
  return true;
}

bool connectex(int32_t socketid, 
               const struct sockaddr *addr, 
               uint32_t addrlength) {
  if (SOCKET_ERROR == connect(socketid, addr, addrlength)) {
#if __LINUX__
    switch (errno) {
      case EALREADY: 
      case EINPROGRESS: 
      case ECONNREFUSED: 
      case EISCONN: 
      case ETIMEDOUT: 
      case ENETUNREACH: 
      case EADDRINUSE: 
      case EBADF: 
      case EFAULT: 
      case ENOTSOCK: 
      default: {
          break;
      }
    }
#elif __WINDOWS__
    error = WSAGetLastError();
    switch (error) {
      case WSANOTINITIALISED: {
        strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN: {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEADDRINUSE: {
        strncpy(errormessage, "WSAEADDRINUSE", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINTR: {
        strncpy(errormessage, "WSAEINTR", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS: {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAEALREADY: {
        strncpy(errormessage, "WSAEALREADY", sizeof(errormessage) - 1);
        break;
      }
      case WSAEADDRNOTAVAIL: {
        strncpy(errormessage, "WSAEADDRNOTAVAIL", sizeof(errormessage) - 1);
        break;
      }
      case WSAEAFNOSUPPORT: {
        strncpy(errormessage, "WSAEAFNOSUPPORT", sizeof(errormessage) - 1);
        break;
      }
      case WSAECONNREFUSED: {
        strncpy(errormessage, "WSAECONNREFUSED", sizeof(errormessage) - 1);
        break;
      }
      case WSAEFAULT: {
        strncpy(errormessage, "WSAEFAULT", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINVAL: {
        strncpy(errormessage, "WSAEINVAL", sizeof(errormessage) - 1);
        break;
      }
      case WSAEISCONN: {
        strncpy(errormessage, "WSAEISCONN", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETUNREACH: {
        strncpy(errormessage, "WSAENETUNREACH", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOBUFS: {
        strncpy(errormessage, "WSAENOBUFS", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTSOCK: {
        strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      case WSAETIMEDOUT: {
        strncpy(errormessage, "WSAETIMEDOUT", sizeof(errormessage) - 1);
        break;
      }
      case WSAEWOULDBLOCK: {
        strncpy(errormessage, "WSAEWOULDBLOCK", sizeof(errormessage) - 1);
        break;
      }
      default: {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
#endif
    return false;
  }
  return true;
}

bool listenex(int32_t socketid, uint32_t backlog) {
  if (SOCKET_ERROR == listen(socketid, backlog)) {
#if __LINUX__
    switch (errno) {
      case EBADF : 
      case ENOTSOCK :
      case EOPNOTSUPP :
      default : {
          break;
      }
    }
#elif __WINDOWS__
    error = WSAGetLastError();
    switch (error) {
      case WSANOTINITIALISED : {
        strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN : {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEADDRINUSE : {
        strncpy(errormessage, "WSAEADDRINUSE", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS : {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINVAL : {
        strncpy(errormessage, "WSAEINVAL", sizeof(errormessage) - 1);
        break;
      }
      case WSAEISCONN : {
        strncpy(errormessage, "WSAEISCONN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEMFILE : {
        strncpy(errormessage, "WSAEMFILE", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOBUFS : {
        strncpy(errormessage, "WSAENOBUFS", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTSOCK : {
        strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      case WSAEOPNOTSUPP : {
        strncpy(errormessage, "WSAEOPNOTSUPP", sizeof(errormessage) - 1);
        break;
      }
      default : {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
#endif
    return false;
  }
  return true;
}

int32_t acceptex(int32_t socketid, 
                 struct sockaddr *addr, 
                 uint32_t *addrlength) {
  int32_t client = SOCKET_INVALID;
#if __LINUX__
  client = accept(socketid, addr, addrlength);
#elif __WINDOWS__
  client = static_cast<int32_t>(accept(socketid, addr, (int32_t *)addrlength));
#endif
  if (SOCKET_INVALID == client) {
#if __LINUX__
    switch (errno) {
      case EWOULDBLOCK : {
        strncpy(errormessage, "EWOULDBLOCK", sizeof(errormessage) - 1);
        break;
      }
      case ECONNRESET : {
        strncpy(errormessage, "ECONNRESET", sizeof(errormessage) - 1);
        break;
      }
      case ECONNABORTED : {
        strncpy(errormessage, "ECONNABORTED", sizeof(errormessage) - 1);
        break;
      }
      case EPROTO : {
        strncpy(errormessage, "EPROTO", sizeof(errormessage) - 1);
        break;
      }
      case EINTR : {
        // from UNIX Network Programming 2nd, 15.6
        // with nonblocking-socket, ignore above errors
        strncpy(errormessage, "EINTR", sizeof(errormessage) - 1);
        break;
      }
      case EBADF : {
        strncpy(errormessage, "EBADF", sizeof(errormessage) - 1);
        break;
      }
      case ENOTSOCK : {
        strncpy(errormessage, "ENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      case EOPNOTSUPP : {
        strncpy(errormessage, "EOPNOTSUPP", sizeof(errormessage) - 1);
        break;
      }
      case EFAULT : {
        strncpy(errormessage, "EFAULT", sizeof(errormessage) - 1);
        break;
      }
      default : {
        memset(errormessage,'\0',sizeof(errormessage));
        snprintf(errormessage, sizeof(errormessage) - 1, "error: %d", errno);
        break;
      }
    }
#elif __WINDOWS__
    error = WSAGetLastError();
    switch (error) {
      case WSANOTINITIALISED : {
        strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN : {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEFAULT : {
        strncpy(errormessage, "WSAEFAULT", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINTR : {
        strncpy(errormessage, "WSAEINTR", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS : {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINVAL : {
        strncpy(errormessage, "WSAEINVAL", sizeof(errormessage) - 1);
        break;
      }
      case WSAEMFILE : {
        strncpy(errormessage, "WSAEMFILE", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOBUFS : {
        strncpy(errormessage, "WSAENOBUFS", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTSOCK : {
        strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      case WSAEOPNOTSUPP : {
        strncpy(errormessage, "WSAEOPNOTSUPP", sizeof(errormessage) - 1);
        break;
      }
      case WSAEWOULDBLOCK : {
        strncpy(errormessage, "WSAEWOULDBLOCK", sizeof(errormessage) - 1);
        break;
      }
      default : {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
#endif
  } else {
    //do nothing
  }
  return client;
}

bool getsockopt_exb(int32_t socketid, 
                    int32_t level, 
                    int32_t optname, 
                    void *optval, 
                    uint32_t *optlength) {
#if __LINUX__
  if (SOCKET_ERROR == getsockopt(socketid, 
                                 level, 
                                 optname, 
                                 optval, 
                                 optlength)) {
    switch (errno) {
      case EBADF : 
      case ENOTSOCK : 
      case ENOPROTOOPT : 
      case EFAULT : 
      default : {
        break;
      }
    }
    return false;
  }
#elif __WINDOWS__
  if (SOCKET_ERROR == getsockopt(socketid, 
                                 level, 
                                 optname, 
                                 (char *)optval, 
                                 (int32_t *)optlength)) {
    error = WSAGetLastError();
    switch (error) 
    {
      case WSANOTINITIALISED : {
        strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN : {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEFAULT : {
        strncpy(errormessage, "WSAEFAULT", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS : {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINVAL : {
        strncpy(errormessage, "WSAEINVAL", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOPROTOOPT : {
        strncpy(errormessage, "WSAENOPROTOOPT", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTSOCK : {
        strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      default : {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
    return false;
  }
#endif
  return true;
}

uint32_t getsockopt_exu(int32_t socketid, 
                        int32_t level, 
                        int32_t optname, 
                        void *optval, 
                        uint32_t *optlength) {
uint32_t result = 0;
#if __LINUX__
  if (SOCKET_ERROR == getsockopt(socketid, 
                                 level, 
                                 optname, 
                                 optval, 
                                 optlength)) {
    switch (errno) {
      case EBADF: { 
        result = 1;
        break;
      }
      case ENOTSOCK: {
        result = 2;
        break;
      }
      case ENOPROTOOPT: {
        result = 3;
        break;
      }
      case EFAULT: {
        result = 4;
        break;
      }
      default: {
        result = 5;
      }
    }
  }
#elif __WINDOWS__
  if (SOCKET_ERROR == getsockopt(socketid, 
                                 level, 
                                 optname, 
                                 (char *)optval, 
                                 (int32_t *)optlength)) {
    error = WSAGetLastError();
    switch (error) {
      case WSANOTINITIALISED: {
        strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN: {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEFAULT: {
        strncpy(errormessage, "WSAEFAULT", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS: {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINVAL: {
        strncpy(errormessage, "WSAEINVAL", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOPROTOOPT: {
        strncpy(errormessage, "WSAENOPROTOOPT", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTSOCK: {
        strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      default : {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
  }
#endif
  return result;
}

bool setsockopt_ex(int32_t socketid, 
                   int32_t level, 
                   int32_t optname, 
                   const void *optval, 
                   uint32_t optlength) {
  bool result = true;
#if __LINUX__
  if (SOCKET_ERROR == setsockopt(socketid, 
                                 level, 
                                 optname, 
                                 optval, 
                                 optlength)) {
    switch (errno) {
      case EBADF : 
      case ENOTSOCK : 
      case ENOPROTOOPT : 
      case EFAULT : 
      default : {
          break;
      }
    }
    result = false;
  }
#elif __WINDOWS__
  if (SOCKET_ERROR == setsockopt(socketid, 
                                 level, 
                                 optname, 
                                 (char *)optval, 
                                 optlength)) {
    error = WSAGetLastError();
    switch (error) {
      case WSANOTINITIALISED : { 
        strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN : {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEFAULT : {
        strncpy(errormessage, "WSAEFAULT", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS : {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINVAL : {
        strncpy(errormessage, "WSAEINVAL", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETRESET : {
        strncpy(errormessage, "WSAENETRESET", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOPROTOOPT : {
        strncpy(errormessage, "WSAENOPROTOOPT", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTCONN : {
        strncpy(errormessage, "WSAENOTCONN", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTSOCK : {
        strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      default : {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
    result = false;
  }
#endif
  return result;
}

int32_t sendex(int32_t socketid, 
               const void *buffer, 
               uint32_t length, 
               uint32_t flag) {
  int32_t result = 0;
#if __LINUX__
  result = send(socketid, buffer, length, flag);
#elif __WINDOWS__
  result = send(socketid, (const char *)buffer, length, flag);
#endif

  if (SOCKET_ERROR == result) {
#if __LINUX__
    switch (errno) {
      case EWOULDBLOCK: {
        result = SOCKET_ERROR_WOULD_BLOCK;
        break;
      }
      case ECONNRESET:
      case EPIPE:
      case EBADF: 
      case ENOTSOCK: 
      case EFAULT: 
      case EMSGSIZE: 
      case ENOBUFS: 
      default: {
        break;
      }
    }
#elif __WINDOWS__
    error = WSAGetLastError();
    switch (error) {
      case WSANOTINITIALISED : { 
        strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN : {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEACCES : {
        strncpy(errormessage, "WSAEACCES", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINTR : {
        strncpy(errormessage, "WSAEINTR", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS : {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAEFAULT : {
        strncpy(errormessage, "WSAEFAULT", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETRESET : {
        strncpy(errormessage, "WSAENETRESET", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOBUFS : {
        strncpy(errormessage, "WSAENOBUFS", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTCONN : {
        strncpy(errormessage, "WSAENOTCONN", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTSOCK : {
        strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      case WSAEOPNOTSUPP : {
        strncpy(errormessage, "WSAEOPNOTSUPP", sizeof(errormessage) - 1);
        break;
      }
      case WSAESHUTDOWN : {
        strncpy(errormessage, "WSAESHUTDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEWOULDBLOCK : {
        //strncpy(errormessage, "WSAEWOULDBLOCK", sizeof(errormessage) - 1);
        result = SOCKET_ERROR_WOULD_BLOCK;
        break;
      }
      case WSAEMSGSIZE : {
        strncpy(errormessage, "WSAEMSGSIZE", sizeof(errormessage) - 1);
        break;
      }
      case WSAEHOSTUNREACH : {
        strncpy(errormessage, "WSAEHOSTUNREACH", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINVAL : {
        strncpy(errormessage, "WSAEINVAL", sizeof(errormessage) - 1);
        break;
      }
      case WSAECONNABORTED : {
        strncpy(errormessage, "WSAECONNABORTED", sizeof(errormessage) - 1);
        break;
      }
      case WSAECONNRESET : {
        strncpy(errormessage, "WSAECONNRESET", sizeof(errormessage) - 1);
        break;
      }
      case WSAETIMEDOUT : {
        strncpy(errormessage, "WSAETIMEDOUT", sizeof(errormessage) - 1);
        break;
      }
      default : {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
#endif
  } 
  else if (0 == result) {
    //do nothing
  }
  return result;
}

int32_t sendtoex(int32_t socketid, 
                 const void *buffer, 
                 int32_t length, 
                 uint32_t flag, 
                 const struct sockaddr* to, 
                 int32_t tolength) {
  int32_t result = 0;
#if __LINUX__
  result = sendto(socketid, buffer, length, flag, to, tolength);
#elif __WINDOWS__
  result = sendto(socketid, (const char *)buffer,length, flag, to, tolength);
#endif

  if (SOCKET_ERROR == result) {
#if __LINUX__
    switch (errno) {
      case EWOULDBLOCK : {
        result = 0;
        break;
      }
      case ECONNRESET :
      case EPIPE :
      case EBADF : 
      case ENOTSOCK : 
      case EFAULT : 
      case EMSGSIZE : 
      case ENOBUFS : 
      default : {
          break;
      }
    }  
#elif __WINDOWS__
    //do nothing
#endif
  }
  return result;
}

int32_t recvex(int32_t socketid, 
               void *buffer, 
               uint32_t length, 
               uint32_t flag) {

  int32_t result = 0;
#if __LINUX__
  result = recv(socketid, buffer, length, flag);
#elif __WINDOWS__
  result = recv(socketid, (char *)buffer, length, flag);
#endif
  if (SOCKET_ERROR == result) {
#if __LINUX__
    switch (errno) {
      case EWOULDBLOCK : {
        result = SOCKET_ERROR_WOULD_BLOCK;
        break;
      }
      case ECONNRESET :
      case EPIPE :
      case EBADF : 
      case ENOTCONN : 
      case ENOTSOCK : 
      case EINTR : 
      case EFAULT : 

      default : {
        break;
      }
    }
#elif __WINDOWS__
    error = WSAGetLastError();
    switch (error) {
      case WSANOTINITIALISED : { 
        strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN : {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEFAULT : {
        strncpy(errormessage, "WSAEFAULT", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTCONN : {
        strncpy(errormessage, "WSAENOTCONN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINTR : {
        strncpy(errormessage, "WSAEINTR", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS : {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETRESET : {
        strncpy(errormessage, "WSAENETRESET", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTSOCK : {
        strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      case WSAEOPNOTSUPP : {
        strncpy(errormessage, "WSAEOPNOTSUPP", sizeof(errormessage) - 1);
        break;
      }
      case WSAESHUTDOWN : {
        strncpy(errormessage, "WSAESHUTDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEWOULDBLOCK : {
        //strncpy(errormessage, "WSAEWOULDBLOCK", sizeof(errormessage) - 1);
        result = SOCKET_ERROR_WOULD_BLOCK;
        break;
      }
      case WSAEMSGSIZE : {
        strncpy(errormessage, "WSAEMSGSIZE", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINVAL : {
        strncpy(errormessage, "WSAEINVAL", sizeof(errormessage) - 1);
        break;
      }
      case WSAECONNABORTED : {
        strncpy(errormessage, "WSAECONNABORTED", sizeof(errormessage) - 1);
        break;
      }
      case WSAETIMEDOUT : {
        strncpy(errormessage, "WSAETIMEDOUT", sizeof(errormessage) - 1);
        break;
      }
      case WSAECONNRESET : {
        strncpy(errormessage, "WSAECONNRESET", sizeof(errormessage) - 1);
        break;
      }
      default : {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
#endif
  } 
  else if (0 == result) {
    //do nothing
  }
  return result;
}

int32_t recvfrom_ex(int32_t socketid, 
                    void *buffer, 
                    int32_t length, 
                    uint32_t flag, 
                    struct sockaddr *from, 
                    uint32_t *fromlength) {
int32_t result = 0;
#if __LINUX__
  result = recvfrom(socketid, buffer, length, flag, from, fromlength);
#elif __WINDOWS__
  result = 
    recvfrom(socketid, (char*)buffer, length, flag, from, (int32_t*)fromlength);
#endif

  if (SOCKET_ERROR == result) {
#if __LINUX__
    switch (errno) {
      case EWOULDBLOCK : 
        result = SOCKET_ERROR_WOULD_BLOCK;
      case ECONNRESET :
      case EPIPE :
      case EBADF : 
      case ENOTCONN : 
      case ENOTSOCK : 
      case EINTR : 
      case EFAULT : 
      default : {
        break;
      }
    }
#elif __WINDOWS__
    //do nothing
#endif
  }
  return result;
}

bool closeex(int32_t socketid) {
  bool result = true;
#if __LINUX__
  pf_file::api::closeex(socketid);
#elif __WINDOWS__
  if (SOCKET_ERROR == closesocket(socketid)) {
    error = WSAGetLastError();
    switch (error) {
      case WSANOTINITIALISED : {
        strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN : {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTSOCK : {
        strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS : {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINTR : {
        strncpy(errormessage, "WSAEINTR", sizeof(errormessage) - 1);
        break;
      }
      case WSAEWOULDBLOCK : {
        strncpy(errormessage, "WSAEWOULDBLOCK", sizeof(errormessage) - 1);
        break;
      }
      default : {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
    result = false;
  }
#endif
  return result;
}


bool ioctlex(int32_t socketid, int64_t cmd, uint64_t *argp) {
  bool result = true;
#if __LINUX__
  //do nothing
#elif __WINDOWS__
  if (SOCKET_ERROR == ioctlsocket(socketid,(long)cmd,(u_long*)argp)) {
    error = WSAGetLastError();
      switch (error) {
      case WSANOTINITIALISED : {
        strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN : {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS : {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTSOCK : {
        strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      case WSAEFAULT : {
        strncpy(errormessage, "WSAEFAULT", sizeof(errormessage) - 1);
        break;
      }
      default : {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
    result = false;
  }
#endif
  return result;
} 

bool get_nonblocking_ex(int32_t socketid) {
  bool result = true;
#if __LINUX__
  result = pf_file::api::get_nonblocking_ex(socketid);
#elif __WINDOWS__
  USE_PARAM(socketid);
  result = false;
#endif
  return result;
}

bool set_nonblocking_ex(int32_t socketid, bool on) {
  bool result = true;
#if __LINUX__
  pf_file::api::set_nonblocking_ex(socketid, on);
#elif __WINDOWS__
  uint64_t argp = true == on ? 1 : 0;
  result = ioctlex(socketid, FIONBIO, &argp);
#endif
  return result;
}

uint32_t availableex(int32_t socketid) {
  uint32_t result = 0;
#if __LINUX__
  result = pf_file::api::availableex(socketid);
#elif __WINDOWS__
  uint64_t argp = 0;
  ioctlex(socketid, FIONREAD, &argp);
  result = (uint32_t)argp;
#endif
  return result;
}

bool shutdown_ex(int32_t socketid, int32_t how) {
  bool result = true;
  if (shutdown(socketid, how) < 0) {
#if __LINUX__
    switch (errno) {
      case EBADF : 
      case ENOTSOCK : 
      case ENOTCONN : 
      default : {
          break;
      }
    }
#elif __WINDOWS__
    error = WSAGetLastError();
    switch (error) {
      case WSANOTINITIALISED : {
        strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN : {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINVAL : {
        strncpy(errormessage, "WSAEINVAL", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS : {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTCONN : {
        strncpy(errormessage, "WSAENOTCONN", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTSOCK : {
        strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      default : {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
#endif
    result = false;
  }
  return result;
}

int32_t selectex(int32_t maxfdp, 
                 fd_set *readset, 
                 fd_set *writeset, 
                 fd_set *exceptset, 
                 struct timeval *timeout) {
  int32_t result = 0;
  result = select(maxfdp, readset, writeset, exceptset, timeout);
  if(SOCKET_ERROR == result) {
#if __LINUX__

#elif __WINDOWS__
    error = WSAGetLastError();
    switch (error) {
      case WSANOTINITIALISED : {
        strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
        break;
      }
      case WSAEFAULT: {
        strncpy(errormessage, "WSAEFAULT", sizeof(errormessage) - 1);
        break;
      }
      case WSAENETDOWN: {
        strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINVAL: {
        strncpy(errormessage, "WSAEINVAL", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINTR: {
        strncpy(errormessage, "WSAEINTR", sizeof(errormessage) - 1);
        break;
      }
      case WSAEINPROGRESS: {
        strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
        break;
      }
      case WSAENOTSOCK: {
        strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
        break;
      }
      default : {
        strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
        break;
      }
    }
#endif
  }
  return result;
}

int32_t getsockname_ex(int32_t socketid, 
                       struct sockaddr *name, 
                       int32_t *namelength) {
  int32_t result = 0;
#if __LINUX__
  result = 
    getsockname(socketid, name, reinterpret_cast<socklen_t *>(namelength));
#elif __WINDOWS__
  result = 
    getsockname(socketid, name, namelength);
  error = WSAGetLastError();
  switch (error) {
    case WSANOTINITIALISED : {
      strncpy(errormessage, "WSANOTINITIALISED", sizeof(errormessage) - 1);
      break;
    }
    case WSAEFAULT: {
      strncpy(errormessage, "WSAEFAULT", sizeof(errormessage) - 1);
      break;
    }
    case WSAENETDOWN: {
      strncpy(errormessage, "WSAENETDOWN", sizeof(errormessage) - 1);
      break;
    }
    case WSAEINVAL: {
      strncpy(errormessage, "WSAEINVAL", sizeof(errormessage) - 1);
      break;
    }
    case WSAEINTR: {
      strncpy(errormessage, "WSAEINTR", sizeof(errormessage) - 1);
      break;
    }
    case WSAEINPROGRESS: {
      strncpy(errormessage, "WSAEINPROGRESS", sizeof(errormessage) - 1);
      break;
    }
    case WSAENOTSOCK: {
      strncpy(errormessage, "WSAENOTSOCK", sizeof(errormessage) - 1);
      break;
    }
    default : {
      strncpy(errormessage, "UNKNOWN", sizeof(errormessage) - 1);
      break;
    }
  }
#endif
  return result;
}

int32_t getlast_errorcode() {
#if __LINUX__
  return errno;
#elif __WINDOWS__
  return error;
#endif
}

void getlast_errormessage(char *buffer, uint16_t length) {
  snprintf(buffer, length, "%s", errormessage);
}

} //namespace api

} //namespace socket

} //namespace pf_net
