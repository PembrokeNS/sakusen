#ifndef LIBSAKUSEN_RESOURCES_GLOBAL_H
#define LIBSAKUSEN_RESOURCES_GLOBAL_H

/* Ensure 64-bit ftello and fseeko are available
 *
 * \todo See whether this is actually necessary any more given that we're
 * using boost::filesystem::fstream tell methods now. */
#define _FILE_OFFSET_BITS 64

#include "libsakusen-global.h"
#include "libsakusen-comms-global.h"

#include "pcrecpp.h"

/** DATA_SUBDIR is the subdirectory of CONFIG_SUBDIR where the game data is to
 * be found */
#define DATA_SUBDIR "data"

#if defined(_MSC_VER)
  /* The __declspec stuff for ensuring symbols are exported from DLLs and
   * imported back into libraries */
  #ifdef LIBSAKUSEN_RESOURCES_EXPORTS
    #define LIBSAKUSEN_RESOURCES_API   __declspec(dllexport)
  #else
    #define LIBSAKUSEN_RESOURCES_API   __declspec(dllimport)
  #endif
#else
  #if defined(__GNUC__) && (__GNUC__ >= 4)
    #define LIBSAKUSEN_RESOURCES_API __attribute__ ((visibility ("default")))
  #else
    #define LIBSAKUSEN_RESOURCES_API
  #endif
#endif

namespace sakusen {
namespace resources {

/** \brief Defines a regex which must be matched by filenames.
 *
 * \warning Do not expect this to be initialised during static constructors. */
extern pcrecpp::RE filenameRegex;

/** Defines a regex which must be matched by paths in the Skusen
 * VFS
 *
 * \warning Do not expect this to be initialised during static constructors. */
extern pcrecpp::RE pathRegex;

}}

#endif // LIBSAKUSEN_RESOURCES_GLOBAL_H

