#ifndef DISABLE_UNIX_SOCKETS

#include <sakusen/comms/unixdatagramsocket.h>

#include <sakusen/comms/unixdatagramconnectingsocket.h>
#include <sakusen/comms/unixdatagramlisteningsocket.h>
#include <sakusen/comms/errorutils.h>
#include <sakusen/comms/socketexn.h>
#include <sakusen/comms/timeutils.h>

#include <sys/socket.h>
#include <fcntl.h>

#include <cerrno>
#include <cstddef>

using namespace std;

namespace sakusen {
namespace comms {

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

Socket::Ptr UnixDatagramSocket::newConnectionToAddress(list<String>& address)
{
  bool abstract;
  String path;

  interpretAddress(address, &path, &abstract);

  if (path.empty()) {
    return Socket::Ptr();
  }

  return Socket::Ptr(new UnixDatagramConnectingSocket(path, abstract));
}

Socket::Ptr UnixDatagramSocket::newBindingToAddress(list<String>& address)
{
  bool abstract;
  String path;

  interpretAddress(address, &path, &abstract);

  if (path.empty()) {
    return Socket::Ptr();
  }

  return Socket::Ptr(new UnixDatagramListeningSocket(path, abstract));
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
  /* There are some crazy issues with the size of the sun_path member, for
   * which there doesn't seem to be a nice solution.  At least we can make sure
   * that we don't write past the end of the struct.  If you get a compile
   * error here you may need to reduce SAKUSEN_COMMS_UNIX_PATH_MAX (q.v.) or
   * take the trouble to solve this problem the Right Way. */
  BOOST_MPL_ASSERT_RELATION(
    SAKUSEN_COMMS_UNIX_PATH_MAX,<=,sizeof(addr.sun_path)
  );
  sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);

  if (sockfd == -1) {
    SAKUSEN_FATAL("Error creating socket");
  }

  memset(&addr, 0, sizeof(sockaddr_un));
  addr.sun_family = AF_UNIX;

  if (abstract) {
    memset(path, 0, SAKUSEN_COMMS_UNIX_PATH_MAX);
    if (strlen(p) >= SAKUSEN_COMMS_UNIX_PATH_MAX-1) {
      SAKUSEN_FATAL("Socket path too long");
    }

    /* First byte of path must be zero to indicate abstract namespace */
    strncpy(path+1, p, SAKUSEN_COMMS_UNIX_PATH_MAX-1);
    memcpy(addr.sun_path, path, SAKUSEN_COMMS_UNIX_PATH_MAX);
  } else {
    if (strlen(p) >= SAKUSEN_COMMS_UNIX_PATH_MAX) {
      SAKUSEN_FATAL("Socket path too long");
    }

    strncpy(path, p, SAKUSEN_COMMS_UNIX_PATH_MAX-1);
    path[SAKUSEN_COMMS_UNIX_PATH_MAX-1] = '\0';
    strncpy(addr.sun_path, path, SAKUSEN_COMMS_UNIX_PATH_MAX-1);
  }
}

void UnixDatagramSocket::send(const void* buf, size_t len)
{
  int retVal = ::send(sockfd, buf, len, 0 /* flags */);
  if (retVal == -1) {
    switch (errno) {
      case EAGAIN:
      /*case EWOULDBLOCK:*/
        /** \bug What do we so here?? */
        SAKUSEN_FATAL("message send must be repeated");
        break;
      case ENOTCONN:
      case ECONNREFUSED:
        throw SocketClosedExn();
        break;
      default:
        SAKUSEN_FATAL("error " << errorUtils_parseErrno(errno) << " sending message");
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
  /* \todo Maybe write this, although I think it won't ever actually be needed
   * */
  SAKUSEN_FATAL("Not implemeted");
}

size_t UnixDatagramSocket::receive(void* buf, size_t len)
{
  ssize_t retVal = recv(sockfd, buf, len, 0);
  if (retVal == -1) {
    if (errno == EAGAIN) {
      return 0;
    }
    SAKUSEN_FATAL("error receiving message");
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
      SAKUSEN_FATAL("Error " << errorUtils_parseErrno(errno) << " closing socket");
    }
    closed = true;
  }
}

void UnixDatagramSocket::setNonBlocking(bool val)
{
  int flags = fcntl(sockfd, F_GETFL);
  if (-1 == flags) {
    SAKUSEN_FATAL("could not get socket flags");
  }
  if (val) {
    if (-1 == fcntl(sockfd, F_SETFL, O_NONBLOCK | flags)) {
      SAKUSEN_FATAL("could not set socket flags");
    }
  } else {
    if (-1 == fcntl(sockfd, F_SETFL, (~O_NONBLOCK) & flags)) {
      SAKUSEN_FATAL("could not set socket flags");
    }
  }
}

String UnixDatagramSocket::getAddress() const {
  if (abstract) {
    return String(
        "unix"SAKUSEN_COMMS_ADDR_DELIM"abstract"SAKUSEN_COMMS_ADDR_DELIM
      ) + (path+1);
  } else {
    return String(
        "unix"SAKUSEN_COMMS_ADDR_DELIM"concrete"SAKUSEN_COMMS_ADDR_DELIM
      ) + path;
  }
}

}}

#endif // DISABLE_UNIX_SOCKETS

