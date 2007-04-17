#ifndef IPSOCKET_H
#define IPSOCKET_H

#include "libsakusen-comms-global.h"
#include "socket.h"

#ifdef WIN32
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

class IPSocket : public Socket {
  protected:
    static void interpretAddress(
        std::list<String>& address,
        String* hostname,
        uint16* port
      );
  protected:
    IPSocket() : closed(false) {}
    IPSocket(uint16 myPort) : closed(false), port(myPort) {}
    IPSocket(NativeSocket s, const sockaddr_in& peerAddress) :
      closed(false), addr(peerAddress), sockfd(s), port(ntohs(addr.sin_port))
    {
      assert(sockfd != -1);
    }
  public:
    virtual ~IPSocket() { close(); }
  protected:
    /** whether this socket has been closed already */
    bool closed;
    /** The address of the near endpoint */
    sockaddr_in addr;
    /** The interface to the OS-provided sockets implementation */
    NativeSocket sockfd;
    /** The listening port for a listening Socket; the remote port for a
     * connecting Socket.
     */
    uint16 port;
  public:
    size_t receiveTimeout(void* buf, size_t len, const struct timeval& timeout);
    void close();
    void setNonBlocking(bool val);
    String getAddress() const;
    /** \brief Returns the first section of the sakusen-style address for this
     * socket (e.g. "udp" or "tcp") */
    virtual String getType() const = 0;
};

}}

#endif // IPSOCKET_H
