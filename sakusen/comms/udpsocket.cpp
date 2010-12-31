#include <sakusen/comms/udpsocket.h>
#include <sakusen/comms/udpconnectingsocket.h>
#include <sakusen/comms/udplisteningsocket.h>
#include <sakusen/stringutils.h>
#include <sakusen/comms/errorutils.h>
#include <sakusen/comms/socketexn.h>

#ifdef WIN32
#include <sakusen/comms/wsabsd.h>
#define NativeReceiveReturnType int
#else // BSD sockets
#define NativeReceiveReturnType ssize_t
#endif

using namespace std;

using namespace sakusen;
using namespace sakusen::comms;

Socket::Ptr UDPSocket::newConnectionToAddress(std::list<String>& address)
{
  String hostname;
  uint16 port;

  interpretAddress(address, &hostname, &port);

  if (hostname.empty()) {
    return Socket::Ptr();
  }

  return Socket::Ptr(new UDPConnectingSocket(hostname, port));
}

Socket::Ptr UDPSocket::newBindingToAddress(std::list<String>& address)
{
  String hostname;
  uint16 port;

  interpretAddress(address, &hostname, &port);

  if (hostname.empty()) {
    return Socket::Ptr();
  }

  /* hostname is just ignored except for checking the error condition */
  return Socket::Ptr(new UDPListeningSocket(port));
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
    switch (socketErrno()) {
      case ENOTCONN:
      case ECONNREFUSED:
        throw SocketClosedExn();
        break;
      default:
        SAKUSEN_FATAL(
            "error " << errorUtils_parseErrno(socketErrno()) <<
            " sending message"
          );
        break;
    }
  }
}

void UDPSocket::sendTo(const void* buf, size_t len, const String& address)
{
  std::list<String> splitAddress =
    stringUtils_split<list<String> >(address, SAKUSEN_COMMS_ADDR_DELIM);
  assert(!splitAddress.empty());
  assert(splitAddress.front() == getType());
  splitAddress.pop_front();

  String hostname;
  uint16 port;

  interpretAddress(splitAddress, &hostname, &port);

  struct hostent *endpoint;
  endpoint = gethostbyname(hostname.c_str());

  if (endpoint == NULL) {
    SAKUSEN_FATAL("host " << hostname << " not found.");
  }

  sockaddr_in dest;

  memset(&dest, 0, sizeof(addr));
  dest.sin_family = AF_INET;
  memcpy(&dest.sin_addr.s_addr, endpoint->h_addr, endpoint->h_length);
  dest.sin_port = htons(port);

  int retVal;

  retVal = ::sendto(
      sockfd, reinterpret_cast<const char*>(buf), len, 0,
      reinterpret_cast<sockaddr*>(&dest), sizeof(dest)
    );
  if (retVal == -1) {
    switch (socketErrno()) {
      case ENOTCONN:
      case ECONNREFUSED:
        throw SocketClosedExn();
        break;
      default:
        SAKUSEN_FATAL(
            "error " << errorUtils_parseErrno(socketErrno()) <<
            " sending message"
          );
        break;
    }
  }
}

size_t UDPSocket::receive(void* buf, size_t len)
{
  NativeReceiveReturnType retVal = ::recv(sockfd, reinterpret_cast<char*>(buf), len, 0);
  if (retVal == -1) {
    if (socketErrno() == EAGAIN || socketErrno() == EWOULDBLOCK) {
      return 0;
    }
    SAKUSEN_FATAL(
        "error receiving message: " << errorUtils_errorMessage(socketErrno())
      );
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
    if (socketErrno() == EAGAIN || socketErrno() == EWOULDBLOCK) {
      return 0;
    }
    SAKUSEN_FATAL(
        "error receiving message: " << errorUtils_parseErrno(socketErrno())
      );
  }
#ifdef WIN32
  /** \bug Not thread-safe */
  char* fromChar = inet_ntoa(fromAddr.sin_addr);
#else
  char fromChar[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &fromAddr.sin_addr, fromChar, INET_ADDRSTRLEN);
#endif
  from = getType() + SAKUSEN_COMMS_ADDR_DELIM + fromChar +
    SAKUSEN_COMMS_ADDR_DELIM + numToString(ntohs(fromAddr.sin_port));
  return retVal;
}

