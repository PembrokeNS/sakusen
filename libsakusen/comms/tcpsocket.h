#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "libsakusen-comms-global.h"
#include "ipsocket.h"

namespace sakusen {
namespace comms {

class TCPSocket : public IPSocket {
  public:
    static Socket* newConnectionToAddress(std::list<String>& address);
    static Socket* newBindingToAddress(std::list<String>& address);
  private:
    /** Copying this would be a bad idea, since the socket would be closed
     * twice, and other such problems.
     */
    TCPSocket(const TCPSocket& copy);
  protected:
    /** create the socket without binding or connecting */
    TCPSocket();
    /** create the socket and set the port */
    TCPSocket(uint16 myPort);
    /** create the socket around the given socket and address */
    TCPSocket(NativeSocket s, const sockaddr_in& peerAddress);
  public:
    virtual ~TCPSocket() { delete[] buffer; }
  private:
    uint8* buffer;
    size_t bufferLength;
    size_t bufferCapacity;
  public:
    void send(const void* buf, size_t len);
    void sendTo(const void* buf, size_t len, const String& address);
    size_t receive(void* buf, size_t len);
    size_t receiveFrom(void* buf, size_t len, String& from);
    String getType() const { return "tcp"; }
};

}}

#endif // TCPSOCKET_H
