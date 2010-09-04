#ifndef LIBSAKUSEN__STRINGUTILS_H
#define LIBSAKUSEN__STRINGUTILS_H

#include "libsakusen-global.h"

#include <list>
#include <sstream>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "gnu_extensions.h"

/** \file
 * \brief String utilities
 *
 * This header defines various utilities (primarily functions) for
 * manipulating strings.
 */

namespace sakusen {

/** \brief Split the string splitee at characters contained in spliton */
template<typename Container>
Container stringUtils_split(
    const String& splitee,
    const String& spliton
  ) {
  Container c;
  return boost::algorithm::split(
      c, splitee, boost::algorithm::is_any_of(spliton),
      boost::algorithm::token_compress_on
    );
}

/** \brief Join the list of strings together, joining them with delim */
template<typename Container>
 String stringUtils_join(
    const Container& strings,
    const String& delim
  )
{
  if (strings.empty()) {
    return "";
  }
  typename Container::const_iterator s = strings.begin();
  String result = *s;
  ++s;
  
  for (; s != strings.end(); ++s) {
    result += delim;
    result += *s;
  }
  
  return result;
}

/** \brief Convert all characters of the argument into printable characters */
String LIBSAKUSEN_API stringUtils_makePrintable(const String&);

/** \brief Converts given unicode character to a UTF-8 string */
String LIBSAKUSEN_API stringUtils_charToUtf8(uint16);

/** \brief Runs the give iterator backwards until it's pointing at the start of
 * a UTF-8 character or it matches bound */
String::iterator LIBSAKUSEN_API stringUtils_findPreviousCharStart(
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
  sint32 i; /* Has to be an int32 because uint8 interpreted as a char */
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

/** \brief Converts a number to a String */
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

String LIBSAKUSEN_API stringUtils_bufferToHex(const uint8* buffer, size_t length);

String LIBSAKUSEN_API stringUtils_getSecureHashAsString(const uint8* buffer, size_t length);

}

#endif // LIBSAKUSEN__STRINGUTILS_H

