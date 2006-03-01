#include "tcpconnectingsocket.h"

#ifdef WIN32
#include <winsock2.h>
#include "wsabsd.h"
#endif
#include <sys/socket.h>

using namespace sakusen::comms;

TCPConnectingSocket::TCPConnectingSocket(
    const ::String hostname, uint16 port):
  TCPSocket(port),
  remoteHost(hostname)
{
  struct hostent *endpoint;
  
  endpoint = gethostbyname(hostname.c_str());
  
  if (endpoint == NULL) {
    Fatal("host " << hostname << " not found.");
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  memcpy(&addr.sin_addr.s_addr, endpoint->h_addr, endpoint->h_length);
  addr.sin_port = htons(port);
  if (-1 == connect(sockfd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr))) {
    Fatal("Error connecting socket");
  }
}

TCPConnectingSocket::TCPConnectingSocket(
    NativeSocket s,
    const sockaddr_in& peerAddress
  ) :
  TCPSocket(s, peerAddress)
{
  char host[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &peerAddress, host, INET_ADDRSTRLEN);
  remoteHost = host;
}

