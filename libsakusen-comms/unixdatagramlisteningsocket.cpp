#include "unixdatagramlisteningsocket.h"
#include "errorutils.h"

#include <sys/socket.h>
#include <sys/time.h>

#include <cerrno>
#include <sstream>

UnixDatagramListeningSocket::UnixDatagramListeningSocket(bool abstract) :
  UnixDatagramSocket(makeRandomPath(abstract), abstract)
{
  if (-1 ==
      bind(sockfd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)))
  {
    Fatal("Error binding socket");
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
    Fatal("Error binding socket");
  }
}

UnixDatagramListeningSocket::~UnixDatagramListeningSocket()
{
  if (!abstract && -1==unlink(path)) {
    Fatal("Error " << errorUtils_parseErrno(errno) << " removing socket file");
  }
}

String UnixDatagramListeningSocket::makeRandomPath(bool abstract)
{
  std::ostringstream os;
  if (!abstract) {
    /* FIXME: This method of producing a temporary filename is somewhat
     * horrific (the filename is much too long, and it contains the pid, which
     * is a security problem, and it may not be unique).  However, there seems
     * to exist no library method for creating a temporary socket.  All the
     * decent ones (e.g. tempfile(3)) simply open an ordinary file for r/w
     * access. */
    os << "/tmp/";
  }
  struct timeval tv;
  gettimeofday(&tv, NULL);
  os << "sakusen-" << getpid() << "-" << tv.tv_sec << "-" << tv.tv_usec;
  return os.str();
}

