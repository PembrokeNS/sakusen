#include <sakusen/stringutils.h>

#include <iomanip>

/* I seem to have now ported mhash. Which worries me a little. */
#ifdef _MSC_VER
#include <mutils/mhash_config.h>
#endif //_MSC_VER

#include <stdbool.h>
#ifndef _Bool
/* mhash relies on this #define, which is non-standard but present in the gcc
 * library.  This preamble to #include <mhash.h> makes it work elsewhere too */
#define _Bool bool
#endif //_Bool
#include <mhash.h>

using namespace std;

namespace sakusen {

String stringUtils_makePrintable(const String& s)
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

String stringUtils_charToUtf8(uint16 c)
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

String::iterator stringUtils_findPreviousCharStart(
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
String LIBSAKUSEN_API stringUtils_bufferToHex(
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
String LIBSAKUSEN_API stringUtils_getSecureHashAsString(
    const uint8* buffer,
    size_t length
  )
{
  /** \todo Maybe support other hashes */
  MHASH thread = mhash_init(MHASH_SHA256);
  /** \bug Commented out because it generates an old style cast warning (silly
   * reason...)
  if (thread == MHASH_FAILED) {
    SAKUSEN_FATAL("error initing hash");
  }
  */
  
  mhash(thread, buffer, length);

  uint8 hash[32];
  
  mhash_deinit(thread, hash);
  
  return sakusen::stringUtils_bufferToHex(hash, 32);
}

}

