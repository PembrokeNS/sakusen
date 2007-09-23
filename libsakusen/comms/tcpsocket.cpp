#include "tcpsocket.h"
#include "tcpconnectingsocket.h"
#include "tcplisteningsocket.h"
#include "stringutils.h"
#include "errorutils.h"
#include "socketexn.h"

#ifdef WIN32
#include "wsabsd.h"
#define NativeReceiveReturnType int
#else // BSD sockets
#define NativeReceiveReturnType ssize_t
#endif

using namespace sakusen;
using namespace sakusen::comms;

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
  uint8* longerBuf = new uint8[len+2];
  *reinterpret_cast<uint16*>(longerBuf) = htons(len);
  memcpy(longerBuf+2, buf, len);
  
  int retVal = ::send(sockfd, reinterpret_cast<char*>(longerBuf), len+2, 0);
  if (retVal == -1) {
    switch (socket_errno) {
      case ENOTCONN:
      case ECONNREFUSED:
      case ECONNABORTED:
        throw SocketClosedExn();
        break;
      default:
        Fatal("error " << errorUtils_parseErrno(socket_errno) << " sending message");
        break;
    }
  }

  delete[] longerBuf;
}

void TCPSocket::sendTo(const void* /*buf*/, size_t /*len*/, const String& /*address*/)
{
  Fatal("not implemented");
#if 0
  std::list<String> splitAddress = stringUtils_split(address, ADDR_DELIM);
  assert(!splitAddress.empty());
  assert(splitAddress.front() == getType());
  splitAddress.pop_front();
  
  String hostname;
  uint16 port;

  interpretAddress(splitAddress, &hostname, &port);
  
  struct hostent *endpoint;
  endpoint = gethostbyname(hostname.c_str());
  
  if (endpoint == NULL) {
    Fatal("host " << hostname << " not found.");
  }

  sockaddr_in dest;

  memset(&dest, 0, sizeof(addr));
  dest.sin_family = AF_INET;
  memcpy(&dest.sin_addr.s_addr, endpoint->h_addr, endpoint->h_length);
  dest.sin_port = htons(port);
  
  /* Prefix the message by its length */
  uint8 longerBuf[len+2];
  *reinterpret_cast<uint16*>(longerBuf) = htons(len);
  memcpy(longerBuf+2, buf, len);
  
  int retVal = ::sendto(
      sockfd, longerBuf, len+2, 0, reinterpret_cast<sockaddr*>(&dest),
      sizeof(dest)
    );
  if (retVal == -1) {
    switch (socket_errno) {
      case ENOTCONN:
      case ECONNREFUSED:
        throw SocketClosedExn();
        break;
      default:
        Fatal("error " << errorUtils_parseErrno(socket_errno) << " sending message");
        break;
    }
  }
#endif
}

size_t TCPSocket::receive(void* outBuf, size_t len)
{
  /* We fetch as much as possible and buffer it locally */
  do {
    if (bufferCapacity > bufferLength) {
      NativeReceiveReturnType received = recv(
          sockfd, reinterpret_cast<char*>(buffer+bufferLength), bufferCapacity - bufferLength, 0
        );
      if (received == -1) {
        if (socket_errno == EAGAIN || socket_errno == EWOULDBLOCK) {
          break;
        }
        if (socket_errno == ECONNABORTED) {
          throw SocketClosedExn();
        }
        if (socket_errno == ECONNRESET) {
           throw SocketClosedExn();
        }
        Fatal("error receiving message: " << errorUtils_parseErrno(socket_errno));
      }
      if (received != 0) {
        /*Debug("received = " << received);*/
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
  if (bufferLength < 2) {
    return 0;
  }
  uint16 nextMessageLen = ntohs(*reinterpret_cast<uint16*>(buffer));
  assert(nextMessageLen > 0);
  /*Debug("nextMessageLen=" << nextMessageLen);*/
  if (bufferLength >= nextMessageLen + 2U) {
    if (len < nextMessageLen) {
      Fatal("insufficient space in buffer for message");
    }
    memcpy(outBuf, buffer+2, nextMessageLen);
    memmove(buffer, buffer+nextMessageLen+2, bufferLength-nextMessageLen-2);
    bufferLength -= (nextMessageLen+2);
    return nextMessageLen;
  }
  return 0;
}

size_t TCPSocket::receiveFrom(void* /*buf*/, size_t /*len*/, String& /*from*/)
{
  Fatal("not implemented");
  return 0; /* Return statement for the benefit of MSVC */
}

