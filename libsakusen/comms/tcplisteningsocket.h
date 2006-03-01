#ifndef TCPLISTENINGSOCKET_H
#define TCPLISTENINGSOCKET_H

#include "tcpsocket.h"

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
    Socket* accept();
};

}}

#endif // UDPLISTENINGSOCKET_H

