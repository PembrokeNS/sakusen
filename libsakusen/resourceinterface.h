#ifndef RESOURCE_INTERFACE_H
#define RESOURCE_INTERFACE_H

#include "libsakusen-global.h"

#include <boost/shared_ptr.hpp>

#include "resourcesearchresult.h"
#include "resourcetype.h"
#include "writer.h"

namespace sakusen {

class Universe;

/** \brief Abstract class providing an interface to a supply of resources
 *
 * This class is an abstract class which can be used to aid in serialization
 * and deserialization, so that knowledge about the underlying storage medium
 * is not needed.
 */
class LIBSAKUSEN_API ResourceInterface {
  public:
    typedef boost::shared_ptr<ResourceInterface> Ptr;
  protected:
    ResourceInterface() {}
    ResourceInterface(const ResourceInterface&) {}
  public:
    virtual ~ResourceInterface() {}
  protected:
    String error;
    
    virtual boost::shared_ptr<void> internalSearch(
        const String& name,
        ResourceType type,
        ResourceSearchResult* result,
        boost::shared_ptr<const Universe> = boost::shared_ptr<const Universe>()
      ) = 0;
    
    virtual void* internalSymbolSearch(
        const String& moduleName,
        const String& symbolName,
        ResourceSearchResult* result
      ) = 0;

    virtual bool internalSave(
        const boost::shared_ptr<const void>& resource,
        ResourceType type
      ) = 0;
    
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
    inline boost::shared_ptr<T> search(
        const String& name,
        ResourceSearchResult* result,
        boost::shared_ptr<const Universe> = boost::shared_ptr<const Universe>()
      );

    /** \brief Search for a a symbol in a dlopenable module
     *
     * \param      moduleName Name (or prefix thereof) of module to search for
     * \param      symbolName Name of symbol to search for within the module
     * \param[out] result     Result of search returned through this value
     * \return The pointer to the symbol, cast to type T (probably a function
     *         pointer type); or NULL if there was an error.
     */
    template<typename T>
    T symbolSearch(
        const String& moduleName,
        const String& symbolName,
        ResourceSearchResult* result
      ) {
      return reinterpret_cast<T>(
          internalSymbolSearch(moduleName, symbolName, result)
        );
    }
    
    template<typename T>
    inline bool save(const boost::shared_ptr<const T>& resource);

    /** \brief Open a Writer appropriate to this ResourceInterface.
     *
     * \p name should be a simple name, as opposed to a full path.  If the
     * ResourceInterface is to files, then the appropriate directory will be
     * chosen according to \p type.
     */
    virtual Writer::Ptr openWriter(
        const String& name,
        ResourceType type
      ) = 0;

    /** \brief Returns description of last error
     *
     * When a search indicates a result of resourceSearchResult_error, a
     * description of the error can be obtained by calling this function.
     */
    inline String getError() const { return error; }
};

}

#endif // RESOURCE_INTERFACE_H

