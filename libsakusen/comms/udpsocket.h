#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include "libsakusen-comms-global.h"
#include "socket.h"

#ifdef WINDOWS
#include "win32.h"
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

namespace sakusen {
namespace comms {

#ifdef WIN32
typedef int socklen_t;
typedef SOCKET NativeSocket;

#else // use BSD sockets
typedef int NativeSocket;
#endif

class UDPSocket : public Socket {
  private:
    static void interpretAddress(
        std::list<String>& address,
        String* hostname,
        uint16* port
      );
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
    virtual ~UDPSocket() { close(); }
  protected:
    /** whether this socket has been closed already */
    bool closed;
    /** The address of the near endpoint */
    sockaddr_in addr;
    /** The interface to the OS-provided sockets implementation */
    NativeSocket sockfd;
    /** The listening port for a ::UDPListeningSocket; the remote port for a
     * ::UDPConnectingSocket.
     */
    uint16 port;
  public:
    void send(const void* buf, size_t len);
    size_t receive(void* buf, size_t len);
    size_t receive(void* buf, size_t len, const struct timeval& timeout);
    size_t receiveFrom(void* buf, size_t len, String& from);
    void close();
    void setAsynchronous(bool val);
    String getAddress() const;
};

}}

#endif // UDPSOCKET_H
