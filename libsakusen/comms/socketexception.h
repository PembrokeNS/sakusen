#ifndef LIBSAKUSEN_COMMS__SOCKETEXCEPTION_H
#define LIBSAKUSEN_COMMS__SOCKETEXCEPTION_H

#include "libsakusen-global.h"
#include "exceptions.h"

namespace sakusen {
namespace comms {

class SocketExn : public Exn {
  public:
    SocketExn(const String& message) :
      Exn(message) { }
    virtual ~SocketExn() throw() { }
};

class SocketClosedExn : public SocketExn {
  public:
    SocketClosedExn() :
      SocketExn("Socket was never connected or was closed remotely") { }
    virtual ~SocketClosedExn() throw() { }
};

}}

#endif // LIBSAKUSEN_COMMS__SOCKETEXCEPTION_H

