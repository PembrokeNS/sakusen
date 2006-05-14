#include "fileresourceinterface.h"

#include "libsakusen-resources-global.h"
#include "iarchive.h"
#include "resourcetype.h"
#include "maptemplate.h"
#include "errorutils.h"
#include "lockingfilereader.h"
#include "lockingfilewriter.h"
#include "fileutils.h"
#include "fileioexn.h"

#include <sys/stat.h>

#include <pcrecpp.h>

using namespace std;
using namespace __gnu_cxx;

using namespace sakusen;
using namespace sakusen::comms;
using namespace sakusen::resources;

FileResourceInterface::FileResourceInterface(const String& d) :
  saveDirectory(d),
  directories()
{
  directories.push_back(d);
}

FileResourceInterface::FileResourceInterface(const std::vector<String>& d) :
  saveDirectory(d.front()),
  directories(d)
{
}

String FileResourceInterface::getSubdir(ResourceType type)
{
  switch (type) {
    case resourceType_universe:
      return String(FILE_SEP "universe" FILE_SEP);
    case resourceType_mapTemplate:
      return String(FILE_SEP "maptemplate" FILE_SEP);
    default:
      Fatal("Unexpected ResourceType: " << type);
  }
}

void* FileResourceInterface::internalSearch(
    String name,
    ResourceType type,
    const void* arg,
    ResourceSearchResult* result
  )
{
  uint64 length;
  size_t lengthAsSizeT;
  /* Find the appropriate subdirectory name */
  String subdir = getSubdir(type);
  /*QDebug("Searching for " << name << " in subdir " << subdir);*/

  /* This hash map will contain as keys the filenames, and as values the paths
   * to those files.  We store the names also so that we don't consider it an
   * ambiguous result if there are two files of the same name in different
   * places; we rather assume that they are identical. */
  hash_map<String, String, StringHash> matchingFiles(10);

  /* Search through all our directories to find all matching files */
  for (vector<String>::const_iterator directory = directories.begin();
      directory != directories.end(); directory++) {
    String resourceDir = *directory + subdir;
#ifndef WIN32
    /** \bug For some reason this fails to find the directory under Windows.
     * This needs investigation */
    NativeStructStat s;
    if (-1 == NativeStat(resourceDir.c_str(), &s)) {
      /* The directory was not there (or other
       * error), but this is not an error, because a
       * resource repository might not have all the different types of resource
       * in it. */
      /*QDebug("Could not stat resource search path '" << resourceDir << "' (" << errorUtils_parseErrno(errno) << ")");*/
      continue;
    }
#endif
    list<String> newMatches = fileUtils_findMatches(resourceDir, name);
    while (!newMatches.empty()) {
      String path = newMatches.front();
      newMatches.pop_front();
      String fileName = fileUtils_notDirPart(path);
      if (0 == matchingFiles.count(fileName)) {
        QDebug("Adding " << fileName << " at " << path);
        matchingFiles[fileName] = path;
      }
    }
  }

  /* Check how many results we found */
  switch (matchingFiles.size()) {
    case 0:
      *result = resourceSearchResult_notFound;
      return NULL;
    case 1:
      break;
    default:
      *result = resourceSearchResult_ambiguous;
      return NULL;
  }

  /* We have found exactly one matching file, so we try to open it */
  String path = matchingFiles.begin()->second;

  LockingFileReader file(path);
  /** \bug This blocks until a lock is achieved.  This could be a bad thing */
  try {
    length = file.getLength(true);
  } catch (FileIOExn* e) {
    delete e;
    /* Indicates error while getting length */
    *result = resourceSearchResult_error;
    error = String("error getting length of file '") + path + "': " +
      errorUtils_errorMessage(errno);
    return NULL;
  }
  lengthAsSizeT = static_cast<size_t>(length);

  /** \bug For the moment we have a length constraint to prevent excessive
   * memory allocation.  In the long run we probably want to make IArchive
   * abstract so that we can have a version that extracts data directly from
   * the file rather than copying it all over the place as we do at present */
  if (length > (1 << 20)) {
    Fatal("file size exceeded arbitrary limit");
  }
  uint8* fileAsArray = new uint8[lengthAsSizeT];
  size_t bytesRead;
  if (lengthAsSizeT != (bytesRead = file.getWholeFile(fileAsArray, lengthAsSizeT, true))) {
    switch (bytesRead) {
      case -1:
        error = String("error reading file: ") + errorUtils_errorMessage(errno);
        *result = resourceSearchResult_error;
        delete[] fileAsArray;
        return NULL;
      default:
        error = String("read only ") + numToString(static_cast<sint32>(bytesRead)) +
          " of " + numToString(length) + " bytes";
        *result = resourceSearchResult_error;
        delete[] fileAsArray;
        return NULL;
    }
  }
  file.releaseLock();
  /** \todo Check the hash of the data to ensure that there's been no
   * corruption or foul play */
  IArchive fileAsArchive(fileAsArray, lengthAsSizeT);
  delete[] fileAsArray;
  void* resource = NULL;

  try {
    switch (type) {
      case resourceType_universe:
        resource = Universe::loadNew(fileAsArchive);
        break;
      case resourceType_mapTemplate:
        resource = new MapTemplate(
            MapTemplate::load(fileAsArchive, static_cast<const Universe*>(arg))
          );
        break;
      default:
        Fatal("unexpected ResourceType: " << type);
    }
  } catch (DeserializationExn* e) {
    *result = resourceSearchResult_error;
    delete e;
    error = String("exception: ") + e->message;
    return NULL;
  }
  
  if (!fileAsArchive.isFinished()) {
    *result = resourceSearchResult_error;
    switch(type) {
      case resourceType_universe:
        delete static_cast<Universe*>(resource);
        break;
      case resourceType_mapTemplate:
        delete static_cast<MapTemplate*>(resource);
        break;
      default:
        Fatal("unexpected ResourceType: " << type);
    }
    
    resource = NULL;
    error = "archive was not exhausted by deserialization - could it be "
      "corrupted?";
    return NULL;
  }
  
  *result = resourceSearchResult_success;
  return resource;
}

