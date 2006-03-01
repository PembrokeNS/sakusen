#include "ipsocket.h"
#include "errorutils.h"
#include "socketexception.h"
#include "stringutils.h"
#include "timeutils.h"

#include <sys/socket.h>
#include <cerrno>

#ifdef WIN32
#define errno WSAGetLastError()
#define NativeSocketClose(x) closesocket(x)
#else // BSD sockets
#include <fcntl.h>
#define NativeSocketClose(x) ::close(x)
#endif

using namespace sakusen;
using namespace sakusen::comms;

/** \brief Extracts hostname and port from a split sakusen-style IP address
 * \param address the split address with 'udp'/'tcp' removed
 * \param[out] hostname the extracted hostname or a null string if there is a
 * problem interpreting the address
 * \param[out] port the extracted port or undefined if there is a problem
 * interpreting the address */
void IPSocket::interpretAddress(
    std::list<String>& address,
    String* hostname,
    uint16* port
  )
{
  if (address.empty()) {
    *hostname = String();
    return;
  }

  *hostname = address.front();
  address.pop_front();
  if (address.empty()) {
    /* We might have a port in the hostname, after a ':' */
    size_t colon = hostname->rfind(':');
    if (colon == String::npos) {
      *port = DEFAULT_PORT;
    } else {
      *port = numFromString<uint16>(hostname->substr(colon+1));
      *hostname = hostname->substr(0, colon);
    }
  } else {
    String sPort = address.front();
    *port = numFromString<uint16>(sPort);
    address.pop_front();
    if (!address.empty()) {
      *hostname = String();
      return;
    }
  }
}

size_t IPSocket::receiveTimeout(
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

void IPSocket::close()
{
  if (!closed) {
    if (-1 == NativeSocketClose(sockfd)) {
      Fatal("Error " << errorUtils_parseErrno(errno) << " closing socket");
    }
    closed = true;
  }
}

void IPSocket::setAsynchronous(bool val)
{
#ifdef WIN32
  unsigned long flags; = 0ul;
  if (val) flags = 0ul else flags = 1ul;
  if (0 != ioctlsocket(sockfd, FIONBIO, flags))
    Fatal("could not set non-blocking; " << errno);
#else
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
#endif
}

String IPSocket::getAddress() const {
  if (addr.sin_addr.s_addr == INADDR_ANY) {
    /** \todo In theory the hostname / IP address section of this address needs
     * to be appropriate for allowing another computer to connect to this
     * socket when given this address as a string.  I've used the hostname as
     * given by gethostname, although this is presumably non-portable.
     * In practice I don't think this will in fact be used in those
     * circumstances, because it would be broken if the connection were going
     * through a router or some such thin anyway. */
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    return getType() + ADDR_DELIM + hostname + ADDR_DELIM +
      numToString(port);
  } else {
    char host[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, host, INET_ADDRSTRLEN);
    return getType() + ADDR_DELIM + host + ADDR_DELIM +
      numToString(port);
  }
}

