#ifndef DISABLE_UNIX_SOCKETS

#include "unixdatagramlisteningsocket.h"
#include "errorutils.h"

#include <sys/socket.h>
#include <sys/time.h>

#include <cerrno>
#include <sstream>

namespace sakusen {
namespace comms {

UnixDatagramListeningSocket::UnixDatagramListeningSocket(bool abstract) :
  UnixDatagramSocket(makeRandomPath(abstract), abstract)
{
  if (-1 ==
      bind(sockfd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)))
  {
    SAKUSEN_FATAL("Error binding socket");
  }
}

UnixDatagramListeningSocket::UnixDatagramListeningSocket(
    const String& path,
    bool abstract) :
  UnixDatagramSocket(path, abstract)
{
  if (-1 ==
      bind(sockfd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)))
  {
    SAKUSEN_FATAL("Error binding socket: " << errorUtils_errorMessage(errno));
  }
}

UnixDatagramListeningSocket::~UnixDatagramListeningSocket()
{
  if (!abstract && -1==unlink(path)) {
    SAKUSEN_FATAL("Error " << errorUtils_parseErrno(errno) << " removing socket file");
  }
}

String UnixDatagramListeningSocket::makeRandomPath(bool abstract)
{
  std::ostringstream os;
  if (!abstract) {
    /** \bug This method of producing a temporary filename is somewhat
     * horrific (the filename is much too long, and it contains the pid, which
     * is a security problem, and there is a tiny
     * chance it may not be unique).  However, there seems
     * to exist no library method for creating a temporary socket.  All the
     * decent ones (e.g. tempfile(3)) simply open an ordinary file for r/w
     * access.
     *
     * \bug We should obtain the temp directory by some more general means
     * (environment variable, or something) */
    os << "/tmp/";
  }
  struct timeval tv;
  gettimeofday(&tv, NULL);
  os << "sakusen-" << getpid() << "-" << tv.tv_sec << "-" << tv.tv_usec;
  return os.str();
}

}}

#endif // DISABLE_UNIX_SOCKETS

