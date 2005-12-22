#ifndef UNIXDATAGRAMLISTENINGSOCKET_H
#define UNIXDATAGRAMLISTENINGSOCKET_H

#include "unixdatagramsocket.h"

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

#endif // UNIXDATAGRAMLISTENINGSOCKET_H

