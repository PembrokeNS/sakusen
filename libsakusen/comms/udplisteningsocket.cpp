#include "udplisteningsocket.h"
#include "errorutils.h"

#include <sys/types.h>

#include <cerrno>

using namespace sakusen::comms;

UDPListeningSocket::UDPListeningSocket(uint16 port) :
  UDPSocket(port)
{
  memset(&addr, 0, sizeof(addr));
  /** \todo Support binding to fewer than all local interfaces. */
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (-1 == bind(sockfd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)))
    SAKUSEN_FATAL("Error binding socket: " << errorUtils_errorMessage(socket_errno));
}

