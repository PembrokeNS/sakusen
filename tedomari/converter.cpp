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
  //Find native locale.
  native_locale = nl_langinfo(CODESET);
  if(native_locale ==NULL)
		native_locale = "ISO-8859-1";//default_locale
  Debug("native codeset is " << String(native_locale));
}

/** \brief Destructor */
Converter::~Converter(){}

/** \brief Converts a string from the native codeset to UTF-8. 
 *   TODO: There are more efficient/powerful ways to do this.  */
String Converter::convertNativeToUTF8(const String& s) {
    char out[CONVERT_BUFFER_LEN];
    const char* mobileIn = s.c_str();
    size_t outBytesLeft = CONVERT_BUFFER_LEN;
    if( -1 == ucnv_convert ("UTF-8",native_locale, out,outBytesLeft, mobileIn, 1+s.length(), &ErrorCode) || U_FAILURE(ErrorCode))
	 {
       Debug("error converting string");
       return s;
    }
  return String(out);
}
/** \brief Converts a string from UTF-8 to the native codeset */
String Converter::convertUTF8ToNative(const String& s) {
    char out[CONVERT_BUFFER_LEN];
    const char* mobileIn = s.c_str();
    size_t outBytesLeft = CONVERT_BUFFER_LEN;
    if( -1 == ucnv_convert (native_locale, "UTF-8", out,	outBytesLeft, mobileIn, 1+s.length(), &ErrorCode) || U_FAILURE(ErrorCode))
	 {
        Debug("error converting string");
		  return s;
	 }
  return String(out);
}
#endif // DISABLE_CONVERSION
