#ifndef LIBSAKUSEN_COMMS__UDPLISTENINGSOCKET_H
#define LIBSAKUSEN_COMMS__UDPLISTENINGSOCKET_H

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

#endif // LIBSAKUSEN_COMMS__UDPLISTENINGSOCKET_H

