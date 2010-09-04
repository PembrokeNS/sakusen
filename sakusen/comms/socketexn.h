#ifndef LIBSAKUSEN_COMMS__SOCKETEXCEPTION_H
#define LIBSAKUSEN_COMMS__SOCKETEXCEPTION_H

#include <sakusen/libsakusen-global.h>
#include <sakusen/exceptions.h>

namespace sakusen {
namespace comms {

class LIBSAKUSEN_COMMS_API SocketExn : public Exn {
  public:
    SocketExn(const String& message) :
      Exn(message) { }
    virtual ~SocketExn() throw() { }
};

class LIBSAKUSEN_COMMS_API SocketClosedExn : public SocketExn {
  public:
    SocketClosedExn() :
      SocketExn("Socket was never connected or was closed remotely") { }
    virtual ~SocketClosedExn() throw() { }
};

class LIBSAKUSEN_COMMS_API HostNotFoundExn : public SocketExn {
  public:
    HostNotFoundExn(const String& hostname) :
      SocketExn("Host '"+hostname+"' not found") { }
    virtual ~HostNotFoundExn() throw() { }
};

}}

#endif // LIBSAKUSEN_COMMS__SOCKETEXCEPTION_H

