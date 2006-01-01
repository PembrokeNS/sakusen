#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>

#include "message.h"

class Socket {
  public:
    static Socket* newConnectionToAddress(const String& address);
    
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

#endif // SOCKET_H

