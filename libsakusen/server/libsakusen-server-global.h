#ifndef LIBSAKUSEN_SERVER_GLOBAL_H
#define LIBSAKUSEN_SERVER_GLOBAL_H

#if defined(_MSC_VER)
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
  #define LIBSAKUSEN_SERVER_API
#endif

#endif // LIBSAKUSEN_SERVER_GLOBAL_H

