#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "libsakusen-resources-global.h"

#include <list>
#include <boost/filesystem/path.hpp>

#ifdef _MSC_VER
#define NativeFileno _fileno
#endif

namespace sakusen {
namespace resources {

LIBSAKUSEN_RESOURCES_API void fileUtils_mkdirRecursive(
    const boost::filesystem::path& path
  );

/** \brief Returns all files in the directory whose name
 * begins with the given name.
 */
LIBSAKUSEN_RESOURCES_API std::list<boost::filesystem::path>
  fileUtils_findMatches(
    const boost::filesystem::path& directory,
    const String& name
  );

LIBSAKUSEN_RESOURCES_API boost::filesystem::path fileUtils_getHome();

}}

#endif // FILEUTILS_H

