#ifndef LIBSAKUSEN_COMMS__UNIXDATAGRAMLISTENINGSOCKET_H
#define LIBSAKUSEN_COMMS__UNIXDATAGRAMLISTENINGSOCKET_H

#ifndef DISABLE_UNIX_SOCKETS

#include <sakusen/comms/unixdatagramsocket.h>

namespace sakusen {
namespace comms {

class LIBSAKUSEN_COMMS_API UnixDatagramListeningSocket :
  public UnixDatagramSocket {
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

#endif // LIBSAKUSEN_COMMS__UNIXDATAGRAMLISTENINGSOCKET_H