bool FileResourceInterface::internalSave(
    const void* resource,
    ResourceType type
  )
{
  /* Get the nice, short name and extension and generate the archive */
  String shortName;
  String extension;
  OArchive archive;

  /** \todo The following switch could be neatly made into a template method */
  switch(type) {
    case resourceType_universe:
      {
        const Universe* u = static_cast<const Universe*>(resource);
        shortName = u->getInternalName();
        extension = "sakusenuniverse";
        u->store(archive);
      }
      break;
    case resourceType_mapTemplate:
      {
        const MapTemplate* m = static_cast<const MapTemplate*>(resource);
        shortName = m->getInternalName();
        extension = "sakusenmaptemplate";
        m->store(archive);
      }
      break;
    default:
      Fatal("unexpected ResourceType: " << type);
  }

  if (!pcrecpp::RE(FILENAME_REGEX).FullMatch(shortName)) {
    error = String("Name '") + shortName + "' contains invalid characters and "
      "cannot be used as part of a filename.  The name must match " +
      FILENAME_REGEX + ".";
    return true;
  }

  String hash = archive.getSecureHashAsString();
  String fullName = shortName + "." + hash + "." + extension;
  String directory = saveDirectory+getSubdir(type);

  if(-1==fileUtils_mkdirRecursive(directory, 0777)) {
    error = "error creating directory: " + errorUtils_errorMessage(errno);
    return true;
  }

  LockingFileWriter writer(directory + fullName);

  /** \bug Currently blocks until a lock can be obtained.  This might be bad */
  if (writer.write(archive, true)) {
    error = "error writing file: " + errorUtils_errorMessage(errno);
    return true;
  }
  
  writer.releaseLock();
  return false;
}

