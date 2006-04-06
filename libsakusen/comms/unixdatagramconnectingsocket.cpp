#ifndef DISABLE_UNIX_SOCKETS

#include "unixdatagramconnectingsocket.h"

#include "errorutils.h"

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
    Fatal("Error connecting socket: " << errorUtils_errorMessage(errno));
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
    Fatal("Error connecting socket (abstract="<<abstract<<", path='"<<path<<
        "', addr.sun_path='"<<addr.sun_path<<"'): " <<
        errorUtils_errorMessage(errno));
  }
}

#endif // DISABLE_UNIX_SOCKETS

