#ifndef LIBSAKUSEN_RESOURCES__FILERESOURCEINTERFACE_H
#define LIBSAKUSEN_RESOURCES__FILERESOURCEINTERFACE_H

#include <sakusen/resources/global.h>

#include <vector>
#include <boost/weak_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include <sakusen/resourceinterface.h>
#include <sakusen/universe.h>
#include <sakusen/resources/vfs/branch.h>

namespace sakusen {
namespace resources {

/** \brief File-based ResourceInterface
 *
 * This ResourceInterface loads resources from disk.
 *
 * All constructors are private.  To get one, call a create method.  This aids
 * in establishing certain internal properties.
 */
class LIBSAKUSEN_RESOURCES_API FileResourceInterface :
  public ResourceInterface,
  private boost::noncopyable
{
  public:
    static Ptr create(
        const boost::filesystem::path& directory,
        bool loadModules
      );
    static Ptr create(
        const std::vector<boost::filesystem::path>& directories,
        bool loadModules
      );
  private:
    FileResourceInterface(
        const boost::filesystem::path& directory,
        bool loadModules
      );
    FileResourceInterface(
        const std::vector<boost::filesystem::path>& directories,
        bool loadModules
      );
  public:
    virtual ~FileResourceInterface() {}
  private:
    vfs::Branch::Ptr vfsRoot;
    bool loadModules;
    boost::weak_ptr<ResourceInterface> ptrToThis;

    String getExtension(ResourceType type) const;
  protected:
    virtual boost::tuple<boost::shared_ptr<void>, ResourceSearchResult, String>
    internalSearch(
        const String& sakusenPath,
        const String& hash,
        ResourceType type,
        Universe::ConstPtr
      );
    
    virtual boost::tuple<void*, ResourceSearchResult> internalSymbolSearch(
        const String& moduleName,
        const String& symbolName
      );

    virtual boost::tuple<Image::Ptr, String> imageSearch(const String& path);
    
    virtual bool internalSave(
        const boost::shared_ptr<const void>& resource,
        const String& path,
        ResourceType type
      );
  public:
    virtual Writer::Ptr openWriter(
        const String& sakusenPath
      );
};

}}

#endif // LIBSAKUSEN_RESOURCES__FILERESOURCEINTERFACE_H

