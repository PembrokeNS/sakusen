#include "fileresourceinterface.h"

#include "libsakusen-resources-global.h"
#include "iarchive.h"
#include "maptemplate.h"
#include "errorutils.h"
#include "lockingfilereader.h"
#include "lockingfilewriter.h"
#include "fileutils.h"

#include <sys/stat.h>
#include <dirent.h>

#include <pcrecpp.h>

using namespace std;

using namespace sakusen::resources;
using namespace sakusen::comms;

FileResourceInterface::FileResourceInterface(const String& d) :
  saveDirectory(d),
  directories()
{
  directories.push_back(d);
}

FileResourceInterface::FileResourceInterface(const std::list<String>& d) :
  saveDirectory(d.front()),
  directories(d)
{
}

/* static data for use by prefixFilter, set by findMatches */
String prefix;

int prefixFilter(const struct dirent* entry)
{
  /* Returns non-zero iff the entry name starts with prefix */
  return 0 == String(entry->d_name).compare(0, prefix.size(), prefix);
}

std::list<String> FileResourceInterface::findMatches(
    const String& directory,
    const String& name
  ) const
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
    result.push_front(directory + FILE_SEP + nameList[numEntries]->d_name);
    free(nameList[numEntries]);
  }
  free(nameList);
  
  return result;
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
  /* Find the appropriate subdirectory name */
  String subdir = getSubdir(type);

  /* Search through all our directories to find all matching files */
  list<String> matchingFiles;

  for (list<String>::const_iterator directory = directories.begin();
      directory != directories.end(); directory++) {
    String resourceDir = *directory + subdir;
    struct stat s;
    if (-1 == stat(resourceDir.c_str(), &s)) {
      /* The directory was not there (or other
       * error), but this is not an error, because a
       * resource repository might not have all the different types of resource
       * in it. */
      continue;
    }
    list<String> newMatches = findMatches(resourceDir, name);
    matchingFiles.splice(matchingFiles.end(), newMatches);
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
  String fileName = matchingFiles.front();

  LockingFileReader file(fileName);
  /* FIXME: This blocks until a lock is achieved.  This could be a bad thing */
  sint64 length = file.getLength(true);
  if (length == -1) {
    /* Indicates error while getting length */
    *result = resourceSearchResult_error;
    error = String("error getting length of file '") + fileName + "': " +
      errorUtils_errorMessage(errno);
    return NULL;
  }
  /* FIXME: For the moment we have a length constraint to prevent excessive
   * memory allocation.  In the long run we probably want to make IArchive
   * abstract so that we can have a version that extracts data directly from
   * the file rather than copying it all over the place as we do at present */
  if (length > (1 << 20)) {
    Fatal("file size exceeded arbitrary limit");
  }
  uint8* fileAsArray = new uint8[length];
  ssize_t bytesRead;
  if (length != (bytesRead = file.getWholeFile(fileAsArray, length, true))) {
    switch (bytesRead) {
      case -1:
        error = String("error reading file: ") + errorUtils_errorMessage(errno);
        *result = resourceSearchResult_error;
        delete[] fileAsArray;
        return NULL;
      default:
        error = String("read only ") + numToString(bytesRead) +
          " of " + numToString(length) + " bytes";
        *result = resourceSearchResult_error;
        delete[] fileAsArray;
        return NULL;
    }
  }
  file.releaseLock();
  /* TODO: double-check the hash of the data to ensure that there's been no
   * corruption or foul play */
  IArchive fileAsArchive(fileAsArray, length);
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

  /* TODO: The following switch could be neatly made into a template method */
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

  /* Currently blocks until a lock can be obtained.  This might be bad */
  if (writer.write(archive, true)) {
    error = "error writing file: " + errorUtils_errorMessage(errno);
    return true;
  }
  
  writer.releaseLock();
  return false;
}

