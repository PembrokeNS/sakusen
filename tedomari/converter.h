#ifndef CONVERTER_H
#define CONVERTER_H

#include <unicode.h>

#include "libsakusen-global.h"

namespace tedomari {

class Converter {
  private:
    /** Copying not possible, due to unicode_iconv_t members */
    Converter(const Converter&);
  public:
    Converter();
    ~Converter();
  private:
    unicode_iconv_t nativeToUTF8;
    unicode_iconv_t UTF8ToNative;
  public:
    String convert(const String&, unicode_iconv_t conv);
    inline String convertNativeToUTF8(const String& s) {
      return convert(s, nativeToUTF8);
    }
    inline String convertUTF8ToNative(const String& s) {
      return convert(s, UTF8ToNative);
    }
};

}

#endif // CONVERTER_H

