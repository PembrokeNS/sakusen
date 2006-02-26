#include "udpsocket.h"
#include "udpconnectingsocket.h"
#include "udplisteningsocket.h"
#include "errorutils.h"
#include "socketexception.h"
#include "stringutils.h"

#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <cerrno>

#define MICRO 1000000

#ifdef WIN32
#define errno WSAGetLastError()
#define NativeSocketClose(x) closesocket(x)
#else // BSD sockets
#include <fcntl.h>
#define NativeSocketClose(x) ::close(x)
#define NativeSocketRecv(a,b,c,d) ::recv(a,b,c,d)
#define NativeSocketRecvFrom(a,b,c,d,e,f) ::recvfrom(a,b,c,d,e,f)
#endif

using namespace sakusen;
using namespace sakusen::comms;

/** \brief Extracts hostname and port from a split sakusen-style UDP address
 * \param address the split address with 'udp' removed
 * \param[out] hostname the extracted hostname or a null string if there is a
 * problem interpreting the address
 * \param[out] port the extracted port or undefined if there is a problem
 * interpreting the address */
void UDPSocket::interpretAddress(
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

Socket* UDPSocket::newConnectionToAddress(std::list< ::String>& address)
{
  String hostname;
  uint16 port;

  interpretAddress(address, &hostname, &port);

  if (hostname.empty()) {
    return NULL;
  }

  return new UDPConnectingSocket(hostname, port);
}

Socket* UDPSocket::newBindingToAddress(std::list< ::String>& address)
{
  String hostname;
  uint16 port;

  interpretAddress(address, &hostname, &port);

  if (hostname.empty()) {
    return NULL;
  }

  /* hostname is just ignored except for checking the error condition */
  return new UDPListeningSocket(port);
}

UDPSocket::UDPSocket()
{
  sockfd = ::socket(PF_INET, SOCK_DGRAM, 0);
}

UDPSocket::UDPSocket(uint16 myPort):
  port(myPort)
{
  sockfd = ::socket(PF_INET, SOCK_DGRAM, 0);
}

void UDPSocket::send(const void* buf, size_t len)
{
  int retVal;
 
  retVal = ::send(sockfd, buf, len, 0);
  if (retVal == -1) {
    switch (errno) {
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

size_t UDPSocket::receive(void* buf, size_t len)
{
  ssize_t retVal = NativeSocketRecv(sockfd, buf, len, 0);
  if (retVal == -1) {
    if (errno == EAGAIN) {
      return 0;
    }
    Fatal("error receiving message");
  }
  return retVal;
}

size_t UDPSocket::receive(
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

size_t UDPSocket::receiveFrom(void* buf, size_t len, String& from)
{
  sockaddr_in fromAddr;
  socklen_t fromLen = sizeof(fromAddr);
  ssize_t retVal = NativeSocketRecvFrom(
      sockfd, buf, len, 0, reinterpret_cast<sockaddr*>(&fromAddr), &fromLen
    );
  if (retVal == -1) {
    if (errno == EAGAIN) {
      return 0;
    }
    Fatal("error receiving message");
  }
  char fromChar[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &fromAddr.sin_addr, fromChar, INET_ADDRSTRLEN);
  from = String("udp"ADDR_DELIM) + fromChar + ADDR_DELIM +
    numToString(ntohs(fromAddr.sin_port));
  return retVal;
}

void UDPSocket::close()
{
  if (!closed) {
    if (-1 == NativeSocketClose(sockfd)) {
      Fatal("Error " << errorUtils_parseErrno(errno) << " closing socket");
    }
    closed = true;
  }
}

void UDPSocket::setAsynchronous(bool val)
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

String UDPSocket::getAddress() const {
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
    return ::String("udp"ADDR_DELIM) + hostname + ADDR_DELIM +
      numToString(port);
  } else {
    char host[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, host, INET_ADDRSTRLEN);
    return ::String("udp"ADDR_DELIM) + host + ADDR_DELIM +
      numToString(port);
  }
}

