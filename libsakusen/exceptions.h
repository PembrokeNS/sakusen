#ifndef LIBSAKUSEN__EXCEPTIONS_H
#define LIBSAKUSEN__EXCEPTIONS_H

#include "libsakusen-global.h"

#include <errno.h>

#include <stdexcept>

class Exn : public std::runtime_error {
  public:
    Exn(const String& message) :
      std::runtime_error(message), errorNum(errno) { }
    virtual ~Exn() throw() { }
    int errorNum;
};

class DeserializationExn : public Exn {
  public:
    DeserializationExn(const String& message) :
      Exn(message) { }
    virtual ~DeserializationExn() throw() { }
};

class EndOfArchiveDeserializationExn :
    public DeserializationExn {
  public:
    EndOfArchiveDeserializationExn() :
      DeserializationExn("tried to read past end of arhive") { }
    virtual ~EndOfArchiveDeserializationExn() throw() { }
};

class UnresolvedNameDeserializationExn :
    public DeserializationExn {
  public:
    UnresolvedNameDeserializationExn(String name) :
      DeserializationExn(
          "name '"+name+"' could not be resolved"
        ) { }
    virtual ~UnresolvedNameDeserializationExn() throw() { }
};

class DuplicateNameDeserializationExn :
    public DeserializationExn {
  public:
    DuplicateNameDeserializationExn(String name) :
      DeserializationExn(
          "name '"+name+"' was duplicated"
        ) { }
    virtual ~DuplicateNameDeserializationExn() throw() { }
};

#endif // LIBSAKUSEN__EXCEPTIONS_H

