#ifndef LIBSAKUSEN_RESOURCES__FILEUTILS_H
#define LIBSAKUSEN_RESOURCES__FILEUTILS_H

#include <sakusen/resources/libsakusen-resources-global.h>

#include <list>
#include <boost/filesystem/path.hpp>

namespace sakusen {
namespace resources {

LIBSAKUSEN_RESOURCES_API int fileUtils_fileno(FILE*);

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

#endif // LIBSAKUSEN_RESOURCES__FILEUTILS_H

