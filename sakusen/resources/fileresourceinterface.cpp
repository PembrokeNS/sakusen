#include <sakusen/resources/fileresourceinterface.h>

#ifdef __GNUC__
  #ifdef ENABLE_LTDL_HACKED
    #include <ltdl_hacked/ltdl_hacked.h>
  #else
    #include <ltdl.h>
  #endif
#endif //__GNUC__: I don't want to use this with MSVC.

#include <sys/stat.h>

#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/xpressive/regex_algorithms.hpp>

#include <sakusen/iarchive.h>
#include <sakusen/resourcetype.h>
#include <sakusen/maptemplate.h>
#include <sakusen/fileutils.h>
#include <sakusen/comms/errorutils.h>
#include <sakusen/resources/filereader.h>
#include <sakusen/resources/filewriter.h>
#include <sakusen/resources/fileioexn.h>
#include <sakusen/resources/pngimage.h>
#include <sakusen/resources/vfs/directorybranch.h>
#include <sakusen/resources/vfs/unionbranch.h>

using namespace std;

using namespace sakusen;
using namespace sakusen::comms;
using namespace sakusen::resources;

ResourceInterface::Ptr FileResourceInterface::create(
    const boost::filesystem::path& directory,
    bool loadModules
  )
{
  boost::shared_ptr<FileResourceInterface>
    result(new FileResourceInterface(directory, loadModules));
  result->ptrToThis = result;
  return result;
}

ResourceInterface::Ptr FileResourceInterface::create(
    const std::vector<boost::filesystem::path>& directories,
    bool loadModules
  )
{
  boost::shared_ptr<FileResourceInterface>
    result(new FileResourceInterface(directories, loadModules));
  result->ptrToThis = result;
  return result;
}

FileResourceInterface::FileResourceInterface(
    const boost::filesystem::path& d,
    bool lM
  ) :
  vfsRoot(vfs::DirectoryBranch::create("", d)),
  loadModules(lM)
{
}

FileResourceInterface::FileResourceInterface(
    const std::vector<boost::filesystem::path>& d,
    bool lM
  ) :
  vfsRoot(),
  loadModules(lM)
{
  vector<vfs::Branch::Ptr> dBranches;
  for (vector<boost::filesystem::path>::const_iterator path = d.begin();
      path != d.end(); ++path) {
    vfs::Branch::Ptr dBranch(vfs::DirectoryBranch::create("", *path));
    dBranches.push_back(dBranch);
  }
  vfsRoot = vfs::UnionBranch::create("", dBranches);
}

String FileResourceInterface::getExtension(ResourceType type) const
{
  switch (type) {
    case resourceType_universe:
      return ".sakusenuniverse";
    case resourceType_source:
      return ".sakusensource";
    case resourceType_mapTemplate:
      return ".sakusenmaptemplate";
    case resourceType_module:
#ifdef _MSC_VER
      /* Have to provide an extension under Windows to disambiguate against
       * debugging info files */
      return ".dll";
#else
      /* Otherwise assume we're looking for a libtool library.  This may not
       * be sufficiently general */
      return ".la";
#endif
    case resourceType_replay:
      return ".sakusenreplay";
    case resourceType_replayIndex:
      return ".sakusenreplayindex";
    default:
      SAKUSEN_FATAL("Unexpected ResourceType: " << type);
  }
}

