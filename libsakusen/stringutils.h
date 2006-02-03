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

/* Split the string splitee at characters contained in spliton */
std::list<String> stringUtils_split(
    const String& splitee,
    const String& spliton
  );

/* Convert a String to an number  */
template<typename T>
inline T numFromString(String str);

/*Because this file is linked to from several places, VC gives this warning quite a lot. So it is disabled. 
This warning is about losing information when the sint32 in the following funciton is typecast as uint8, 
so not important. */
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4244)
#endif

template<>
inline uint8 numFromString<uint8>(String str)
{
  std::istringstream s(str);
  sint32 i; /* Has to be a uint32 because uint8 interpreted as a char */
  s >> i;
  assert(uint8(i) == i); /* Check for overflow */
  return i;
}
#if defined(_MSC_VER)
#pragma warning (pop)
#endif
//Restores old state, in case this warning is useful anywhere else

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

String stringUtils_getSecureHashAsString(const uint8* buffer, size_t length);

}

#endif // STRINGUTILS_H

