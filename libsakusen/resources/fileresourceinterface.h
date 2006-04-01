#ifndef FILERESOURCEINTERFACE_H
#define FILERESOURCEINTERFACE_H

#include "libsakusen-resources-global.h"

#include <list>

#include "resourceinterface.h"

namespace sakusen {
namespace resources {

class LIBSAKUSEN_RESOURCES_API FileResourceInterface : public ResourceInterface {
  private:
    FileResourceInterface();
  public:
    FileResourceInterface(const String& directory);
    FileResourceInterface(const std::list<String>& directories);
    virtual ~FileResourceInterface() {}
  private:
    String saveDirectory;
    std::list<String> directories;

    String getSubdir(ResourceType type);
    
  protected:
    virtual void* internalSearch(
        String name,
        ResourceType type,
        const void* arg,
        ResourceSearchResult* result
      );
    virtual bool internalSave(const void* resource, ResourceType type);
};

}}

#endif // FILERESOURCEINTERFACE_H

