#include "stringutils.h"

#include <iomanip>

/* I seem to have now ported mhash. Which worries me a little. */
#ifdef _MSC_VER
#include <mutils/mhash_config.h>
#endif //_MSC_VER

#include <mhash.h>

using namespace std;

String sakusen::stringUtils_join(
    const list<String>& strings,
    const String& delim
  )
{
  if (strings.empty()) {
    return "";
  }
  list<String>::const_iterator s = strings.begin();
  String result = *s;
  ++s;
  
  for (; s != strings.end(); ++s) {
    result += delim;
    result += *s;
  }
  
  return result;
}

String sakusen::stringUtils_makePrintable(const String& s)
{
  /* Note that this is UTF-8 safe */
  String result(s);
  for (String::iterator c = result.begin(); c != result.end(); ++c) {
    if (*c < ' ') {
      *c = '.';
    }
  }
  return result;
}

String sakusen::stringUtils_charToUtf8(uint16 c)
{
  if (c<0x0080) {
    return String(1, static_cast<char>(c));
  } else if (c<0x0800) {
    String s(2, '\0');
    s[0] = (c >> 6) | 0xc0;
    s[1] = (c & 0x3f) | 0x80;
    return s;
  } else {
    String s(3, '\0');
    s[0] = (c >> 12) | 0xe0;
    s[1] = ((c >> 6) & 0x3f) | 0x80;
    s[2] = (c & 0x3f) | 0x80;
    return s;
  }
}

String::iterator sakusen::stringUtils_findPreviousCharStart(
    String::iterator i,
    const String::iterator& bound
  )
{
  do {
    if (i == bound) {
      break;
    }
    --i;
  } while ((*i & 0xc0) == 0x80);
  return i;
}

/** \brief Returns hex string version of buffer */
String LIBSAKUSEN_API sakusen::stringUtils_bufferToHex(
    const uint8* buffer, size_t length
  )
{
  ostringstream stream;
  stream << hex << setfill('0');
  for (size_t i=0; i<length; i++) {
    stream << setw(2) << uint16(buffer[i]);
  }
  return stream.str();
}

/** \brief Computes a secure hash of the buffer and returns it as a hex string.
 *
 * The hash used is SHA-256. */
String LIBSAKUSEN_API sakusen::stringUtils_getSecureHashAsString(
    const uint8* buffer,
    size_t length
  )
{
  /** \todo Maybe support other hashes */
  MHASH thread = mhash_init(MHASH_SHA256);
  /** \bug Commented out because it generates an old style cast warning (silly
   * reason...)
  if (thread == MHASH_FAILED) {
    Fatal("error initing hash");
  }
  */
  
  mhash(thread, buffer, length);

  uint8 hash[32];
  
  mhash_deinit(thread, hash);
  
  return sakusen::stringUtils_bufferToHex(hash, 32);
}

