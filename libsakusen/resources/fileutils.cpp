#include "fileutils.h"

#include "stringutils.h"
#include "fileioexn.h"

#include <sys/stat.h>

#ifdef WIN32
#include <direct.h>
#include <io.h>
#define NativeMkdir(a,b) _mkdir(a)
#else
#include <dirent.h>
#define NativeMkdir(a,b) mkdir(a, b)
#endif

#include <cerrno>
#include <list>

using namespace std;

using namespace sakusen;

namespace sakusen {
namespace resources {

size_t fileUtils_read(int fd, void* buffer, size_t bufferLen)
{
#ifdef WIN32
  int retVal = _read(fd, buffer, bufferLen);
#else
  /** \bug I note that if length>SSIZE_MAX, then this might cause problems (the
   * result is undefined), so we would need to read the file in chunks */
  ssize_t retVal = read(fd, buffer, bufferLen);
#endif
  if (retVal == -1)
    throw new FileIOExn("read");
  return retVal;
}

size_t fileUtils_write(int fd, const void* buffer, size_t length)
{
#ifdef WIN32
  int retVal = _write(fd, buffer, length);
#else
  /** \bug I note that if length>SSIZE_MAX, then this might cause problems (the
   * result is undefined), so we would need to read the file in chunks */
  ssize_t retVal = write(fd, buffer, length);
#endif
  if (retVal == -1)
    throw new FileIOExn("write");
  return retVal;
}

int fileUtils_mkdirRecursive(String path, mode_t mode)
{
  if (path=="") {
    return 0;
  }
  
  list<String> pathElements = stringUtils_split(path, FILE_SEP);

  if (pathElements.empty()) {
    return 0;
  }
  
  String partialPath = ( path[0] == FILE_SEPC ? FILE_SEP : "" );
  partialPath += pathElements.front();
  list<String>::iterator pathElement = pathElements.begin();
  ++pathElement;
  struct stat s;

  for ( ; pathElement != pathElements.end(); ++pathElement) {
    partialPath += FILE_SEP;
    partialPath += *pathElement;
    if (-1 == stat(partialPath.c_str(), &s)) {
      if (errno == ENOENT) {
        //Debug("trying to make " << partialPath);
        if (-1 == NativeMkdir(partialPath.c_str(), mode)) {
          return -1;
        }
      } else {
        return -1;
      }
    }
  }

  return 0;
}

#ifdef WIN32
list<String> fileUtils_findMatches(
    const String& directory,
    const String& name
  )
{
  String pattern = directory + FILE_SEP + name + "*";
  /*Debug("Searching with pattern" << pattern);*/
  struct __finddata64_t file;
  intptr_t hFile;
  list<String> result;

  if (-1 != (hFile = _findfirst64(pattern.c_str(), &file))) {
    do {
      result.push_back(directory + FILE_SEP + file.name);
    } while (-1 != (hFile = _findnext64(hFile, &file)));
  }
  _findclose(hFile);

  return result;
}
#else
/* static data for use by prefixFilter, set by findMatches */
String prefix;

int prefixFilter(const struct dirent* entry)
{
  /* Returns non-zero iff the entry name starts with prefix */
  return 0 == String(entry->d_name).compare(0, prefix.size(), prefix);
}

std::list<String> fileUtils_findMatches(
    const String& directory,
    const String& name
  )
{
  /* TODO: This function needs to be more general to cope with such things as
   * archives and subdirectories, but for the moment it just searches for plain
   * old files. */

  /* The existence of the directory should already have been checked, so any
   * error is a real problem */
  prefix = name;
  struct dirent** nameList;
  int numEntries =
    scandir(directory.c_str(), &nameList, prefixFilter, versionsort);

  if (numEntries == -1) {
    Fatal("error scanning directory: " << errorUtils_errorMessage(errno));
  }

  /* Put all the results into the list in String format */
  list<String> result;
  
  while (numEntries--) {
    result.push_back(directory + FILE_SEP + nameList[numEntries]->d_name);
    free(nameList[numEntries]);
  }
  free(nameList);
  
  return result;
}
#endif

String fileUtils_getHome()
{
#ifdef WIN32
  String path(getenv("HOMEDRIVE"));
  path += getenv("HOMEPATH");
  return path;
#else
  return getenv("HOME");
#endif
}

}}
