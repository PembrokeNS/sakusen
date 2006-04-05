#include "udpconnectingsocket.h"
#include "errorutils.h"

#ifdef WIN32
#include <winsock2.h>
#include "wsabsd.h"
#endif

using namespace sakusen::comms;

UDPConnectingSocket::UDPConnectingSocket(
    const String hostname, uint16 port
  ) :
  UDPSocket(port),
  remoteHost(hostname)
{
  struct hostent *endpoint;
  
  endpoint = gethostbyname(hostname.c_str());
  
  if (endpoint == NULL) {
    Fatal("host " << hostname << " not found: " << errorUtils_parseErrno(socket_errno));
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  memcpy(&addr.sin_addr.s_addr, endpoint->h_addr, endpoint->h_length);
  addr.sin_port = htons(port);
  if (-1 == connect(sockfd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr))) {
    Fatal("Error connecting socket");
  }
}

