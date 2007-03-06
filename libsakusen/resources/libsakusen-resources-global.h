#ifndef LIBSAKUSEN_RESOURCES_GLOBAL_H
#define LIBSAKUSEN_RESOURCES_GLOBAL_H

/* Ensure 64-bit ftello and fseeko are available */
#define _FILE_OFFSET_BITS 64

#include "libsakusen-global.h"
#include "libsakusen-comms-global.h"

/** DATA_SUBDIR is the subdirectory of CONFIG_SUBDIR where the game data is to
 * be found */
#define DATA_SUBDIR FILE_SEP "data"

/** FILENAME_REGEX defines a regex which must be matched by filenames */
#define FILENAME_REGEX "[-a-z0-9_.]+"

#if defined(_MSC_VER)
  /* The __declspec stuff for ensuring symbols are exported from DLLs and
   * imported back into libraries */
  #ifdef LIBSAKUSEN_RESOURCES_EXPORTS
    #define LIBSAKUSEN_RESOURCES_API   __declspec(dllexport)
  #else
    #define LIBSAKUSEN_RESOURCES_API   __declspec(dllimport)
  #endif
#else
  #define LIBSAKUSEN_RESOURCES_API
#endif

#ifdef WIN32
/* typederfs for types that don't exist in Windows, to allow for common API.
 * Arguments of these types should not have any effect under Windows */
typedef int mode_t;
#endif

#endif // LIBSAKUSEN_RESOURCES_GLOBAL_H

