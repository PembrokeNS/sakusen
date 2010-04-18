#include "tcplisteningsocket.h"
#include "tcpconnectingsocket.h"
#include "errorutils.h"

#include <sys/types.h>

#include <cerrno>

/** \todo This value for backlog was chosen entirely arbitrarily - it would be
 * nice if it were confirmed as sane by someone in the know */
#define BACKLOG 10

using namespace sakusen::comms;

TCPListeningSocket::TCPListeningSocket(uint16 port) :
  TCPSocket(port)
{
  memset(&addr, 0, sizeof(addr));
  /** \todo Support binding to fewer than all local interfaces */
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (-1 == bind(sockfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))) {
    SAKUSEN_FATAL(
        "Error binding socket: " << errorUtils_errorMessage(socketErrno())
      );
  }
  if (-1 == listen(sockfd, BACKLOG)) {
    SAKUSEN_FATAL(
        "Error binding socket: " << errorUtils_errorMessage(socketErrno())
      );
  }
}

Socket::Ptr TCPListeningSocket::accept()
{
  sockaddr_in peerAddress;
  socklen_t addrlen = sizeof(peerAddress);
  NativeSocket newSocket;
  
  if (-1 == (newSocket = ::accept(
          sockfd, reinterpret_cast<sockaddr*>(&peerAddress), &addrlen
        ))) {
    switch (socketErrno()) {
      case EAGAIN:
#if EAGAIN != EWOULDBLOCK
      case EWOULDBLOCK:
#endif
#ifndef WIN32 /* Actually this should be on linux only */
      /* According to accept(2), all of the following errors should be treated
       * like EAGAIN */
      case ENETDOWN:
      case EPROTO:
      case ENOPROTOOPT:
      case EHOSTDOWN:
#ifndef __FreeBSD__
      case ENONET:
#endif
      case EHOSTUNREACH:
      case EOPNOTSUPP:
      case ENETUNREACH:
#endif // WIN32
        return Socket::Ptr();
      default:
        SAKUSEN_FATAL(
            "Error accepting new connection: " <<
            errorUtils_errorMessage(socketErrno())
          );
    }
  }

  return Socket::Ptr(new TCPConnectingSocket(newSocket, peerAddress));
}

