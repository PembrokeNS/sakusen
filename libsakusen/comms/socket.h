#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>

#include "message.h"

namespace sakusen {
namespace comms {


/** \brief This is a generic class for representing any type of socket.
 * 
 * Socket::newConnectionToAddress() and Socket::newBindingToAddress() can be
 * used to create Sockets */
class Socket {
  public:
    /* \brief Creates a new Socket object connected to the given address.
     * \param address Sakusen-format address to connect to.
     * \return Pointer to a new Socket object, which must be deleted by the
     * caller. */
    static Socket* newConnectionToAddress(const String& address);
    /* \brief Creates a new Socket object bound to the given address.
     * \param address Sakusen-format address to bind socket at.
     * \return Pointer to a new Socket object, which must be deleted by the
     * caller. */
    static Socket* newBindingToAddress(const String& address);
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
    /** \brief Reads \p len bytes of binary from \p buf and sends it.
     * \param buf Must not be NULL.
     */
    virtual void send(const void* buf, size_t len) = 0;
    /** \brief sends the given ::Message down the line. */
    inline void send(const Message& message)
    {
      send(message.getBytes(), message.getBytesLength());
    }
    /** \brief Reads \p len bytes of binary from \p buf and sends it.
     * \param buf Must not be NULL.
     * \param address Sakusen-style address to send to.
     */
    virtual void sendTo(const void* buf, size_t len, const String& address) = 0;
    /** \brief sends the given ::Message to the given address. */
    inline void sendTo(const Message& message, const String& address)
    {
      sendTo(message.getBytes(), message.getBytesLength(), address);
    }
    /** \brief receives a binary stream from the network
     * \param[out] buf Must not be NULL, and should point to a buffer into
     * which \p len bytes can be written.
     * \param len The maximum number of bytes you are willing to see.
     * \return The number of bytes actually read.
     */
    virtual size_t receive(void* buf, size_t len) = 0;
    /** \brief receives a binary stream from the network, with a timeout
     * \param[out] buf Must not be NULL, and should point to a buffer into
     * which \p len bytes can be written.
     * \param len The maximum number of bytes you are willing to see.
     * \param timeout The length of time you are willing to wait for the
     * buffer to fill.
     * \return The number of bytes actually read.
     */
    virtual size_t receiveTimeout(
        void* buf,
        size_t len,
        const struct timeval& timeout
      ) = 0;
    /** \brief receives a binary stream from the network
     * \param[out] buf Must not be NULL, and should point to a buffer into
     * which \p len bytes can be written.
     * \param len The maximum number of bytes you are willing to see.
     * \param[out] from The sakusen-style address from which the message was
     * sent, or a null string of the underlying socket does not support
     * reporting the source.
     * \return The number of bytes actually read.
     */
    virtual size_t receiveFrom(void* buf, size_t len, String& from) = 0;
    /** \brief Returns true iff this socket is connections-based (e.g. TCP but
     * not UDP) */
    virtual bool isConnectionBased() = 0;
    /** \brief Accepts a pending connection on this socket
     *
     * For a connection-based socket, this will return a new Socket
     * corresponding to the next pending connection, or NULL if there is no
     * such.  For a non-connection-based socket, this will Fatal() */
    virtual Socket* accept() = 0;
    /** \brief Close a socket, unless it is already closed. */
    virtual void close() = 0;
    /** \brief Set the async flag on this socket. */
    virtual void setAsynchronous(bool val) = 0;
    /** \brief Get the Sakusen-style address of this socket. */
    virtual String getAddress() const = 0;
};

}}

#endif // SOCKET_H

