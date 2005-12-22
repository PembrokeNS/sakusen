#include "converter.h"

#include "errorutils.h"

#define CONVERT_BUFFER_LEN 4096

using namespace tedomari;

Converter::Converter() :
  nativeToUTF8(unicode_iconv_open("UTF-8", nl_langinfo(CODESET))),
  UTF8ToNative(unicode_iconv_open(nl_langinfo(CODESET), "UTF-8"))
{
  Debug("native codeset is " << nl_langinfo(CODESET));
  /* TODO: error checking */
}

Converter::~Converter()
{
  unicode_iconv_close(nativeToUTF8);
  unicode_iconv_close(UTF8ToNative);
}

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

