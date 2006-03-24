#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include "libsakusen-comms-global.h"
#include "ipsocket.h"

namespace sakusen {
namespace comms {

class UDPSocket : public IPSocket {
  public:
    static Socket* newConnectionToAddress(std::list<String>& address);
    static Socket* newBindingToAddress(std::list<String>& address);
  private:
    /** Copying this would be a bad idea, since the socket would be closed
     * twice, and other such problems.
     */
    UDPSocket(const UDPSocket& copy);
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
    Socket* accept() { Fatal("Not connection-based socket"); return NULL; /* Return statement for the benefit of MSVC */ }
    String getType() const { return "udp"; }
};

}}

#endif // UDPSOCKET_H
