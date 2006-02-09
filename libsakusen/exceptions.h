#ifndef LIBSAKUSEN__EXCEPTIONS_H
#define LIBSAKUSEN__EXCEPTIONS_H

#include "libsakusen-global.h"

#include <cerrno>
#include <stdexcept>

namespace sakusen {

class Exn {
  public:
    Exn(const String& m) :
      errorNum(errno), message(m) { }
    virtual ~Exn() throw() { }
    int errorNum;
    const String message;
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

}

#endif // LIBSAKUSEN__EXCEPTIONS_H

