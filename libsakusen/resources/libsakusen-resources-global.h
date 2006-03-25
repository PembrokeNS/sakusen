#ifndef LIBSAKUSEN_RESOURCES_GLOBAL_H
#define LIBSAKUSEN_RESOURCES_GLOBAL_H

#include "libsakusen-global.h"

/** DATA_SUBDIR is the subdirectory of CONFIG_SUBDIR where the game data is to
 * be found */
#define DATA_SUBDIR FILE_SEP "data"

/** FILENAME_REGEX defines a regex which must be matched by filenames */
#define FILENAME_REGEX "[-a-z0-9_.]+"




#if defined(_MSC_VER)
  /* The __declspec stuff for ensuring symbols are exported from DLLs and
   * imported back into libraries */
  #ifdef LIBSAKUSEN_RESOURCES_EXPORTS
    #define LIBSAK_RES_API   __declspec(dllexport)
    #define LIBSAK_RES_EXIMP extern
  #else
    #define LIBSAK_RES_API   __declspec(dllimport)
    #define LIBSAK_RES_EXIMP
  #endif
#else
  #define LIBSAKUSEN_RESOURCES_API
#endif

#endif // LIBSAKUSEN_RESOURCES_GLOBAL_H

