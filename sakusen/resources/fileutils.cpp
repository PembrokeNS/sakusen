#include "fileutils.h"

#include "stringutils.h"
#include "fileioexn.h"

#include <sys/stat.h>

#include <cerrno>
#include <list>
#include <cstdio>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/predicate.hpp>

using namespace std;

using namespace sakusen;
using namespace sakusen::comms;

namespace sakusen {
namespace resources {

#ifdef _MSC_VER
int fileUtils_fileno(FILE* s) { return _fileno(s); }
#else
int fileUtils_fileno(FILE* s) { return fileno(s); }
#endif

/** \brief Make a directory, creating parent directories as necessary.
 *
 * \param path Directory to create.
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
    if (boost::algorithm::starts_with(item->filename(), name)) {
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
    SAKUSEN_FATAL("Error getting environment variable");
  return path;
#else
  return getenv("HOME");
#endif
}

}}
