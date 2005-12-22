#ifndef LIBSAKUSEN__EXCEPTIONS_H
#define LIBSAKUSEN__EXCEPTIONS_H

#include "libsakusen-global.h"

#include <errno.h>

#include <stdexcept>

class Exception : public std::runtime_error {
  public:
    Exception(const String& message) :
      std::runtime_error(message), errorNum(errno) { }
    virtual ~Exception() throw() { }
    int errorNum;
};

class DeserializationException : public Exception {
  public:
    DeserializationException(const String& message) :
      Exception(message) { }
    virtual ~DeserializationException() throw() { }
};

class UnexpectedEndOfArchiveDeserializationException :
    public DeserializationException {
  public:
    UnexpectedEndOfArchiveDeserializationException() :
      DeserializationException("tried to read past end of arhive") { }
    virtual ~UnexpectedEndOfArchiveDeserializationException() throw() { }
};

class UnresolvedNameDeserializationException :
    public DeserializationException {
  public:
    UnresolvedNameDeserializationException(String name) :
      DeserializationException(
          "name '"+name+"' could not be resolved"
        ) { }
    virtual ~UnresolvedNameDeserializationException() throw() { }
};

class DuplicateNameDeserializationException :
    public DeserializationException {
  public:
    DuplicateNameDeserializationException(String name) :
      DeserializationException(
          "name '"+name+"' was duplicated"
        ) { }
    virtual ~DuplicateNameDeserializationException() throw() { }
};

#endif // LIBSAKUSEN__EXCEPTIONS_H

