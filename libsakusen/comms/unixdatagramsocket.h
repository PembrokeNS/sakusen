#ifndef UNIXDATAGRAMSOCKET_H
#define UNIXDATAGRAMSOCKET_H

#include "libsakusen-comms-global.h"
#include "socket.h"

#include <sys/un.h>

#include <list>

namespace sakusen {
namespace comms {

class UnixDatagramSocket : public Socket {
  private:
    static void interpretAddress(
        std::list<String>& address,
        String* path,
        bool* abstract
      );
  public:
    static Socket* newConnectionToAddress(std::list<String>& address);
    static Socket* newBindingToAddress(std::list<String>& address);
  
  /* Abstract class - all constructors must be protected */
  private:
    UnixDatagramSocket();
    /** Copying this would be a bad idea, since the socket would be closed
     * twice, and other such problems.
     */
    UnixDatagramSocket(const UnixDatagramSocket& copy);
  protected:
    UnixDatagramSocket(const char* path, bool abstract);
    UnixDatagramSocket(const String& path, bool abstract);
  public:
    virtual ~UnixDatagramSocket() { close(); }
  protected:
    /** whether this socket exists in the abstract namespace (see unix(7)) */
    bool abstract;
    /** whether this socket has been closed already */
    bool closed;
    char path[UNIX_PATH_MAX];
    struct sockaddr_un addr; /**< socket address */
    int sockfd; /**< socket file descriptor */
      /* This class's constructor sets up everything except actually binding or
       * connecting the socket itself - that is the responsibility of the child
       * classes */
  private:
    void initialize(const char* path);
  public:
    void send(const void* buf, size_t len);
    void sendTo(const void* buf, size_t len, const String& address);
    size_t receive(void* buf, size_t len);
    size_t receiveTimeout(void* buf, size_t len, const struct timeval& timeout);
    size_t receiveFrom(void* buf, size_t len, String& from);
    bool isConnectionBased() { return false; }
    Socket* accept() { Fatal("Not connection-based socket"); }
    void close();
    void setAsynchronous(bool val);
    String getAddress() const;
};

}}

#endif // UNIXDATAGRAMSOCKET_H

