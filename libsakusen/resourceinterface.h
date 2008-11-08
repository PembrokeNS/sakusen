#ifndef RESOURCE_INTERFACE_H
#define RESOURCE_INTERFACE_H

#include "libsakusen-global.h"

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include "resourcesearchresult.h"
#include "resourcetype.h"
#include "writer.h"
#include "image.h"

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
    
    virtual boost::tuple<boost::shared_ptr<void>, ResourceSearchResult, String>
    internalSearch(
        const String& name,
        const String& hash,
        ResourceType type,
        boost::shared_ptr<const Universe> = boost::shared_ptr<const Universe>()
      ) = 0;
    
    virtual boost::tuple<void*, ResourceSearchResult> internalSymbolSearch(
        const String& moduleName,
        const String& symbolName
      ) = 0;

    virtual bool internalSave(
        const boost::shared_ptr<const void>& resource,
        const String& path,
        ResourceType type
      ) = 0;
    
    template<typename T>
    inline ResourceType typenameToResourceType();
  public:
    /** \brief Search for resource of given name and type T
     *
     * This method searches for a resource of the given name type T.  It
     * returns a shared_ptr to the resource, if found, a ResourceSearchResult
     * value indicating the result of the search, and the path in the sakusen
     * VFS where the resource was found.
     *
     * The last optional argument allows a universe to be passed through to
     * the search if one exists and might be useful.  This is necessary, for
     * example, when loading a MapTemplate.
     */
    template<typename T>
    inline boost::tuple<boost::shared_ptr<T>, ResourceSearchResult, String>
    search(
        const String& name,
        boost::shared_ptr<const Universe> = boost::shared_ptr<const Universe>()
      );

    /** \brief Search for resources of given name and type T, insisting on an
     * exact hash.
     *
     * As the other overload of search, but also specifying a hash.  If the
     * hash does not match, the result will be an error.
     */
    template<typename T>
    inline boost::tuple<boost::shared_ptr<T>, ResourceSearchResult, String>
    search(
        const String& name,
        const String& hash,
        boost::shared_ptr<const Universe> = boost::shared_ptr<const Universe>()
      );

    /** \brief Search for a a symbol in a dlopenable module
     *
     * \param      moduleName Name (or prefix thereof) of module to search for
     * \param      symbolName Name of symbol to search for within the module
     * \return The pointer to the symbol, cast to type T (probably a function
     *         pointer type); or NULL if there was an error.  Also a
     *         ResourceSearchResult indicating the result.
     */
    template<typename T>
    boost::tuple<T, ResourceSearchResult> symbolSearch(
        const String& moduleName,
        const String& symbolName
      ) {
      void* p;
      ResourceSearchResult result;
      boost::tie(p, result) = internalSymbolSearch(moduleName, symbolName);
      return boost::make_tuple(reinterpret_cast<T>(p), result);
    }

    /** \brief Open image at given sakusen path.
     *
     * \return If found, a pointer to an image and the exact sakusen path
     * where it was found. */
    virtual boost::tuple<Image::Ptr, String>
    imageSearch(const String& path) = 0;

    template<typename T>
    inline bool save(
        const boost::shared_ptr<const T>& resource,
        const String& path
      );

    template<typename T>
    inline bool save(
        const boost::shared_ptr<T>& resource,
        const String& path
      ) {
      return save(boost::shared_ptr<const T>(resource), path);
    }

    /** \brief Open a Writer appropriate to this ResourceInterface.
     *
     * \p path is a full sakusen path to the desired destination.  If the writer
     * cannot be created for some reason, then a NULL pointer is returned.
     */
    virtual Writer::Ptr openWriter(
        const String& path
      ) = 0;

    /** \brief Returns description of last error.
     *
     * When a search indicates a result of resourceSearchResult_error, a
     * description of the error can be obtained by calling this function.
     */
    inline String getError() const { return error; }
};

}

#endif // RESOURCE_INTERFACE_H

