#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include "libsakusen-global.h"

#include <list>
#include <sstream>

#include "gnu_extensions.h" // Included only for __gnu_cxx::hash

namespace sakusen {

/* A hash function for Strings */
class StringHash {
  private:
    __gnu_cxx::hash<const char*> hasher;
  public:
    inline size_t operator() (const String& s) const {
      return hasher(s.c_str());
    }
};

/** \brief Split the string splitee at characters contained in spliton */
LIBSAKUSEN_API std::list<String> stringUtils_split(
    const String& splitee,
    const String& spliton
  );

/** \brief Join the list of strings together, joining them with delim */
LIBSAKUSEN_API String stringUtils_join(
    const std::list<String>& strings,
    const String& delim
  );

/** \brief Convert all characters of the argument into printable characters */
String stringUtils_makePrintable(const String&);

/** \brief Converts given unicode character to a UTF-8 string */
String stringUtils_charToUtf8(uint16);

/** \brief Runs the give iterator backwards until it's pointing at the start of
 * a UTF-8 character or it matches bound */
String::iterator stringUtils_findPreviousCharStart(
    String::iterator i,
    const String::iterator& bound
  );

/** \brief Convert a String to an number  */
template<typename T>
inline T numFromString(String str);

template<>
inline uint8 numFromString<uint8>(String str)
{
  std::istringstream s(str);
  sint32 i; /* Has to be a uint32 because uint8 interpreted as a char */
  s >> i;
  assert(uint8(i) == i); /* Check for overflow */
  return i;
}

template<typename T>
inline T numFromString(String str)
{
  std::istringstream s(str);
  T i;
  s >> i;
  return i;
}

/* Convert a number to a String */
template<typename T>
inline String numToString(T i);

template<>
inline String numToString<uint8>(uint8 i)
{
  std::ostringstream s;
  s << static_cast<uint32>(i); /* Has to be cast because otherwise
                                   interpreted as a char */
  return s.str();
}

template<typename T>
inline String numToString(T i)
{
  std::ostringstream s;
  s << i;
  return s.str();
}

String LIBSAKUSEN_API stringUtils_getSecureHashAsString(const uint8* buffer, size_t length);

}

#endif // STRINGUTILS_H

