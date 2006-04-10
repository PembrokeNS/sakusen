#ifndef CONVERTER_H
#define CONVERTER_H

#include "libsakusen-global.h"

#ifndef DISABLE_CONVERSION
#include <unicode.h>
#endif

namespace tedomari {

/** \brief Handles conversion of text between the native encoding (used for
 * terminal I/O) and UTF-8 (used for all sakusen strings internally)
 *
 * When DISABLE_CONVERSION is defined, this class performs no alterations to
 * the strings it is given to convert, but simply passes them on verbatim.
 * 
 * \warning This may yield invalid UTF-8 strings
 *
 * Otherwise, the unicode library is used to convert strings between the native
 * encoding (as determined by locale settings) and UTF-8. */
class Converter {
  private:
    /** Copying not possible, due to unicode_iconv_t members */
    Converter(const Converter&);
  public:
    Converter();
    ~Converter();
#ifdef DISABLE_CONVERSION
    /** when DISABLE_CONVERSION is set we just forward
     * the strings verbatim */
    inline String convertNativeToUTF8(const String& s) {
      return s;
    }
    inline String convertUTF8ToNative(const String& s) {
      return s;
    }
#else
  private:
    unicode_iconv_t nativeToUTF8;
    unicode_iconv_t UTF8ToNative;
  public:
    String convert(const String&, unicode_iconv_t conv);
    /** \brief Converts a string from the native codeset to UTF-8 */
    inline String convertNativeToUTF8(const String& s) {
      return convert(s, nativeToUTF8);
    }
    /** \brief Converts a string from UTF-8 to the native codeset */
    inline String convertUTF8ToNative(const String& s) {
      return convert(s, UTF8ToNative);
    }
#endif
};

}

#endif // CONVERTER_H

