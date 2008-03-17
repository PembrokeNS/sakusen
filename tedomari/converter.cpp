#include "converter.h"

#include "errorutils.h"

#define CONVERT_BUFFER_LEN 4096

using namespace sakusen::comms;
using namespace tedomari;

#ifdef DISABLE_CONVERSION
Converter::Converter() {}
Converter::~Converter() {}
#else

/** \brief Default constructor
 *
 * Uses nl_langinfo(3) to determine native codeset for conversion */
Converter::Converter() 
{
  unicode_init();
  nativeToUTF8=unicode_iconv_open("UTF-8", nl_langinfo(CODESET));
  UTF8ToNative=unicode_iconv_open(nl_langinfo(CODESET), "UTF-8");
  Debug("native codeset is " << nl_langinfo(CODESET));
  if(nativeToUTF8 == (unicode_iconv_t) -1
      || UTF8ToNative==(unicode_iconv_t) -1)
  {
     Debug("Failed to find converter between "<<nl_langinfo(CODESET)<<" and UTF8");
     Debug("Falling back to ASCII");
     nativeToUTF8=unicode_iconv_open("UTF-8", "US-ASCII");
     UTF8ToNative=unicode_iconv_open("US-ASCII", "UTF-8");
     if(nativeToUTF8 == (unicode_iconv_t) -1
      || UTF8ToNative==(unicode_iconv_t) -1)
       Fatal("Failed to find converter between US-ASCII and UTF-8. Giving up.");
  }
  
}

/** \brief Destructor */
Converter::~Converter()
{
  unicode_iconv_close(nativeToUTF8);
  unicode_iconv_close(UTF8ToNative);
}

/** \brief Convert string using given converter
 *
 * \param s String to convert
 * \param conv Converter to use
 *
 * It is not expected that this method be used externally.  The other methods
 * are generally more useful */
String Converter::convert(const String& s, unicode_iconv_t conv)
{
  char out[CONVERT_BUFFER_LEN];
  const char* mobileIn = s.c_str();
  size_t inBytesLeft = 1+s.length();
  char* mobileOut = out;
  size_t outBytesLeft = CONVERT_BUFFER_LEN;
  
  if (-1 ==
      unicode_iconv(conv, &mobileIn, &inBytesLeft, &mobileOut, &outBytesLeft)) {
    Fatal("error converting string: " << errorUtils_errorMessage(errno) <<
        " (" << errorUtils_parseErrno(errno) << ")");
  }
  return String(out);
}
#endif // DISABLE_CONVERSION
