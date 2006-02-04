#ifndef FILERESOURCEINTERFACE_H
#define FILERESOURCEINTERFACE_H

#include <list>

#include "resourceinterface.h"

namespace sakusen {
namespace resources {

class FileResourceInterface : public ResourceInterface {
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
    
    /** \brief Returns all files in the directory matching the given name
     *
     * \warning This method is not thread-safe!
     */
    std::list<String> findMatches(
        const String& directory,
        const String& name
      ) const;
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

