#ifndef DISABLE_UNIX_SOCKETS

#include <sakusen/comms/unixdatagramconnectingsocket.h>

#include <sakusen/comms/errorutils.h>
#include <sakusen/comms/socketexn.h>

#include <sys/socket.h>

using namespace sakusen::comms;

UnixDatagramConnectingSocket::UnixDatagramConnectingSocket(
    const char* path,
    bool abstract) :
  UnixDatagramSocket(path, abstract)
{
  if (-1 ==
      connect(sockfd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)))
  {
    throw SocketExn(
        "Error connecting socket: " + errorUtils_errorMessage(errno)
      );
  }
}

UnixDatagramConnectingSocket::UnixDatagramConnectingSocket(
    const String& path,
    bool abstract) :
  UnixDatagramSocket(path, abstract)
{
  if (-1 ==
      connect(sockfd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)))
  {
    throw SocketExn(
        "Error connecting socket: " + errorUtils_errorMessage(errno)
      );
  }
}

#endif // DISABLE_UNIX_SOCKETS

