#ifndef LIBSAKUSEN__FILEUTILS_H
#define LIBSAKUSEN__FILEUTILS_H

#include <sakusen/global.h>

#include <list>
#include <cstdio>
#include <boost/filesystem/path.hpp>

namespace sakusen {

LIBSAKUSEN_API int fileUtils_fileno(FILE*);

LIBSAKUSEN_API void fileUtils_mkdirRecursive(
    const boost::filesystem::path& path
  );

LIBSAKUSEN_API std::list<boost::filesystem::path>
  fileUtils_findMatches(
    const boost::filesystem::path& directory,
    const String& name
  );

LIBSAKUSEN_API boost::filesystem::path fileUtils_getHome();

LIBSAKUSEN_API boost::filesystem::path fileUtils_configDirectory();

}

#endif // LIBSAKUSEN__FILEUTILS_H

