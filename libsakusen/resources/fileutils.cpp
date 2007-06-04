#include "fileutils.h"

#include "stringutils.h"
#include "fileioexn.h"

#include <sys/stat.h>

#include <cerrno>
#include <list>

using namespace std;

using namespace sakusen;
using namespace sakusen::comms;

namespace sakusen {
namespace resources {

/** \brief Wrapper for native read function.
 *
 * Returns number of bytes read.  Throws FileIOExn on error
 *
 * \todo Perhaps should be inlined?
 */
size_t fileUtils_read(int fd, void* buffer, size_t bufferLen)
{
#ifdef WIN32
  int retVal = _read(fd, buffer, (sint32)bufferLen);
#else
  /** \bug I note that if length>SSIZE_MAX, then this might cause problems (the
   * result is undefined), so we would need to read the file in chunks */
  ssize_t retVal = read(fd, buffer, bufferLen);
#endif
  if (retVal == -1)
    throw FileIOExn("read");
  return retVal;
}

/** \brief Wrapper for native write function.
 *
 * Returns number of bytes written.  Throws FileIOExn on error
 *
 * \todo Perhaps should be inlined?
 */
size_t fileUtils_write(int fd, const void* buffer, size_t length)
{
#ifdef WIN32
  int retVal = _write(fd, buffer, (sint32)length);
#else
  /** \bug I note that if length>SSIZE_MAX, then this might cause problems (the
   * result is undefined), so we would need to read the file in chunks */
  ssize_t retVal = write(fd, buffer, length);
#endif
  if (retVal == -1)
    throw FileIOExn("write");
  return retVal;
}

/** \brief Make a directory, creating parent directories as necessary.
 *
 * \param path Directory to create.
 * \param mode Mode to use for all directories created.
 */
void fileUtils_mkdirRecursive(String path, mode_t mode)
{
  if (path=="") {
    return;
  }
  
  list<String> pathElements = stringUtils_split(path, FILE_SEP);

  if (pathElements.empty()) {
    return;
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
          throw FileIOExn("mkdir");
        }
      } else {
        throw FileIOExn("stat");
      }
    }
  }

  return;
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
/** \internal \brief Static data for use by prefixFilter, set by
 * fileUtils_findMatches. */
static String prefix;

/** \internal \brief Static function passed to scandir by
 * fileUtils_findMatches.
 */
static int prefixFilter(const struct dirent* entry)
{
  /* Returns non-zero iff the entry name starts with prefix */
  return 0 == String(entry->d_name).compare(0, prefix.size(), prefix);
}

std::list<String> fileUtils_findMatches(
    const String& directory,
    const String& name
  )
{
  /** \todo This function needs to be more general to cope with such things as
   * archives and subdirectories, but for the moment it just searches for plain
   * old files. */

  prefix = name;
  struct dirent** nameList;
  int numEntries =
    scandir(directory.c_str(), &nameList, prefixFilter, versionsort);

  if (numEntries == -1) {
    /* The existence of the directory should already have been checked, so any
     * error is probably a real problem.  However, it can crop up in
     * libsakusen-resourcesp-test because I'm too lazy to stop it, so we just
     * stick to a debug message */
    Debug("error scanning directory: " << errorUtils_errorMessage(errno));
    return list<String>();
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

/** \brief Gets a home directory suitable for the current
 * platform */
String fileUtils_getHome()
{
#ifdef WIN32
  char *buffer;
  errno_t err;
  err=_dupenv_s(&buffer, NULL, "HOMEDRIVE");
  String path(buffer);
  err=_dupenv_s(&buffer, NULL, "HOMEPATH");
  path += buffer;
  if(err)
    Fatal("Error getting environment variable");
  return path;
#else
  return getenv("HOME");
#endif
}

/** \brief Extracts the part of a path which is the bare filename, without the
 * directory */
String fileUtils_notDirPart(const String& path)
{
  size_t seperator = path.rfind(FILE_SEPC, path.length());
  if (seperator == String::npos) {
    return path;
  }
  return path.substr(seperator+1);
}

}}
