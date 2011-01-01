#ifndef LIBSAKUSEN_COMMS__TCPLISTENINGSOCKET_H
#define LIBSAKUSEN_COMMS__TCPLISTENINGSOCKET_H

#include <sakusen/comms/tcpsocket.h>

namespace sakusen {
namespace comms {

class TCPListeningSocket : public TCPSocket {
  private:
    TCPListeningSocket();
    TCPListeningSocket(const TCPListeningSocket& copy);
  public:
    TCPListeningSocket(uint16 port);
    ~TCPListeningSocket() {}

    bool isConnectionBased() { return true; }
    Socket::Ptr accept();
};

}}

#endif // LIBSAKUSEN_COMMS__TCPLISTENINGSOCKET_H

