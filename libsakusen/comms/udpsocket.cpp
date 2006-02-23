#include "udpsocket.h"
#include "udpconnectingsocket.h"
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
#endif
using namespace sakusen::comms;

Socket* UDPSocket::newConnectionToAddress(std::list< ::String> address)
{
  ::String hostname, port;
  uint16 nPort;
  if (address.empty())
    return NULL;
  hostname = address.front();
  address.pop_front();
  if (address.empty())
    nPort = 1723;
  else {
    port = address.front();
    nPort = numFromString<uint16>(port);
    address.pop_front();
    if (!address.empty())
      return NULL;
  }

  return new UDPConnectingSocket(hostname, nPort);
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

void UDPSocket::send(const Message& message)
{
  send(message.getBytes(), message.getBytesLength());
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
  /** \todo what should go here for a listening socket? What's the use of this,
   * anyway?
   */
  return ::String("udp:");
}

