#include "socket.h"
#include "libsakusen-global.h"
#include "stringutils.h"
#include <list>

#ifdef WIN32
#include <winsock2.h>
#endif //WIN32

#include "unixdatagramsocket.h"
#include "udpsocket.h"
#include "tcpsocket.h"

using namespace std;

using namespace sakusen::comms;

void Socket::socketsInit(void)
{
#ifdef WIN32
  WSADATA wsaData;
  int iResult;
  iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
  if ( iResult != NO_ERROR )
    Fatal("Error at WSAStartup()");
#endif
}
#if defined(_MSC_VER) && (_MSC_VER<1450) 
//This is a rather silly way of avoiding a warning on VC.
//Essentially, specifying the exception type is ignored by the compiler, and this generates warnings. 
//These warnings are not critical in any way, but I feel aspergic.
  Socket::Ptr Socket::newConnectionToAddress(const sakusen::String& address) throw (...)
#else
  Socket::Ptr Socket::newConnectionToAddress(const sakusen::String& address) throw (SocketExn)
#endif
{
  list<String> addressComponents =
    stringUtils_split<list<String> >(address, ADDR_DELIM);
  if (addressComponents.empty()) {
    return Ptr();
  }
  String type = addressComponents.front();
  addressComponents.pop_front();
  if (type == "unix") {
#ifdef DISABLE_UNIX_SOCKETS
    return Ptr(); /* Unix sockets not supported */
#else
    return UnixDatagramSocket::newConnectionToAddress(addressComponents);
#endif
  } else if (type == "udp") {
    return UDPSocket::newConnectionToAddress(addressComponents);
  } else if (type == "tcp") {
    return TCPSocket::newConnectionToAddress(addressComponents);
  } else {
    Debug("Unexpected type: " << type);
    return Ptr();
  }
}

Socket::Ptr Socket::newBindingToAddress(const sakusen::String& address)
{
  list<String> addressComponents =
    stringUtils_split<list<String> >(address, ADDR_DELIM);
  if (addressComponents.empty()) {
    return Ptr();
  }
  String type = addressComponents.front();
  addressComponents.pop_front();
  if (type == "unix") {
#ifdef DISABLE_UNIX_SOCKETS
    return Ptr(); /* Unix sockets not supported on Windows */
#else
    return UnixDatagramSocket::newBindingToAddress(addressComponents);
#endif
  } else if (type == "udp") {
    return UDPSocket::newBindingToAddress(addressComponents);
  } else if (type == "tcp") {
    return TCPSocket::newBindingToAddress(addressComponents);
  } else {
    Debug("Unexpected type: " << type);
    return Ptr();
  }
}

Socket::Socket()
{
}

