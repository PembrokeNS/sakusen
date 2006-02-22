#ifndef UDPCONNECTINGSOCKET_H
#define UDPCONNECTINGSOCKET_H

#include "udpsocket.h"

namespace sakusen {
namespace comms {

class UDPConnectingSocket : public UDPSocket {
  private:
    UDPConnectingSocket();
    UDPConnectingSocket(const UDPConnectingSocket& copy);
  protected:
    /** The name of the host we are connecting/ed to. */
    ::String remoteHost;
  public:
    UDPConnectingSocket(const String host, uint16 myPort);
    ~UDPConnectingSocket() {}
};

}}

#endif // UDPCONNECTINGSOCKET_H

