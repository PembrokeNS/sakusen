#ifndef FILERESOURCEINTERFACE_H
#define FILERESOURCEINTERFACE_H

#include "libsakusen-resources-global.h"

#include <vector>

#include "resourceinterface.h"

namespace sakusen {
namespace resources {

class LIBSAKUSEN_RESOURCES_API FileResourceInterface : public ResourceInterface {
  private:
    FileResourceInterface();
  public:
    FileResourceInterface(const String& directory, bool loadModules);
    FileResourceInterface(
        const std::vector<String>& directories,
        bool loadModules
      );
    virtual ~FileResourceInterface() {}
  private:
    String saveDirectory;
    std::vector<String> directories;
    bool loadModules;

    String getSubdir(ResourceType type);
    String getExtension(ResourceType type);
  protected:
    String fileSearch(
        const String& name,
        ResourceType type,
        ResourceSearchResult* result
      );
    
    virtual void* internalSearch(
        const String& name,
        ResourceType type,
        const void* arg,
        ResourceSearchResult* result
      );
    
    virtual void* internalSymbolSearch(
        const String& moduleName,
        const String& symbolName,
        ResourceSearchResult* result
      );
    
    virtual bool internalSave(const void* resource, ResourceType type);
};

}}

#endif // FILERESOURCEINTERFACE_H

