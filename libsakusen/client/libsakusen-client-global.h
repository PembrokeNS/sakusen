#ifndef LIBSAKUSEN_CLIENT_GLOBAL_H
#define LIBSAKUSEN_CLIENT_GLOBAL_H

#if defined(_MSC_VER)
  /*4251: This warning is about members of exported classes not being exported.
   * This is mostly to do with stuff from std, but in at least one case it 
   * was very important.  In the long run we should do without disabling 
   * this warning. */
//  #pragma warning(disable:4251)
  /* The __declspec stuff for ensuring symbols are exported from DLLs and
   * imported back into libraries */
     #define LIBSAKUSEN_CLIENT_API
     #define LIBSAKUSEN_CLIENT_EXIMP
    #if 0 
  #ifdef LIBSAKUSEN_CLIENT_EXPORTS
    #define LIBSAKUSEN_CLIENT_API   __declspec(dllexport)
    #define LIBSAKUSEN_CLIENT_EXIMP extern
  #else
    #define LIBSAKUSEN_CLIENT_API   __declspec(dllimport)
    #define LIBSAKUSEN_CLIENT_EXIMP
  #endif
    #endif
#else
  #if defined(__GNUC__) && (__GNUC__ >= 4)
    #define LIBSAKUSEN_CLIENT_API __attribute__ ((visibility ("default")))
  #else
    #define LIBSAKUSEN_CLIENT_API
  #endif
#endif

#endif // LIBSAKUSEN_CLIENT_GLOBAL_H

