#include "udpsocket.h"
#include "udpconnectingsocket.h"
#include "udplisteningsocket.h"
#include "stringutils.h"
#include "errorutils.h"
#include "socketexn.h"

#ifdef WIN32
#include "wsabsd.h"
#define NativeReceiveReturnType int
#else // BSD sockets
#define NativeReceiveReturnType ssize_t
#endif

using namespace sakusen;
using namespace sakusen::comms;

Socket* UDPSocket::newConnectionToAddress(std::list<String>& address)
{
  String hostname;
  uint16 port;

  interpretAddress(address, &hostname, &port);

  if (hostname.empty()) {
    return NULL;
  }

  return new UDPConnectingSocket(hostname, port);
}

Socket* UDPSocket::newBindingToAddress(std::list<String>& address)
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
  IPSocket(myPort)
{
  sockfd = ::socket(PF_INET, SOCK_DGRAM, 0);
}

void UDPSocket::send(const void* buf, size_t len)
{
  int retVal;
 
  retVal = ::send(sockfd, reinterpret_cast<const char*>(buf), len, 0);
  if (retVal == -1) {
    switch (socket_errno) {
      case ENOTCONN:
      case ECONNREFUSED:
        throw new SocketClosedExn();
        break;
      default:
        Fatal("error " << errorUtils_parseErrno(socket_errno) << " sending message");
        break;
    }
  }
}

void UDPSocket::sendTo(const void* buf, size_t len, const String& address)
{
  std::list<String> splitAddress = stringUtils_split(address, ADDR_DELIM);
  assert(!splitAddress.empty());
  assert(splitAddress.front() == getType());
  splitAddress.pop_front();
  
  String hostname;
  uint16 port;

  interpretAddress(splitAddress, &hostname, &port);
  
  struct hostent *endpoint;
  endpoint = gethostbyname(hostname.c_str());
  
  if (endpoint == NULL) {
    Fatal("host " << hostname << " not found.");
  }

  sockaddr_in dest;

  memset(&dest, 0, sizeof(addr));
  dest.sin_family = AF_INET;
  memcpy(&dest.sin_addr.s_addr, endpoint->h_addr, endpoint->h_length);
  dest.sin_port = htons(port);
  
  int retVal;
 
  retVal = ::sendto(
      sockfd, reinterpret_cast<const char*>(buf), len, 0, reinterpret_cast<sockaddr*>(&dest), sizeof(dest)
    );
  if (retVal == -1) {
    switch (socket_errno) {
      case ENOTCONN:
      case ECONNREFUSED:
        throw new SocketClosedExn();
        break;
      default:
        Fatal("error " << errorUtils_parseErrno(socket_errno) << " sending message");
        break;
    }
  }
}

size_t UDPSocket::receive(void* buf, size_t len)
{
  NativeReceiveReturnType retVal = ::recv(sockfd, reinterpret_cast<char*>(buf), len, 0);
  if (retVal == -1) {
    if (socket_errno == EAGAIN || socket_errno == EWOULDBLOCK) {
      return 0;
    }
    Fatal("error receiving message: " << errorUtils_errorMessage(socket_errno));
  }
  return retVal;
}

size_t UDPSocket::receiveFrom(void* buf, size_t len, String& from)
{
  sockaddr_in fromAddr;
  socklen_t fromLen = sizeof(fromAddr);
  NativeReceiveReturnType retVal = ::recvfrom(
      sockfd, reinterpret_cast<char*>(buf), len, 0, reinterpret_cast<sockaddr*>(&fromAddr), &fromLen
    );
  if (retVal == -1) {
    if (socket_errno == EAGAIN || socket_errno == EWOULDBLOCK) {
      return 0;
    }
    Fatal("error receiving message: " << errorUtils_parseErrno(socket_errno));
  }
#ifdef WIN32
  /** \bug Not thread-safe */
  char* fromChar = inet_ntoa(fromAddr.sin_addr);
#else
  char fromChar[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &fromAddr.sin_addr, fromChar, INET_ADDRSTRLEN);
#endif
  from = getType() + ADDR_DELIM + fromChar + ADDR_DELIM +
    numToString(ntohs(fromAddr.sin_port));
  return retVal;
}

