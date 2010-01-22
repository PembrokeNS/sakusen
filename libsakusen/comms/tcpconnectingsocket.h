#ifndef LIBSAKUSEN_COMMS__TCPCONNECTINGSOCKET_H
#define LIBSAKUSEN_COMMS__TCPCONNECTINGSOCKET_H

#include "tcpsocket.h"

namespace sakusen {
namespace comms {

class TCPConnectingSocket : public TCPSocket {
  private:
    TCPConnectingSocket();
    TCPConnectingSocket(const TCPConnectingSocket& copy);
  protected:
    /** The name of the host we are connecting/ed to. */
    String remoteHost;
  public:
    /** Connect to given host and port */
    TCPConnectingSocket(const String host, uint16 myPort);
    /** Use given existing socket and peer */
    TCPConnectingSocket(NativeSocket s, const sockaddr_in&);
    ~TCPConnectingSocket() {}

    bool isConnectionBased() { return false; }
    Socket::Ptr accept() {
      SAKUSEN_FATAL("Not connection-based socket");
      return Socket::Ptr(); /* Return statement for the benefit of MSVC */
    }
};

}}

#endif // LIBSAKUSEN_COMMS__TCPCONNECTINGSOCKET_H

