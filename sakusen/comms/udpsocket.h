#ifndef LIBSAKUSEN_COMMS__UDPSOCKET_H
#define LIBSAKUSEN_COMMS__UDPSOCKET_H

#include <sakusen/comms/global.h>
#include <sakusen/comms/ipsocket.h>

namespace sakusen {
namespace comms {

class UDPSocket : public IPSocket {
  public:
    static Socket::Ptr newConnectionToAddress(std::list<String>& address);
    static Socket::Ptr newBindingToAddress(std::list<String>& address);
  protected:
    /** create the socket without binding or connecting */
    UDPSocket();
    /** create the socket and set the port */
    UDPSocket(uint16 myPort);
  public:
    virtual ~UDPSocket() {}
  public:
    void send(const void* buf, size_t len);
    void sendTo(const void* buf, size_t len, const String& address);
    size_t receive(void* buf, size_t len);
    size_t receiveFrom(void* buf, size_t len, String& from);
    bool isConnectionBased() { return false; }
    Socket::Ptr accept() {
      SAKUSEN_FATAL("Not connection-based socket");
      return Socket::Ptr(); /* Return statement for the benefit of MSVC */
    }
    String getType() const { return "udp"; }
};

}}

#endif // LIBSAKUSEN_COMMS__UDPSOCKET_H
