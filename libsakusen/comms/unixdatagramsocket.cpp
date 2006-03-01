#include "unixdatagramsocket.h"

#include "unixdatagramconnectingsocket.h"
#include "unixdatagramlisteningsocket.h"
#include "errorutils.h"
#include "socketexception.h"
#include "timeutils.h"

#include <sys/socket.h>
#include <fcntl.h>

#include <cerrno>

using namespace std;

using namespace sakusen::comms;

void UnixDatagramSocket::interpretAddress(
    list<String>& address,
    String* path,
    bool* abstract
  )
{
  *path = String();
  if (address.empty()) {
    return;
  }
  String abstractness = address.front();
  address.pop_front();
  
  if (abstractness == "abstract") {
    *abstract = true;
  } else if (abstractness == "concrete") {
    *abstract = false;
  } else {
    return;
  }

  if (address.empty()) {
    return;
  }

  String p = address.front();
  address.pop_front();

  if (!address.empty()) {
    return;
  }

  *path = p;
}

Socket* UnixDatagramSocket::newConnectionToAddress(list<String>& address)
{
  bool abstract;
  String path;

  interpretAddress(address, &path, &abstract);

  if (path.empty()) {
    return NULL;
  }

  return new UnixDatagramConnectingSocket(path, abstract);
}

Socket* UnixDatagramSocket::newBindingToAddress(list<String>& address)
{
  bool abstract;
  String path;

  interpretAddress(address, &path, &abstract);

  if (path.empty()) {
    return NULL;
  }

  return new UnixDatagramListeningSocket(path, abstract);
}

UnixDatagramSocket::UnixDatagramSocket(const char* p, bool a) :
  Socket(),
  abstract(a),
  closed(false)
{
  initialize(p);
}

UnixDatagramSocket::UnixDatagramSocket(const String& p, bool a) :
  Socket(),
  abstract(a),
  closed(false)
{
  initialize(p.c_str());
}

void UnixDatagramSocket::initialize(const char* p)
{
  sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);

  if (sockfd == -1) {
    Fatal("Error creating socket");
  }

  addr.sun_family = AF_UNIX;
  
  if (abstract) {
    memset(path, 0, UNIX_PATH_MAX);
    if (strlen(p) >= UNIX_PATH_MAX-1) {
      Fatal("Socket path too long");
    }

    /* First byte of path must be zero to indicate abstract namespace */
    strncpy(path+1, p, UNIX_PATH_MAX-1);
    memcpy(addr.sun_path, path, UNIX_PATH_MAX);
  } else {
    if (strlen(p) >= UNIX_PATH_MAX) {
      Fatal("Socket path too long");
    }
    
    strncpy(path, p, UNIX_PATH_MAX-1);
    path[UNIX_PATH_MAX-1] = '\0';
    memset(addr.sun_path, 0, UNIX_PATH_MAX);
    strncpy(addr.sun_path, path, UNIX_PATH_MAX-1);
  }
}

void UnixDatagramSocket::send(const void* buf, size_t len)
{
  int retVal = ::send(sockfd, buf, len, 0 /* flags */);
  if (retVal == -1) {
    switch (errno) {
      case EAGAIN:
      /*case EWOULDBLOCK:*/
        /* FIXME: what do we so here?? */
        Fatal("message send must be repeated");
        break;
      case ENOTCONN:
      case ECONNREFUSED:
        throw new SocketClosedExn();
        break;
      default:
        Fatal("error " << errorUtils_parseErrno(errno) << " sending message");
        break;
    }
  }
}

void UnixDatagramSocket::sendTo(
    const void* /*buf*/,
    size_t /*len*/,
    const String& /*address*/
  )
{
  /* TODO: Maybe write this, although I think it won't ever actually be needed
   * */
  Fatal("Not implemeted");
}

size_t UnixDatagramSocket::receive(void* buf, size_t len)
{
  ssize_t retVal = recv(sockfd, buf, len, 0);
  if (retVal == -1) {
    if (errno == EAGAIN) {
      return 0;
    }
    Fatal("error receiving message");
  }
  return retVal;
}

size_t UnixDatagramSocket::receiveTimeout(
    void* buf,
    size_t len,
    const struct timeval& timeout
  )
{
  struct timeval endTime;
  gettimeofday(&endTime, NULL);
  endTime += timeout;
  size_t receivedLength;
  while (0==(receivedLength=receive(buf, len))) {
    struct timeval timeNow;
    gettimeofday(&timeNow, NULL);
    if (timeNow > endTime) {
      return 0;
    }
    /** \todo Don't spinlock here; sleep a bit to be nice. */
  }
  return receivedLength;
}

size_t UnixDatagramSocket::receiveFrom(void* buf, size_t len, String& from)
{
  from.clear();
  return receive(buf, len);
}

void UnixDatagramSocket::close()
{
  if (!closed) {
    if (-1 == ::close(sockfd)) {
      Fatal("Error " << errorUtils_parseErrno(errno) << " closing socket");
    }
    closed = true;
  }
}

void UnixDatagramSocket::setAsynchronous(bool val)
{
  int flags = fcntl(sockfd, F_GETFL);
  if (-1 == flags) {
    Fatal("could not get socket flags");
  }
  if (val) {
    if (-1 == fcntl(sockfd, F_SETFL, O_NONBLOCK | flags)) {
      Fatal("could not set socket flags");
    }
  } else {
    if (-1 == fcntl(sockfd, F_SETFL, (~O_NONBLOCK) & flags)) {
      Fatal("could not set socket flags");
    }
  }
}

String UnixDatagramSocket::getAddress() const {
  if (abstract) {
    return String("unix"ADDR_DELIM"abstract"ADDR_DELIM) + (path+1);
  } else {
    return String("unix"ADDR_DELIM"concrete"ADDR_DELIM) + path;
  }
}

