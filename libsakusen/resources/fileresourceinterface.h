#ifndef FILERESOURCEINTERFACE_H
#define FILERESOURCEINTERFACE_H

#include "libsakusen-resources-global.h"

#include <vector>
#include <boost/weak_ptr.hpp>

#include "resourceinterface.h"
#include "universe.h"

namespace sakusen {
namespace resources {

/** \brief File-based ResourceInterface
 *
 * This ResourceInterface loads resources from disk.
 *
 * All constructors are private.  To get one, call a create method.  This aids
 * in establishing certain internal properties.
 */
class LIBSAKUSEN_RESOURCES_API FileResourceInterface : public ResourceInterface
{
  public:
    static Ptr create(const String& directory, bool loadModules);
    static Ptr create(
        const std::vector<String>& directories,
        bool loadModules
      );
  private:
    FileResourceInterface(const String& directory, bool loadModules);
    FileResourceInterface(
        const std::vector<String>& directories,
        bool loadModules
      );
  public:
    virtual ~FileResourceInterface() {}
  private:
    String saveDirectory;
    std::vector<String> directories;
    bool loadModules;
    boost::weak_ptr<ResourceInterface> ptrToThis;

    String getSubdir(ResourceType type);
    String getExtension(ResourceType type);
  protected:
    String fileSearch(
        const String& name,
        ResourceType type,
        ResourceSearchResult* result
      );
    
    virtual boost::shared_ptr<void> internalSearch(
        const String& name,
        ResourceType type,
        ResourceSearchResult* result,
        Universe::ConstPtr
      );
    
    virtual void* internalSymbolSearch(
        const String& moduleName,
        const String& symbolName,
        ResourceSearchResult* result
      );
    
    virtual bool internalSave(
        const boost::shared_ptr<const void>& resource,
        ResourceType type
      );
  public:
    virtual Writer::Ptr openWriter(
        const String& name,
        ResourceType
      );
};

}}

#endif // FILERESOURCEINTERFACE_H

