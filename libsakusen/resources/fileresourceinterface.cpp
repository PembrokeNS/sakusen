#include "fileresourceinterface.h"

#include "libsakusen-resources-global.h"
#include "iarchive.h"
#include "resourcetype.h"
#include "maptemplate.h"
#include "errorutils.h"
#include "filereader.h"
#include "filewriter.h"
#include "fileutils.h"
#include "fileioexn.h"

#include <sys/stat.h>
#include <ltdl_hacked.h>

#include <pcrecpp.h>

using namespace std;
using namespace __gnu_cxx;

using boost::shared_ptr;
using boost::shared_array;

using namespace sakusen;
using namespace sakusen::comms;
using namespace sakusen::resources;

ResourceInterface::Ptr FileResourceInterface::create(
    const String& directory,
    bool loadModules
  )
{
  shared_ptr<FileResourceInterface>
    result(new FileResourceInterface(directory, loadModules));
  result->ptrToThis = result;
  return result;
}

ResourceInterface::Ptr FileResourceInterface::create(
    const std::vector<String>& directories,
    bool loadModules
  )
{
  shared_ptr<FileResourceInterface>
    result(new FileResourceInterface(directories, loadModules));
  result->ptrToThis = result;
  return result;
}

FileResourceInterface::FileResourceInterface(const String& d, bool lM) :
  saveDirectory(d),
  directories(),
  loadModules(lM)
{
  directories.push_back(d);
}

FileResourceInterface::FileResourceInterface(
    const std::vector<String>& d,
    bool lM
  ) :
  saveDirectory(d.front()),
  directories(d),
  loadModules(lM)
{
}

String FileResourceInterface::getSubdir(ResourceType type)
{
  switch (type) {
    case resourceType_universe:
    case resourceType_source:
      return String(FILE_SEP "universe" FILE_SEP);
    case resourceType_mapTemplate:
      return String(FILE_SEP "maptemplate" FILE_SEP);
    case resourceType_module:
      return String(FILE_SEP "module" FILE_SEP);
    case resourceType_replay:
    case resourceType_replayIndex:
      return String(FILE_SEP "replay" FILE_SEP);
    default:
      Fatal("Unexpected ResourceType: " << type);
  }
}

String FileResourceInterface::getExtension(ResourceType type)
{
  switch (type) {
    case resourceType_universe:
      return ".sakusenuniverse";
    case resourceType_source:
      return ".sakusensource";
    case resourceType_mapTemplate:
      return ".sakusenmaptemplate";
    case resourceType_module:
      /* Can't know the extension here because it might end in ".la" or ".dll"
       * or ".so" */
      return "";
    case resourceType_replay:
      return ".sakusenreplay";
    case resourceType_replayIndex:
      return ".sakusenreplayindex";
    default:
      Fatal("Unexpected ResourceType: " << type);
  }
}

String FileResourceInterface::fileSearch(
    const String& name,
    ResourceType type,
    ResourceSearchResult* result
  )
{
  /* Find the appropriate subdirectory and extension */
  String subdir = getSubdir(type);
  String extension = getExtension(type);
  /*QDebug("Searching for " << name << " in subdir " << subdir);*/

  /* This hash map will contain as keys the filenames, and as values the paths
   * to those files.  We store the names also so that we don't consider it an
   * ambiguous result if there are two files of the same name in different
   * places; we rather assume that they are identical. */
  hash_map_string<String>::type matchingFiles(10);

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
    /*QDebug("Searching "<<resourceDir);*/
    list<String> newMatches = fileUtils_findMatches(resourceDir, name);
    while (!newMatches.empty()) {
      String path = newMatches.front();
      newMatches.pop_front();
      /*QDebug("Found "<<path);*/
      String fileName = fileUtils_notDirPart(path);
      /* check that the file has the correct extension */
      if (fileName.size() < extension.size() ||
          0 != fileName.compare(
            fileName.size() - extension.size(), extension.size(), extension
          )) {
        continue;
      }
      if (0 == matchingFiles.count(fileName)) {
        /*QDebug("Adding " << fileName << " at " << path);*/
        matchingFiles[fileName] = path;
      }
    }
  }

  /* Check how many results we found */
  switch (matchingFiles.size()) {
    case 0:
      *result = resourceSearchResult_notFound;
      return "";
    case 1:
      break;
    default:
      *result = resourceSearchResult_ambiguous;
      return "";
  }

  /* We have found exactly one matching file, so we try to open it */
  assert(matchingFiles.begin()->second != "");
  return matchingFiles.begin()->second;
}

