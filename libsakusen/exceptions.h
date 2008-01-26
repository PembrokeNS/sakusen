#ifndef LIBSAKUSEN__EXCEPTIONS_H
#define LIBSAKUSEN__EXCEPTIONS_H

#include "libsakusen-global.h"
#include "stringutils.h"
#include "resourcesearchresult.h"

#include <cerrno>
#include <stdexcept>

namespace sakusen {

class LIBSAKUSEN_API Exn : public std::exception {
  public:
    Exn(const String& m) :
      errorNum(errno), message(m) { }
    virtual ~Exn() throw() { }
    int errorNum;
    const String message;
    const char* what() const throw() { return message.c_str(); }
};

class LIBSAKUSEN_API DeserializationExn : public Exn {
  private:
    DeserializationExn();
  public:
    DeserializationExn(const String& m) : Exn(m) { }
    virtual ~DeserializationExn() throw() { }
};

class LIBSAKUSEN_API WrongVersionDeserializationExn :
    public DeserializationExn {
  public:
    WrongVersionDeserializationExn(uint8 expected, uint8 found) :
      DeserializationExn(
          "found version "+numToString(found)+
          ", expected version "+numToString(expected)) { }
    virtual ~WrongVersionDeserializationExn() throw() { }
};

class LIBSAKUSEN_API EndOfArchiveDeserializationExn :
    public DeserializationExn {
  public:
    EndOfArchiveDeserializationExn() :
      DeserializationExn("tried to read past end of archive") { }
    virtual ~EndOfArchiveDeserializationExn() throw() { }
};

class LIBSAKUSEN_API TooMuchArchiveDeserializationExn :
    public DeserializationExn {
  public:
    TooMuchArchiveDeserializationExn() :
      DeserializationExn("some archive remains unused") { }
    virtual ~TooMuchArchiveDeserializationExn() throw() { }
};

class LIBSAKUSEN_API UnresolvedNameDeserializationExn :
    public DeserializationExn {
  public:
    UnresolvedNameDeserializationExn(String name) :
      DeserializationExn(
          "name '"+name+"' could not be resolved"
        ) { }
    virtual ~UnresolvedNameDeserializationExn() throw() { }
};

class LIBSAKUSEN_API DuplicateNameDeserializationExn :
    public DeserializationExn {
  public:
    DuplicateNameDeserializationExn(String name) :
      DeserializationExn(
          "name '"+name+"' was duplicated"
        ) { }
    virtual ~DuplicateNameDeserializationExn() throw() { }
};

class LIBSAKUSEN_API NoWorldDeserializationExn :
    public DeserializationExn {
  public:
    NoWorldDeserializationExn() :
      DeserializationExn("world required for deserialization but missing") {}
    virtual ~NoWorldDeserializationExn() throw() {}
};

class LIBSAKUSEN_API WrongMagicDeserializationExn :
    public DeserializationExn {
  public:
    WrongMagicDeserializationExn(
        const String& expected,
        const uint8* found
      ) :
      DeserializationExn(
          "wrong magic: expected '" + expected +
          "', found: '" + stringUtils_makePrintable(
            String(reinterpret_cast<const char*>(found), expected.size())
          ) + "'"
        ) {}
    virtual ~WrongMagicDeserializationExn() throw() {}
};

class LIBSAKUSEN_API EnumDeserializationExn :
    public DeserializationExn {
  public:
    /** \brief Constructs a new exception
     * 
     * \param name  Name of variable whose value was out of range
     * \param value Actual value taken by variable
     */
    EnumDeserializationExn(const String& name, int value) :
      DeserializationExn(
          "enumeration value '"+name+"' out of range (value was "+
          numToString(value) + ")"
        ) {}
    virtual ~EnumDeserializationExn() throw() {}
};

class LIBSAKUSEN_API ResourceDeserializationExn :
    public DeserializationExn {
  public:
    ResourceDeserializationExn(
        const String& resource,
        ResourceSearchResult result,
        String error
      ) :
      DeserializationExn(
          "resource '"+resource+"' "+
          ( result == resourceSearchResult_error
            ? "caused error: " + error
            : ""
          ) +
          ( result == resourceSearchResult_notFound
            ? "not found. Error: "+error
            : ""
          ) +
          ( result == resourceSearchResult_ambiguous
            ? "ambiguous"
            : ""
          ) +
          ( result == resourceSearchResult_notSupported
            ? "not supported"
            : ""
          )
        ) {}
    virtual ~ResourceDeserializationExn() throw() {}
};

}

#endif // LIBSAKUSEN__EXCEPTIONS_H

