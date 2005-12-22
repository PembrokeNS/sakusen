#ifndef LIBSAKUSEN_COMMS__SOCKETEXCEPTION_H
#define LIBSAKUSEN_COMMS__SOCKETEXCEPTION_H

#include "libsakusen-global.h"
#include "exceptions.h"

class SocketException : public Exception {
  public:
    SocketException(const String& message) :
      Exception(message) { }
    virtual ~SocketException() throw() { }
};

class SocketClosedException : public SocketException {
  public:
    SocketClosedException() :
      SocketException("Socket was never connected or was closed remotely") { }
    virtual ~SocketClosedException() throw() { }
};

#endif // LIBSAKUSEN_COMMS__SOCKETEXCEPTION_H