shared_ptr<void> FileResourceInterface::internalSearch(
    const String& name,
    ResourceType type,
    const void* arg,
    ResourceSearchResult* result
  )
{
  uint64 length;
  size_t lengthAsSizeT;
  String path = fileSearch(name, type, result);

  if (path == "")
    return shared_ptr<void>();

  FileReader file(path);
  try {
    length = file.getLength();
  } catch (FileIOExn& e) {
    /* Indicates error while getting length */
    *result = resourceSearchResult_error;
    error = String("error getting length of file '") + path + "': " +
      errorUtils_errorMessage(errno);
    return shared_ptr<void>();
  }
  lengthAsSizeT = static_cast<size_t>(length);

  /** \bug For the moment we have a length constraint to prevent excessive
   * memory allocation.  In the long run we probably want to make IArchive
   * abstract so that we can have a version that extracts data directly from
   * the file rather than copying it all over the place as we do at present */
  if (length > (1 << 20)) {
    Fatal("file size exceeded arbitrary limit");
  }
  shared_array<uint8> fileAsArray(new uint8[lengthAsSizeT]);
  size_t bytesRead;
  if (lengthAsSizeT !=
      (bytesRead = file.getWholeFile(fileAsArray, lengthAsSizeT))
    ) {
    switch (bytesRead) {
      case -1:
        error =
          String("error reading file: ") + errorUtils_errorMessage(errno);
        *result = resourceSearchResult_error;
        return shared_ptr<void>();
      default:
        error =
          String("read only ") + numToString(static_cast<sint32>(bytesRead)) +
          " of " + numToString(length) + " bytes";
        *result = resourceSearchResult_error;
        return shared_ptr<void>();
    }
  }
  /** \todo Check the hash of the data to ensure that there's been no
   * corruption or foul play */
  IArchive fileAsArchive(fileAsArray, lengthAsSizeT);
  shared_ptr<void> resource;

  try {
    switch (type) {
      case resourceType_universe:
        resource.reset(Universe::loadNew(fileAsArchive, ptrToThis.lock()));
        break;
      case resourceType_mapTemplate:
        resource.reset(new MapTemplate(
            MapTemplate::load(
              fileAsArchive,
              static_cast<const Universe::ConstPtr*>(arg)
            )
          ));
        break;
      default:
        Fatal("unexpected ResourceType: " << type);
    }
  } catch (DeserializationExn& e) {
    *result = resourceSearchResult_error;
    error = String("exception: ") + e.message;
    return shared_ptr<void>();
  }
  
  if (!fileAsArchive.isFinished()) {
    *result = resourceSearchResult_error;
    error = "archive was not exhausted by deserialization - could it be "
      "corrupted?";
    return shared_ptr<void>();
  }
  
  *result = resourceSearchResult_success;
  return resource;
}
    
void* FileResourceInterface::internalSymbolSearch(
    const String& moduleName,
    const String& symbolName,
    ResourceSearchResult* result
  )
{
  if (!loadModules) {
    *result = resourceSearchResult_notSupported;
    return NULL;
  }
  
  String sourcePath = fileSearch(moduleName, resourceType_source, result);
  if (sourcePath == "") {
    return NULL;
  }
  
  /*Debug("sourcePath='" << sourcePath << "'");*/

  String sourceFile = fileUtils_notDirPart(sourcePath);

  /* replace the extension of the filename to get the module name */
  size_t dot = sourceFile.rfind('.');
  String moduleFile = sourceFile.substr(0, dot) + ".sakusenmodule";
  String modulePath = fileSearch(moduleFile, resourceType_module, result);
  if (modulePath == "") {
    if (*result == resourceSearchResult_notFound) {
      *result = resourceSearchResult_error;
      /** \todo runtime compilation of modules */
      error = "source found but module not found; runtime compiling not yet "
        "implemented";
    }
    return NULL;
  }
  /*Debug("modulePath='" << modulePath << "'");*/

  /** \todo Maybe we should keep a record and not open the same module over and
   * over. ltdl does keep track, so it works, but it's a little inelegant */
  lt_dlhandle moduleHandle = lt_dlopenext(modulePath.c_str());
  if (moduleHandle == NULL) {
    *result = resourceSearchResult_error;
    error = String("lt_dlopen() failed: ") + lt_dlerror();
    return NULL;
  }
  /* We could make the module resident (it cannot be closed) so as to avoid
   * segfault madness that would ensue if it was closed, as follows:
  if (lt_dlmakeresident(moduleHandle)) {
    Fatal("lt_dlmakeresident() failed");
  }
   * However, that this causes libltdl to leak the memory used to store the
   * module name and such data, which bugs me, so I'm not doing that now. I
   * will say instead: Never Close Any Modules.  Let lt_dlexit close them for
   * us when it is called at shutdown */

  lt_ptr symbol = lt_dlsym(moduleHandle, symbolName.c_str());

  if (symbol == NULL) {
    *result = resourceSearchResult_error;
    error = "lt_dlsym(..., \"" + symbolName + "\") failed: " + lt_dlerror();
    return NULL;
  }

  *result = resourceSearchResult_success;
  return symbol;
}

bool FileResourceInterface::internalSave(
    const boost::shared_ptr<const void>& resource,
    ResourceType type
  )
{
  try {
    /* Get the nice, short name and generate the archive */
    String shortName;
    OArchive archive;

    /** \todo The following switch could be neatly made into a template method */
    switch(type) {
      case resourceType_universe:
        {
          Universe::ConstPtr u =
            boost::static_pointer_cast<const Universe>(resource);
          shortName = u->getInternalName();
          u->store(archive);
        }
        break;
      case resourceType_mapTemplate:
        {
          MapTemplate::ConstPtr m =
            boost::static_pointer_cast<const MapTemplate>(resource);
          shortName = m->getInternalName();
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

    Writer::Ptr writer(openWriter(shortName + "." + hash, type));

    writer->write(archive);
    
    return false;
  } catch (FileIOExn& e) {
    error = e.message;
    return true;
  }
}

Writer::Ptr FileResourceInterface::openWriter(
    const String& name,
    ResourceType type
  )
{
  String directory = saveDirectory+getSubdir(type);
  fileUtils_mkdirRecursive(directory, 0777);

  return FileWriter::Ptr(new FileWriter(
        directory+name+"."+getExtension(type)
      ));
}

