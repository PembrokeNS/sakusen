#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <sys/types.h>

#include <list>

#include "libsakusen-global.h"
#include "libsakusen-resources-global.h"

#ifdef WIN32
#include <direct.h>
#include <io.h>
#define NativeMkdir(a,b) _mkdir(a)
/** \bug This should be _stat(a,b), but it doesn't seem to work properly.
 * This workaround of calling it 0 works because the struct stat part of
 * the result is not used anywhere at the moment */
#define NativeStat(a,b) _stat(a,b)
#define NativeStructStat struct _stat
#define S_ISDIR(mode) (((mode) & _S_IFDIR) == _S_IFDIR)
#define NativeUnlink(a) _unlink(a)
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#define NativeMkdir(a,b) mkdir(a, b)
#define NativeStat(a,b) stat(a,b)
#define NativeFstat(a,b) fstat(a,b)
#define NativeStructStat struct stat
#define NativeUnlink(a) unlink(a)
#endif

namespace sakusen {
namespace resources {

LIBSAKUSEN_RESOURCES_API size_t fileUtils_read(int fd, void* buffer, size_t bufferLen);

LIBSAKUSEN_RESOURCES_API size_t fileUtils_write(int fd, const void* buffer, size_t length);

LIBSAKUSEN_RESOURCES_API int fileUtils_mkdirRecursive(String path, mode_t mode);

LIBSAKUSEN_RESOURCES_API std::list<String> fileUtils_findMatches(
    const String& directory,
    const String& name
  );

LIBSAKUSEN_RESOURCES_API String fileUtils_getHome();

}}

#endif // FILEUTILS_H

