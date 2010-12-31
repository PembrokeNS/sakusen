#ifndef LIBSAKUSEN_SERVER_GLOBAL_H
#define LIBSAKUSEN_SERVER_GLOBAL_H

#include <sakusen/global.h>

#if defined(_MSC_VER)
  /*4251: This warning is about members of exported classes not being exported.
   * This is mostly to do with stuff from std, but in at least one case it
   * was very important.  In the long run we should do without disabling
   * this warning.
   *4503: This warning is about the decorated function name being too long,
   * and only occurs in debug builds, due to STLport's type checking .*/
  #pragma warning(disable:4251 4275)
  #ifdef _DEBUG
    #pragma warning(disable:4503)
  #endif

  /* The __declspec stuff for ensuring symbols are exported from DLLs and
   * imported back into libraries */
  #ifdef LIBSAKUSEN_SERVER_EXPORTS
    #define LIBSAKUSEN_SERVER_API   __declspec(dllexport)
    #define LIBSAKUSEN_SERVER_EXIMP extern
  #else
    #define LIBSAKUSEN_SERVER_API   __declspec(dllimport)
    #define LIBSAKUSEN_SERVER_EXIMP
  #endif
#else
  #if defined(__GNUC__) && (__GNUC__ >= 4)
    #define LIBSAKUSEN_SERVER_API __attribute__ ((visibility ("default")))
  #else
    #define LIBSAKUSEN_SERVER_API
  #endif
#endif

#endif // LIBSAKUSEN_SERVER_GLOBAL_H

