#ifndef LIBSAKUSEN_CLIENT_GLOBAL_H
#define LIBSAKUSEN_CLIENT_GLOBAL_H

#if defined(_MSC_VER)
  /* The __declspec stuff for ensuring symbols are exported from DLLs and
   * imported back into libraries */
  #ifdef LIBSAKUSEN_CLIENT_EXPORTS
    #define LIBSAKUSEN_CLIENT_API   __declspec(dllexport)
    #define LIBSAKUSEN_CLIENT_EXIMP extern
  #else
    #define LIBSAKUSEN_CLIENT_API   __declspec(dllimport)
    #define LIBSAKUSEN_CLIENT_EXIMP
  #endif
#else
  #define LIBSAKUSEN_CLIENT_API
#endif

#endif // LIBSAKUSEN_CLIENT_GLOBAL_H

