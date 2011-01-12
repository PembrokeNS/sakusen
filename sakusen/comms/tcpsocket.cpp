#include <sakusen/comms/tcpsocket.h>
#include <sakusen/comms/tcpconnectingsocket.h>
#include <sakusen/comms/tcplisteningsocket.h>
#include <sakusen/stringutils.h>
#include <sakusen/comms/errorutils.h>
#include <sakusen/comms/socketexn.h>

#include <boost/scoped_array.hpp>

#ifdef WIN32
#include <sakusen/comms/wsabsd.h>
#define NativeReceiveReturnType int
typedef int ssize_t;
#else // BSD sockets
#define NativeReceiveReturnType ssize_t
#endif

namespace sakusen {
namespace comms {

namespace {

typedef uint32 BufferLenType;
inline BufferLenType hton_buffer(BufferLenType i) { return htonl(i); }
inline BufferLenType ntoh_buffer(BufferLenType i) { return ntohl(i); }

}

Socket::Ptr TCPSocket::newConnectionToAddress(std::list<String>& address)
{
  String hostname;
  uint16 port;

  interpretAddress(address, &hostname, &port);

  if (hostname.empty()) {
    return Socket::Ptr();
  }

  return Socket::Ptr(new TCPConnectingSocket(hostname, port));
}

Socket::Ptr TCPSocket::newBindingToAddress(std::list<String>& address)
{
  String hostname;
  uint16 port;

  interpretAddress(address, &hostname, &port);

  if (hostname.empty()) {
    return Socket::Ptr();
  }

  /* hostname is just ignored except for checking the error condition */
  return Socket::Ptr(new TCPListeningSocket(port));
}

TCPSocket::TCPSocket() :
  buffer(NULL),
  bufferLength(0),
  bufferCapacity(0)
{
  sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
}

TCPSocket::TCPSocket(uint16 myPort):
  IPSocket(myPort),
  buffer(NULL),
  bufferLength(0),
  bufferCapacity(0)
{
  sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
}

TCPSocket::TCPSocket(NativeSocket s, const sockaddr_in& peerAddress) :
  IPSocket(s, peerAddress),
  buffer(NULL),
  bufferLength(0),
  bufferCapacity(0)
{
}

void TCPSocket::send(const void* buf, size_t len)
{
  /* Prefix the message by its length */
  boost::scoped_array<uint8> longerBuf(new uint8[len+sizeof(BufferLenType)]);
  if (len > std::numeric_limits<BufferLenType>::max()) {
    SAKUSEN_FATAL("message too long for protocol ("<<len<<" bytes)");
  }
  *reinterpret_cast<BufferLenType*>(longerBuf.get()) = hton_buffer(len);
  memcpy(longerBuf.get()+sizeof(BufferLenType), buf, len);

  char const* toSend = reinterpret_cast<char*>(longerBuf.get());
  char const* bufferEnd = toSend+len+sizeof(BufferLenType);

  while (toSend != bufferEnd) {
  #ifndef _MSC_VER
      ssize_t retVal = ::send(sockfd, toSend, bufferEnd - toSend, MSG_NOSIGNAL);
  #else
	  /*Windows does not, I think, support SIGPIPE. 
	  Hence a flag to tell the socket not to send the signal is also a bit redundant.*/
	  ssize_t retVal = ::send(sockfd, toSend, bufferEnd - toSend, 0);  
  #endif
    if (retVal == -1) {
      switch (socketErrno()) {
        case ENOTCONN:
        case ECONNREFUSED:
        case ECONNABORTED:
        case ECONNRESET:
          throw SocketClosedExn();
          break;
        case EAGAIN:
          /** \bug Spinlock on EAGAIN; not ideal.  Long term solution: use
           * asyncronous I/O.  In the short term, this could be improved with
           * select. */
          SAKUSEN_DEBUG(
              "EAGAIN; trying again; "<<(bufferEnd-toSend)<<" bytes remain"
            );
          continue;
        case EPIPE:
          throw SocketExn("Socket has been closed locally");
          break;
        default:
          SAKUSEN_FATAL(
              "error " << errorUtils_parseErrno(socketErrno()) <<
              " sending message"
            );
          break;
      }
    }
    assert(retVal > 0);
    toSend += retVal;
  }
}

void TCPSocket::sendTo(const void* /*buf*/, size_t /*len*/, const String& /*address*/)
{
  SAKUSEN_FATAL("not implemented");
}

size_t TCPSocket::receive(void* outBuf, size_t len)
{
  /* We fetch as much as possible and buffer it locally */
  do {
    if (bufferCapacity > bufferLength) {
      NativeReceiveReturnType received = recv(
          sockfd, reinterpret_cast<char*>(buffer+bufferLength),
          bufferCapacity - bufferLength, 0
        );
      if (received == -1) {
        if (socketErrno() == EAGAIN || socketErrno() == EWOULDBLOCK) {
          break;
        }
        if (socketErrno() == ECONNABORTED) {
          throw SocketClosedExn();
        }
        if (socketErrno() == ECONNRESET) {
           throw SocketClosedExn();
        }
        SAKUSEN_FATAL(
            "error receiving message: " << errorUtils_parseErrno(socketErrno())
          );
      }
      if (received != 0) {
        /*SAKUSEN_DEBUG("received = " << received);*/
      }
      bufferLength += received;
      if (bufferCapacity > bufferLength) {
        break;
      }
    }
    size_t newBufferCapacity = 2*bufferCapacity;
    if (newBufferCapacity == bufferCapacity) {
      ++newBufferCapacity;
    }
    uint8* newBuffer = new uint8[newBufferCapacity];
    memcpy(newBuffer, buffer, bufferLength);
    delete[] buffer;
    buffer = newBuffer;
    bufferCapacity = newBufferCapacity;
  } while (true);

  /* Now we check whether we've got a whole message */
  if (bufferLength < sizeof(BufferLenType)) {
    return 0;
  }
  BufferLenType nextMessageLen =
    ntoh_buffer(*reinterpret_cast<BufferLenType*>(buffer));
  assert(nextMessageLen > 0);
  if (bufferLength >= nextMessageLen + sizeof(BufferLenType)) {
    if (len < nextMessageLen) {
      SAKUSEN_FATAL("insufficient space in buffer for message (needed " <<
          nextMessageLen<<", given "<<len<<")");
    }
    memcpy(outBuf, buffer+sizeof(BufferLenType), nextMessageLen);
    memmove(
        buffer, buffer+nextMessageLen+sizeof(BufferLenType),
        bufferLength-nextMessageLen-sizeof(BufferLenType)
      );
    bufferLength -= (nextMessageLen+sizeof(BufferLenType));
    return nextMessageLen;
  }
  return 0;
}

size_t TCPSocket::receiveFrom(void* /*buf*/, size_t /*len*/, String& /*from*/)
{
  SAKUSEN_FATAL("not implemented");
  return 0; /* Return statement for the benefit of MSVC */
}

}}

