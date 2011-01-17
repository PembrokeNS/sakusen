#include <sakusen/fileutils.h>

#include <cerrno>
#include <cstdio>
#include <list>

#include <sys/stat.h>

#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <sakusen/stringutils.h>
#include <sakusen/resources/fileioexn.h>

using namespace std;

using namespace sakusen;

namespace sakusen {

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

/** \brief Returns all files in the directory whose name
 * begins with the given name.
 */
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

/** Returns the user-writable directory where
 * sakusen should store . */
boost::filesystem::path fileUtils_configDirectory()
{
#ifdef WIN32
  return fileUtils_getHome()/"sakusen";
#else
  char const* xdgConfigHome = getenv("XDG_CONFIG_HOME");
  if (xdgConfigHome && *xdgConfigHome) {
    boost::filesystem::path home(xdgConfigHome);
    return home/"sakusen";
  } else {
    return fileUtils_getHome()/".config"/"sakusen";
  }
#endif
}

}
