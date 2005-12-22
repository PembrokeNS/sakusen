#include "unixdatagramconnectingsocket.h"

#include <sys/socket.h>

UnixDatagramConnectingSocket::UnixDatagramConnectingSocket(
    const char* path,
    bool abstract) :
  UnixDatagramSocket(path, abstract)
{
  if (-1 ==
      connect(sockfd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)))
  {
    Fatal("Error connecting socket");
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
    Fatal("Error connecting socket");
  }
}

