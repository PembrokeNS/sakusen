#ifndef UNIXDATAGRAMLISTENINGSOCKET_H
#define UNIXDATAGRAMLISTENINGSOCKET_H

#ifndef DISABLE_UNIX_SOCKETS

#include "unixdatagramsocket.h"

namespace sakusen {
namespace comms {

class UnixDatagramListeningSocket : public UnixDatagramSocket {
  private:
    UnixDatagramListeningSocket();
    UnixDatagramListeningSocket(const UnixDatagramListeningSocket& copy);
  public:
    UnixDatagramListeningSocket(bool abstract);
      /* Creates a socket with a "random" name */
    UnixDatagramListeningSocket(const String& path, bool abstract);
    ~UnixDatagramListeningSocket();
  private:
    static String makeRandomPath(bool abstract);
};

}}

#endif // DISABLE_UNIX_SOCKETS

#endif // UNIXDATAGRAMLISTENINGSOCKET_H

