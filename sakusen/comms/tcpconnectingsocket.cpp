#include <sakusen/comms/tcpconnectingsocket.h>
#include <sakusen/comms/socketexn.h>
#include <sakusen/comms/errorutils.h>

#ifdef WIN32
#include <winsock2.h>
#include <sakusen/comms/wsabsd.h>
#endif

using namespace sakusen::comms;

TCPConnectingSocket::TCPConnectingSocket(
    const sakusen::String hostname, uint16 port
  ) :
  TCPSocket(port),
  remoteHost(hostname)
{
  struct hostent *endpoint;
  
  endpoint = gethostbyname(hostname.c_str());
  
  if (endpoint == NULL) {
    throw HostNotFoundExn(hostname);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  memcpy(&addr.sin_addr.s_addr, endpoint->h_addr, endpoint->h_length);
  addr.sin_port = htons(port);
  if (-1 == connect(sockfd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr))) {
    throw SocketExn(
        "Error connecting TCP socket: "+errorUtils_errorMessage(socketErrno())
      );
  }
}

TCPConnectingSocket::TCPConnectingSocket(
    NativeSocket s,
    const sockaddr_in& peerAddress
  ) :
  TCPSocket(s, peerAddress)
{
#ifdef WIN32
  /** \bug Not thread-safe */
  char* host = inet_ntoa(peerAddress.sin_addr);
#else
  char host[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &peerAddress, host, INET_ADDRSTRLEN);
#endif
  remoteHost = host;
}

