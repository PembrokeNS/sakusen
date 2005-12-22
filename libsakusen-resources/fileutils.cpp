#include "fileutils.h"

#include "stringutils.h"

#include <errno.h>
#include <sys/stat.h>

#include <list>

using namespace std;

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
        if (-1 == mkdir(partialPath.c_str(), mode)) {
          return -1;
        }
      } else {
        return -1;
      }
    }
  }

  return 0;
}

