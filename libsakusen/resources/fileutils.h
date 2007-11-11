#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "libsakusen-resources-global.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <list>
#include <boost/filesystem/path.hpp>

#ifdef WIN32
#include <direct.h>
#include <io.h>
#define NativeStat(a,b) _stat(a,b)
#define NativeFstat(a,b) _fstat(a,b)
#define NativeStructStat struct _stat
#define NativeFtell64(a) _ftelli64(a)
#define NativeFileno(a) _fileno(a)
#else
#include <dirent.h>
#include <sys/unistd.h>
#define NativeStat(a,b) stat(a,b)
#define NativeFstat(a,b) fstat(a,b)
#define NativeStructStat struct stat
/* Note that for ftello to really be 64-bit requires
 * #define _FILE_OFFSET_BITS 64, as in libsakusen-resource-global.h */
#define NativeFtell64(a) ftello(a)
#define NativeFileno(a) fileno(a)
#endif

namespace sakusen {
namespace resources {

LIBSAKUSEN_RESOURCES_API size_t fileUtils_read(
    int fd,
    void* buffer, size_t bufferLen
  );

LIBSAKUSEN_RESOURCES_API size_t fileUtils_write(
    int fd,
    const void* buffer, size_t length
  );

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

