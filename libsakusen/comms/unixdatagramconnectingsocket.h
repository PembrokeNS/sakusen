#ifndef UNIXDATAGRAMCONNECTINGSOCKET_H
#define UNIXDATAGRAMCONNECTINGSOCKET_H

#ifndef DISABLE_UNIX_SOCKETS

#include "unixdatagramsocket.h"

namespace sakusen {
namespace comms {

class UnixDatagramConnectingSocket : public UnixDatagramSocket {
  private:
    UnixDatagramConnectingSocket();
    UnixDatagramConnectingSocket(const UnixDatagramConnectingSocket& copy);
  public:
    UnixDatagramConnectingSocket(const char* path, bool abstract);
    UnixDatagramConnectingSocket(const String& path, bool abstract);
    ~UnixDatagramConnectingSocket() {}
};

}}

#endif // DISABLE_UNIX_SOCKETS

#endif // UNIXDATAGRAMCONNECTINGSOCKET_H

