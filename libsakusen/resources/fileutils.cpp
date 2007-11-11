#include "fileutils.h"

#include "stringutils.h"
#include "fileioexn.h"

#include <sys/stat.h>

#include <cerrno>
#include <list>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/predicate.hpp>

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
  assert(static_cast<sint32>(bufferLen) <= numeric_limits<sint32>::max());
  int retVal = _read(fd, buffer, (sint32)bufferLen);
#else
  /** \bug I note that if length>SSIZE_MAX, then this might cause problems (the
   * result is undefined), so we would need to read the file in chunks */
  assert(bufferLen <= size_t(numeric_limits<ssize_t>::max()));
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
void fileUtils_mkdirRecursive(const boost::filesystem::path& path)
{
  if (path.empty()) {
    return;
  }

  if (boost::filesystem::exists(path)) {
    return;
  }

  fileUtils_mkdirRecursive(path.branch_path());
  boost::filesystem::create_directory(path);
}

list<boost::filesystem::path> fileUtils_findMatches(
    const boost::filesystem::path& directory,
    const String& name
  )
{
  list<boost::filesystem::path> result;

  for (boost::filesystem::directory_iterator
      item = boost::filesystem::directory_iterator(directory);
      item != boost::filesystem::directory_iterator(); ++item) {
    if (boost::algorithm::starts_with(item->leaf(), name)) {
      result.push_back(*item);
    }
  }

  return result;
}

/** \brief Gets a home directory suitable for the current
 * platform */
boost::filesystem::path fileUtils_getHome()
{
#ifdef WIN32
  char *buffer;
  errno_t err;
  //err=_dupenv_s(&buffer, NULL, "HOMEDRIVE");
  
  err=_dupenv_s(&buffer, NULL, "APPDATA");
  String path(buffer);
  //path += buffer;
  if(err)
    Fatal("Error getting environment variable");
  return path;
#else
  return getenv("HOME");
#endif
}

}}
