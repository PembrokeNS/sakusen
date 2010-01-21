#include "converter.h"

#include "errorutils.h"

#define CONVERT_BUFFER_LEN 4096

using namespace sakusen::comms;
using namespace tedomari;

#ifdef DISABLE_CONVERSION
Converter::Converter() {}
Converter::~Converter() {}
#else

#include "langinfo.h"
#include "unicode/ucnv.h"

/** \brief Default constructor
 *
 * Uses nl_langinfo(3) to determine native codeset for conversion */
Converter::Converter() 
{
  //Find native locale.
  nativeLocale = nl_langinfo(CODESET);
  if (nativeLocale == NULL)
    nativeLocale = "ISO-8859-1";//default_locale
  SAKUSEN_DEBUG("native codeset is " << nativeLocale);
}

/** \brief Destructor */
Converter::~Converter(){}

/** \brief Converts a string from the native codeset to UTF-8. 
 *   \todo There are more efficient/powerful ways to do this.  */
String Converter::convertNativeToUTF8(const String& s) {
  UErrorCode errorCode = U_ZERO_ERROR;
  char out[CONVERT_BUFFER_LEN];
  const char* mobileIn = s.c_str();
  size_t outBytesLeft = CONVERT_BUFFER_LEN;
  if (-1 == ucnv_convert("UTF-8", nativeLocale, out, outBytesLeft, mobileIn, 1+s.length(), &errorCode) || U_FAILURE(errorCode))
  {
    SAKUSEN_DEBUG("error converting string: " << u_errorName(errorCode));
    return s;
  }
  return String(out);
}
/** \brief Converts a string from UTF-8 to the native codeset */
String Converter::convertUTF8ToNative(const String& s) {
  UErrorCode errorCode = U_ZERO_ERROR;
  char out[CONVERT_BUFFER_LEN];
  const char* mobileIn = s.c_str();
  size_t outBytesLeft = CONVERT_BUFFER_LEN;
  if (-1 == ucnv_convert(nativeLocale, "UTF-8", out, outBytesLeft, mobileIn, 1+s.length(), &errorCode) || U_FAILURE(errorCode))
  {
    SAKUSEN_DEBUG("error converting string: " << u_errorName(errorCode));
    return s;
  }
  return String(out);
}
#endif // DISABLE_CONVERSION
