#ifndef UDPLISTENINGSOCKET_H
#define UDPLISTENINGSOCKET_H

#include "udpsocket.h"

namespace sakusen {
namespace comms {

class UDPListeningSocket : public UDPSocket {
  private:
    UDPListeningSocket();
    UDPListeningSocket(const UDPListeningSocket& copy);
  public:
    UDPListeningSocket(uint16 port);
    ~UDPListeningSocket() {}
};

}}

#endif // UDPLISTENINGSOCKET_H

