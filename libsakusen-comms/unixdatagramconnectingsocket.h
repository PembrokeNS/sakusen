#ifndef UNIXDATAGRAMCONNECTINGSOCKET_H
#define UNIXDATAGRAMCONNECTINGSOCKET_H

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

#endif // UNIXDATAGRAMCONNECTINGSOCKET_H

