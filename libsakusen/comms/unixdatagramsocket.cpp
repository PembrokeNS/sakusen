#include "unixdatagramsocket.h"

#include "unixdatagramconnectingsocket.h"
#include "errorutils.h"
#include "socketexception.h"

#include <sys/socket.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>

#include <cerrno>

#define MICRO 1000000

using namespace std;

using namespace sakusen::comms;

Socket* UnixDatagramSocket::newConnectionToAddress(list<String> address)
{
  if (address.empty()) {
    return NULL;
  }
  String abstractness = address.front();
  address.pop_front();
  bool abstract;
  
  if (abstractness == "abstract") {
    abstract = true;
  } else if (abstractness == "concrete") {
    abstract = false;
  } else {
    return NULL;
  }

  if (address.empty()) {
    return NULL;
  }

  String path = address.front();
  address.pop_front();

  if (!address.empty()) {
    return NULL;
  }

  return new UnixDatagramConnectingSocket(path, abstract);
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

/** \brief Closes the socket if it is open */
UnixDatagramSocket::~UnixDatagramSocket()
{
  if (!closed) {
    close();
  }
}

/** \brief Reads \p len bytes of binary from \p buf and sends it.
 * \param[out] buf Must not be NULL.
 */
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
      case 111: /* This errno seems to have no name, but signifies that the
                   socket has closed */
        throw new SocketClosedExn();
        break;
      default:
        Fatal("error " << errorUtils_parseErrno(errno) << " sending message");
        break;
    }
  }
}

/** \brief sends the given ::Message down the line. */
void UnixDatagramSocket::send(const Message& message)
{
  send(message.getBytes(), message.getBytesLength());
}

/** \brief receives a binary stream from the network
 * \param[out] buf Must not be NULL, and should point to a buffer into which \p
 * len bytes can be written.
 * \param len The maximum number of bytes you are willing to see.
 * \return The number of bytes actually read.
 */
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

/** \brief receives a binary stream from the network, with a timeout
 * \param[out] buf Must not be NULL, and should point to a buffer into which \p
 * len bytes can be written.
 * \param len The maximum number of bytes you are willing to see.
 * \param timeout The length of time you are willing to wait for the buffer to
 * fill.
 * \return The number of bytes actually read.
 */
size_t UnixDatagramSocket::receive(
    void* buf,
    size_t len,
    const struct timeval& timeout
  )
{
  struct timeval endTime;
  gettimeofday(&endTime, NULL);
  endTime.tv_sec += timeout.tv_sec;
  endTime.tv_usec += timeout.tv_usec;
  while (endTime.tv_usec >= MICRO) {
    endTime.tv_usec -= MICRO;
    ++endTime.tv_sec;
  }
  size_t receivedLength;
  while (0==(receivedLength=receive(buf, len))) {
    struct timeval timeNow;
    gettimeofday(&timeNow, NULL);
    if (timercmp(&timeNow, &endTime, >=)) {
      return 0;
    }
    /** \todo Don't spinlock here; sleep a bit to be nice. */
  }
  return receivedLength;
}

/** \brief Close a socket, unless it is already closed. */
void UnixDatagramSocket::close()
{
  if (!closed) {
    if (-1 == ::close(sockfd)) {
      Fatal("Error " << errorUtils_parseErrno(errno) << " closing socket");
    }
    closed = true;
  }
}

/** \brief Set the async flag on this socket. */
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

/** \brief Get the address of this socket. */
String UnixDatagramSocket::getAddress() const {
  if (abstract) {
    return String("unix:abstract:") + (path+1);
  } else {
    return String("unix:concrete:") + path;
  }
}

