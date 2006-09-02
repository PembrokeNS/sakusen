#ifndef LIBSAKUSEN__EXCEPTIONS_H
#define LIBSAKUSEN__EXCEPTIONS_H

#include "libsakusen-global.h"
#include "stringutils.h"
#include "resourcesearchresult.h"

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
  private:
    DeserializationExn();
  public:
    DeserializationExn(const String& message) :
      Exn(message) { }
    virtual ~DeserializationExn() throw() { }
};

class EndOfArchiveDeserializationExn :
    public DeserializationExn {
  public:
    EndOfArchiveDeserializationExn() :
      DeserializationExn("tried to read past end of archive") { }
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

class NoWorldDeserializationExn :
    public DeserializationExn {
  public:
    NoWorldDeserializationExn() :
      DeserializationExn("world required for deserialization but missing") {}
    virtual ~NoWorldDeserializationExn() throw() {}
};

class WrongMagicDeserializationExn :
    public DeserializationExn {
  public:
    WrongMagicDeserializationExn(
        const char* expected,
        int size,
        const uint8* found
      ) :
      DeserializationExn(
          String("wrong magic: expected '") + String(expected, size) +
          "', found: '" + stringUtils_makePrintable(
            String(reinterpret_cast<const char*>(found), size)
          ) + "'"
        ) {}
    virtual ~WrongMagicDeserializationExn() throw() {}
};

class EnumDeserializationExn :
    public DeserializationExn {
  public:
    EnumDeserializationExn(const String& name, int value) :
      DeserializationExn(
          "enumeration value '"+name+"' out of range (value was "+
          numToString(value) + ")"
        ) {}
    virtual ~EnumDeserializationExn() throw() {}
};

class ResourceDeserializationExn :
    public DeserializationExn {
  public:
    ResourceDeserializationExn(
        const String& resource,
        ResourceSearchResult result,
        String error
      ) :
      DeserializationExn(
          "resource '"+resource+"' "+
          ( result == resourceSearchResult_error ?
            "caused error: " + error : "" ) +
          ( result == resourceSearchResult_notFound ? "not found" : "" ) +
          ( result == resourceSearchResult_ambiguous ? "ambiguous" : "" ) +
          ( result == resourceSearchResult_notSupported ? "not supported" : "" )
        ) {}
    virtual ~ResourceDeserializationExn() throw() {}
};

}

#endif // LIBSAKUSEN__EXCEPTIONS_H

