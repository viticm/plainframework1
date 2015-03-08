#include "pf/base/string.h"
#include "pf/net/socket/base.h"

namespace pf_net {

namespace socket {

Base::Base() {
  __ENTER_FUNCTION
    socketid_ = SOCKET_INVALID;
    memset(host_, '\0', sizeof(host_));
    port_ = 0;
  __LEAVE_FUNCTION
}

Base::Base(const char *host, uint16_t port) {
  __ENTER_FUNCTION
    using namespace pf_base;
    memset(host_, '\0', sizeof(host_));
    if (host != NULL) string::safecopy(host_, host, sizeof(host_));      
    port_ = port;
    create();
  __LEAVE_FUNCTION
}

Base::~Base() {
  __ENTER_FUNCTION
    close();
  __LEAVE_FUNCTION
}

bool Base::create() {
  __ENTER_FUNCTION
    bool result = true;
    socketid_ = api::socketex(AF_INET, SOCK_STREAM, 0);
    result = isvalid();
    return result;
  __LEAVE_FUNCTION
    return false;
}

void Base::close() {
  __ENTER_FUNCTION
    if (isvalid() && !iserror())
      api::closeex(socketid_);
    socketid_ = SOCKET_INVALID;
    memset(host_, '\0', sizeof(host_));
    port_ = 0;
  __LEAVE_FUNCTION
}

bool Base::connect() {
  __ENTER_FUNCTION
    bool result = true;
    struct sockaddr_in connect_sockaddr_in;
    memset(&connect_sockaddr_in, 0, sizeof(connect_sockaddr_in));
    connect_sockaddr_in.sin_family = AF_INET;
    connect_sockaddr_in.sin_addr.s_addr = inet_addr(host_);
#if __WINDOWS__
	if (0 == strcmp(host_, "0.0.0.0")) {
      connect_sockaddr_in.sin_addr.s_addr = inet_addr("127.0.0.1");
	}
#endif
    connect_sockaddr_in.sin_port = htons(port_);
    result = api::connectex(
        socketid_, 
        reinterpret_cast<const struct sockaddr*>(&connect_sockaddr_in), 
        sizeof(connect_sockaddr_in));
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Base::connect(const char *host, uint16_t port) {
  __ENTER_FUNCTION
    using namespace pf_base;
    bool result = true;
    if (host != NULL)
      string::safecopy(host_, host, sizeof(host_));
    port_ = port;
    result = connect();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Base::reconnect(const char *host, uint16_t port) {
  __ENTER_FUNCTION
    using namespace pf_base;
    bool result = true;
    close();
    if (host != NULL)
      string::safecopy(host_, host, sizeof(host_));
    port_ = port;
    create();
    result = connect();
    return result;
  __LEAVE_FUNCTION
    return false;
}

int32_t Base::send(const void *buffer, uint32_t length, uint32_t flag) {
  __ENTER_FUNCTION
    int32_t result = 0;
    result = api::sendex(socketid_, buffer, length, flag);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

int32_t Base::receive(void *buffer, uint32_t length, uint32_t flag) {
  __ENTER_FUNCTION
    int32_t result = 0;
    result = api::recvex(socketid_, buffer, length, flag);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint32_t Base::available() const {
  __ENTER_FUNCTION
    uint32_t result = 0;
    result = api::availableex(socketid_);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

int32_t Base::accept(struct sockaddr_in *accept_sockaddr_in) {
  __ENTER_FUNCTION
    int32_t result = SOCKET_ERROR;
    uint32_t addrlength = 0;
    addrlength = sizeof(struct sockaddr_in);
    result = api::acceptex(
        socketid_, 
        reinterpret_cast<struct sockaddr *>(accept_sockaddr_in),
        &addrlength);
    return result;
  __LEAVE_FUNCTION
    return SOCKET_ERROR;
}

bool Base::bind(const char *ip) {
  __ENTER_FUNCTION
    bool result = true;
    struct sockaddr_in connect_sockaddr_in;
    connect_sockaddr_in.sin_family = AF_INET;
    if (NULL == ip || 
        0 == strlen(ip) || 
        0 == strcmp("127.0.0.1", ip) ||
        0 == strcmp("0.0.0.0", ip)) {
      connect_sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
      connect_sockaddr_in.sin_addr.s_addr = inet_addr(ip);
    }
    connect_sockaddr_in.sin_port = htons(port_);
    result = api::bindex(
        socketid_, 
        reinterpret_cast<const struct sockaddr*>(&connect_sockaddr_in), 
        sizeof(connect_sockaddr_in));
    if (0 == port_) {
      int32_t inlength = sizeof(connect_sockaddr_in);
      if (SOCKET_ERROR == 
          api::getsockname_ex(socketid_, 
            reinterpret_cast<struct sockaddr*>(&connect_sockaddr_in), 
            &inlength)) {
        ERRORPRINTF("[net.socket] (socket::Base::bind) error, can't get port");
        return false;
      }
      port_ = ntohs(connect_sockaddr_in.sin_port);
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Base::bind(uint16_t port, const char *ip) {
  __ENTER_FUNCTION
    bool result = true;
    port_ = port;
    result = bind(ip);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Base::listen(uint32_t backlog) {
  __ENTER_FUNCTION
    bool result = true;
    result = api::listenex(socketid_, backlog);
    return result;
  __LEAVE_FUNCTION
    return false;
}
int32_t Base::select(int32_t maxfdp, 
                     fd_set *readset, 
                     fd_set *writeset, 
                     fd_set *exceptset,
                     timeval *timeout) {
  __ENTER_FUNCTION
    int32_t result = SOCKET_ERROR;
    result = api::selectex(maxfdp, readset, writeset, exceptset, timeout);
    return result;
  __LEAVE_FUNCTION
    return SOCKET_ERROR;
}

uint32_t Base::getlinger() const {
  __ENTER_FUNCTION
    uint32_t result = 0;
    struct linger getlinger;
    uint32_t length = sizeof(getlinger);
    api::getsockopt_exb(socketid_, SOL_SOCKET, SO_LINGER, &getlinger, &length);
    result = getlinger.l_linger;
    return result;
  __LEAVE_FUNCTION
    return 0;
}

bool Base::setlinger(uint32_t lingertime) {
  __ENTER_FUNCTION
    bool result = true;
    struct linger setlinger;
    setlinger.l_onoff = lingertime > 0 ? 1 : 0;
    setlinger.l_linger = static_cast<uint16_t>(lingertime);
    result = api::setsockopt_ex(socketid_, 
                                SOL_SOCKET, 
                                SO_LINGER, 
                                &setlinger, 
                                sizeof(setlinger));
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Base::is_reuseaddr() const {
  __ENTER_FUNCTION
    bool result = true;
    int32_t reuse = 0;
    uint32_t length = sizeof(reuse);
    result = api::getsockopt_exb(socketid_, 
                                 SOL_SOCKET, 
                                 SO_REUSEADDR, 
                                 &reuse, 
                                 &length);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Base::set_reuseaddr(bool on) {
  __ENTER_FUNCTION
    bool result = true;
    int32_t option = true == on ? 1 : 0;
    result = api::setsockopt_ex(socketid_, 
                                SOL_SOCKET, 
                                SO_REUSEADDR, 
                                &option, 
                                sizeof(option));
    return result;
  __LEAVE_FUNCTION
    return false;
}

uint32_t Base::getlast_errorcode() const {
  __ENTER_FUNCTION
    uint32_t result = 0;
    result = api::getlast_errorcode();
    return result;
  __LEAVE_FUNCTION
    return 0;
}

void Base::getlast_errormessage(char *buffer, uint16_t length) const {
  __ENTER_FUNCTION
    api::getlast_errormessage(buffer, length);
  __LEAVE_FUNCTION
}

bool Base::iserror() const {
  __ENTER_FUNCTION
    bool result = true;
    int32_t option_value = 0;
    uint32_t option_length = sizeof(option_value);
    api::getsockopt_exu(socketid_, 
                        SOL_SOCKET, 
                        SO_ERROR, 
                        &option_value,
                        &option_length);
    result = 0 == option_value ? false : true;
    return result;
  __LEAVE_FUNCTION
    return true;
}

bool Base::is_nonblocking() const {
  __ENTER_FUNCTION
    bool result = true;
    result = api::get_nonblocking_ex(socketid_);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Base::set_nonblocking(bool on) {
  __ENTER_FUNCTION
    bool result = true;
    result = api::set_nonblocking_ex(socketid_, on);
    return result;
  __LEAVE_FUNCTION
    return false;
}

uint32_t Base::getreceive_buffersize() const {
  __ENTER_FUNCTION
    uint32_t option_value = 0;
    uint32_t option_length = sizeof(option_value);
    api::getsockopt_exb(socketid_, 
                        SOL_SOCKET, 
                        SO_RCVBUF, 
                        &option_value, 
                        &option_length);
    return option_value;
  __LEAVE_FUNCTION
    return 0;
}

bool Base::setreceive_buffersize(uint32_t size) {
  __ENTER_FUNCTION
    bool result = true;
    result = 
      api::setsockopt_ex(socketid_, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
    return result;
  __LEAVE_FUNCTION
    return false;
}

uint32_t Base::getsend_buffersize() const {
  __ENTER_FUNCTION
    uint32_t option_value = 0;
    uint32_t option_length = sizeof(option_value);
    api::getsockopt_exb(socketid_, 
                        SOL_SOCKET, 
                        SO_SNDBUF, 
                        &option_value, 
                        &option_length);
    return option_value;
  __LEAVE_FUNCTION
    return 0;
}

bool Base::setsend_buffersize(uint32_t size) {
  __ENTER_FUNCTION
    bool result = true;
    result = 
      api::setsockopt_ex(socketid_, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
    return result;
  __LEAVE_FUNCTION
    return false;
}

uint16_t Base::getport() const {
    return port_;
}

uint64_t Base::getuint64host() const {
  __ENTER_FUNCTION
    uint64_t result = 0;
    if (0 == strlen(host_)) {
      result = static_cast<uint64_t>(htonl(INADDR_ANY));
    } else {
      result = static_cast<uint64_t>(inet_addr(host_));
    }
    return result;
  __LEAVE_FUNCTION
    return 0;
}

bool Base::isvalid() const {
  __ENTER_FUNCTION
    bool result = true;
    result = socketid_ != SOCKET_INVALID;
    return result;
  __LEAVE_FUNCTION
    return false;
}

int32_t Base::getid() const {
  __ENTER_FUNCTION
    int32_t result = SOCKET_INVALID;
    result = socketid_;
    return result;
  __LEAVE_FUNCTION
    return SOCKET_INVALID;
}

} //namespace socket

} //namespace pf_net