boost::tuple<boost::shared_ptr<void>, ResourceSearchResult, String>
FileResourceInterface::internalSearch(
    const String& sakusenPath,
    const String& hash,
    ResourceType type,
    Universe::ConstPtr universe
  )
{
  ResourceSearchResult result;
  vfs::Resource resource;
  try {
    boost::tie(resource, result) =
      vfsRoot->search(sakusenPath, getExtension(type));
  } catch (DeserializationExn& e) {
    /* Indicates error while searching */
    error = String("error searching resources for '") +
      sakusenPath + "': " + e.message;
    return boost::make_tuple(
        boost::shared_ptr<void>(),resourceSearchResult_error,""
      );
  }
  Reader::Ptr reader = resource.getReader();

  if (!reader)
    return boost::make_tuple(boost::shared_ptr<void>(), result, "");

  Buffer asBuffer;

  try {
    asBuffer = reader->getBuffer();
  } catch (FileIOExn& e) {
    /* Indicates error while getting length */
    error = String("error getting file '") +
      resource.getSakusenPath() + "': " +
      e.message;
    return boost::make_tuple(
        boost::shared_ptr<void>(),resourceSearchResult_error,""
      );
  }

  /** Check the hash of the data to ensure that there's been no
   * corruption or foul play */
  if (!hash.empty() && hash != asBuffer.getSecureHashAsString()) {
    error = "Hash mismatch!";
    return boost::make_tuple(
        boost::shared_ptr<void>(),resourceSearchResult_error,""
      );
  }
  IArchive fileAsArchive(asBuffer);
  boost::shared_ptr<void> resourcePtr;

  try {
    switch (type) {
      case resourceType_universe:
        resourcePtr.reset(Universe::loadNew(
              fileAsArchive, DeserializationContext(ptrToThis.lock())
            ));
        break;
      case resourceType_mapTemplate:
        resourcePtr.reset(new MapTemplate(
            MapTemplate::load(
              fileAsArchive,
              DeserializationContext(ptrToThis.lock(), universe)
            )
          ));
        break;
      default:
        SAKUSEN_FATAL("unexpected ResourceType: " << type);
    }
  } catch (DeserializationExn& e) {
    error = String("exception: ") + e.message;
    return boost::make_tuple(
        boost::shared_ptr<void>(), resourceSearchResult_error, ""
      );
  }

  if (!fileAsArchive.isFinished()) {
    error = "archive was not exhausted by deserialization - could it be "
      "corrupted?";
    return boost::make_tuple(
        boost::shared_ptr<void>(), resourceSearchResult_error, ""
      );
  }

  return boost::make_tuple(
      resourcePtr, resourceSearchResult_success, resource.getSakusenPath()
    );
}

boost::tuple<Image::Ptr, String>
FileResourceInterface::imageSearch(const String& path)
{
  ResourceSearchResult result;
  vfs::Resource resource;
  boost::tie(resource, result) =
    vfsRoot->search(path, "");

  if (result != resourceSearchResult_success) {
    return boost::make_tuple(Image::Ptr(), "");
  }

  boost::filesystem::path realPath = resource.asPath();
  if (boost::algorithm::ends_with(realPath.leaf(), ".png")) {
    Image::Ptr image(new PngImage(realPath));
    return boost::make_tuple(image, resource.getSakusenPath());
  } else {
    return boost::make_tuple(Image::Ptr(), "");
  }
}

