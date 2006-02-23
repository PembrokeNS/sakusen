#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>

#include "message.h"

namespace sakusen {
namespace comms {


/** This is a generic class for representing any type of socket.
 * Socket::newConnectionToAddress(), when given a Sakusen-format
 * address, will automatically open the right type of socket and connect
 * to the given address. */
class Socket {
  public:
    static Socket* newConnectionToAddress(const String& address);
    /** This function should be called before attempting to instantiate a
     * ::Socket or any subclass of same. On a Windows box it calls the
     * Winsock initialization routines; on boxes with BSD sockets it does
     * nothing ATM, but this is not guaranteed for future versions. It
     * Fatal()s on failure.
     */
    static void socketsInit(void);
    
  /* Abstract class - all constructors must be protected */
  protected:
    Socket();
    Socket(const Socket&);
  public:
    virtual ~Socket() {}
    virtual void send(const void* buf, size_t len) = 0;
    virtual void send(const Message& message) = 0;
    virtual size_t receive(void* buf, size_t len) = 0;
    virtual size_t receive(
        void* buf,
        size_t len,
        const struct timeval& timeout
      ) = 0;
    virtual void close() = 0;
    virtual void setAsynchronous(bool val) = 0;
    virtual String getAddress() const = 0;
};

}}

#endif // SOCKET_H

