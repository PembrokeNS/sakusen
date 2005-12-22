#include "socket.h"
#include "libsakusen-global.h"
#include "stringutils.h"
#include <list>

#ifdef WIN32
//Whatever I eventually put here.

#else 

#include "unixdatagramsocket.h"

#endif //Windows

using namespace std;

Socket* Socket::newConnectionToAddress(const String& address)
{
  list<String> addressComponents = stringUtils_split(address, ":");
  if (addressComponents.empty()) {
    return NULL;
  }
  String type = addressComponents.front();
  addressComponents.pop_front();
  if (type == "unix") {
#ifdef WIN32
    return NULL; /* Unix sockets not supported on Windows */
#else
    return UnixDatagramSocket::newConnectionToAddress(addressComponents);
#endif
  } else {
    return NULL;
  }
}

Socket::Socket()
{
}

Socket::Socket(const Socket&)
{
}

