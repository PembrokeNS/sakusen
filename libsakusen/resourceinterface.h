#ifndef RESOURCE_INTERFACE_H
#define RESOURCE_INTERFACE_H

#include "libsakusen-global.h"
#include "resourcesearchresult.h"
#include "resourcetype.h"

namespace sakusen {

/** \brief Abstract class providing an interface to a supply of resources
 *
 * This class is an abstract class which can be used to aid in serialization
 * and deserialization, so that knowledge about the underlying storage medium
 * is not needed.
 */
class LIBSAKUSEN_API ResourceInterface {
  protected:
    ResourceInterface() {}
    ResourceInterface(const ResourceInterface&) {}
  public:
    virtual ~ResourceInterface() {}
  protected:
    String error;
    
    virtual void* internalSearch(
        String name,
        ResourceType type,
        const void* arg,
        ResourceSearchResult* result
      ) = 0;

    virtual bool internalSave(const void* resource, ResourceType type) = 0;
    
    template<typename T>
    inline ResourceType typenameToResourceType();
  public:
    /** \brief Search resources for given name and type
     *
     * This method searches for a resource of the given name and type.  It puts
     * the result of the search at the place pointed to by the \a result
     * argument (unless \a result is \c NULL), and returns a pointer to
     * whatever was found (if anything, otherwise NULL).
     *
     * \a arg allows some other argument to be passed through to the
     * constructor or factory method for the resource (typically this will be a
     * Universe).
     */
    template<typename T>
    inline T* search(
        String name,
        const typename T::loadArgument* arg,
        ResourceSearchResult* result
      );

    template<typename T>
    inline bool save(const T* resource);

    /** \brief Returns description of last error
     *
     * When a search indicates a result of resourceSearchResult_error, a
     * description of the error can be obtained by calling this function.
     */
    inline String getError() const { return error; }
};

}

#endif // RESOURCE_INTERFACE_H