boost::tuple<void*, ResourceSearchResult>
FileResourceInterface::internalSymbolSearch(
    const String& moduleName,
    const String& symbolName
  )
{
  if (!loadModules) {
    return boost::make_tuple<void*>(NULL, resourceSearchResult_notSupported);
  }

  vfs::Resource sourceResource;
  ResourceSearchResult result;
  boost::tie(sourceResource, result) =
    vfsRoot->search(moduleName, getExtension(resourceType_source));
  Reader::Ptr sourceReader = sourceResource.getReader();
  if (!sourceReader) {
    switch (result) {
      case resourceSearchResult_notFound:
        error = "source '"+moduleName+"' not found";
        break;
      case resourceSearchResult_error:
        error = "error loading source '"+moduleName+"'";
        break;
      case resourceSearchResult_ambiguous:
        error = "ambiguous source '"+moduleName+"'";
        break;
      default:
        SAKUSEN_FATAL("unexpected enum value");
    }
    return boost::make_tuple<void*>(NULL, result);
  }

  String sourceSakPath = sourceResource.getSakusenPath();

  /* replace the extension of the filename to get the module name */
  size_t dot = sourceSakPath.rfind('.');
  String moduleSakPath = sourceSakPath.substr(0, dot) + ".sakusenmodule";
  vfs::Branch::Ptr topModuleDir = vfsRoot->getSubBranch("module");
  if (!topModuleDir) {
    error = "There appears to be no 'module' directory in any of the data "
      "locations you have specified";
    return boost::make_tuple<void*>(NULL, resourceSearchResult_error);
  }
  vfs::Resource moduleResource;
  boost::tie(moduleResource, result) =
    topModuleDir->search(moduleSakPath, getExtension(resourceType_module));
  if (!moduleResource) {
    if (result == resourceSearchResult_notFound) {
      result = resourceSearchResult_error;
      /** \todo runtime compilation of modules */
      error = "Source file "+sourceSakPath+
        " found.\nCorresponding "
        "module "+moduleSakPath+".dll/.la"
        " not found; runtime compiling not yet implemented.\n"
        "If you have already compiled this module from the source, try moving "
        "it to the right place.  If the source is at "
        "$dataroot/foo/bar.sakusensource then the module shoule be at "
        "$dataroot/module/foo/bar.sakusenmodule.dll/.la";
    }
    return boost::make_tuple<void*>(NULL, result);
  }
  boost::filesystem::path modulePath = moduleResource.asPath();
  /*SAKUSEN_DEBUG("modulePath='" << modulePath << "'");*/

#ifdef __GNUC__
  /** \todo Maybe we should keep a record and not open the same module over and
   * over. ltdl does keep track, so it works, but it's a little inelegant */
  lt_dlhandle moduleHandle =
    lt_dlopen(modulePath.native_file_string().c_str());
  if (moduleHandle == NULL) {
    error = String("lt_dlopen("+modulePath.native_file_string()+") failed: ") +
      lt_dlerror();
    return boost::make_tuple<void*>(NULL, resourceSearchResult_error);
  }
  /* We could make the module resident (it cannot be closed) so as to avoid
   * segfault madness that would ensue if it was closed, as follows:
  if (lt_dlmakeresident(moduleHandle)) {
    SAKUSEN_FATAL("lt_dlmakeresident() failed");
  }
   * However, that this causes libltdl to leak the memory used to store the
   * module name and such data, which bugs me, so I'm not doing that now. I
   * will say instead: Never Close Any Modules.  Let lt_dlexit close them for
   * us when it is called at shutdown */

  lt_ptr symbol = lt_dlsym(moduleHandle, symbolName.c_str());

  if (symbol == NULL) {
    error = "lt_dlsym(..., \"" + symbolName + "\") failed: " + lt_dlerror();
    return boost::make_tuple<void*>(NULL, resourceSearchResult_error);
  }
#else
  //Equivalent to lt_dlhandle moduleHandle = lt_dlopenext(modulePath.c_str());
  //Opens the library for searching. Must be a dll or an exe.
  /** \bug This should work for UNICODE filenames.*/
  HMODULE moduleHandle = LoadLibrary(modulePath.native_file_string().c_str());
  //Error handling for the above.
  if(moduleHandle == NULL) {
    char buffer[33];
//    _itoa_s(GetLastError(), buffer, 33,2);
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),buffer,
        33, NULL );
    error = "LoadLibrary() failed on "+modulePath.native_file_string()+
      ".\n Error value: " + String(buffer);
    return boost::make_tuple<void*>(NULL, resourceSearchResult_error);
  }

  //Equivalent to  lt_ptr symbol = lt_dlsym(moduleHandle, symbolName.c_str());
  FARPROC symbol = GetProcAddress(moduleHandle,symbolName.c_str());
  //Error handling if this fails.
  if(symbol==NULL)
  {
    char buffer[33];
 //   _itoa_s(GetLastError(), buffer, 33,2);
     FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),buffer,
        33, NULL );
    error = "GetProcAddress() on "+symbolName+" in " + modulePath.directory_string() +
      " failed. Error value: " + String(buffer);
    return boost::make_tuple<void*>(NULL, resourceSearchResult_error);
  }
#endif //__GNUC__

  return boost::make_tuple(symbol, resourceSearchResult_success);
}

bool FileResourceInterface::internalSave(
    const boost::shared_ptr<const void>& resource,
    const String& path,
    ResourceType type
  )
{
  try {
    /* Get the nice, short name and generate the archive */
    String shortName;
    OArchive archive;

    /** \todo The following switch could be neatly made into a template method
     * */
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
        SAKUSEN_FATAL("unexpected ResourceType: " << type);
    }

    if (!regex_match(shortName, filenameRegex)) {
      error = String("Name '") + shortName +
        "' contains invalid characters and "
        "cannot be used as part of a filename.";
      return true;
    }

    if (!regex_match(path, pathRegex)) {
      error = String("Path '") + path + "' contains invalid characters and "
        "cannot be used as part of a filename.";
      return true;
    }

    String hash = archive.getSecureHashAsString();

    Writer::Ptr writer(
        openWriter(path + "/" + shortName + "." + hash + getExtension(type))
      );

    writer->write(archive);

    return false;
  } catch (FileIOExn& e) {
    error = e.message;
    return true;
  }
}

Writer::Ptr FileResourceInterface::openWriter(
    const String& sakusenPath
  )
{
  return vfsRoot->openWriter(sakusenPath);
}

