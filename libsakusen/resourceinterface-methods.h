#ifndef LIBSAKUSEN__RESOURCEINTERFACE_METHODS_H
#define LIBSAKUSEN__RESOURCEINTERFACE_METHODS_H

#include "resourceinterface.h"
#include "universe.h"
#include "map.h"

namespace sakusen {

template<>
inline ResourceType ResourceInterface::typenameToResourceType<Universe>()
{
  return resourceType_universe;
}

template<>
inline ResourceType ResourceInterface::typenameToResourceType<MapTemplate>()
{
  return resourceType_mapTemplate;
}

template<typename T>
inline boost::tuple<boost::shared_ptr<T>, ResourceSearchResult, String>
ResourceInterface::search(
    const String& name,
    Universe::ConstPtr universe
  ) {
    return search<T>(name, "", universe);
}

template<typename T>
inline boost::tuple<boost::shared_ptr<T>, ResourceSearchResult, String>
ResourceInterface::search(
    const String& name,
    const String& hash,
    Universe::ConstPtr universe
  ) {
  boost::shared_ptr<void> p;
  ResourceSearchResult result;
  String path;
  boost::tie(p, result, path) =
    internalSearch(name, hash, typenameToResourceType<T>(), universe);
  return boost::make_tuple(boost::static_pointer_cast<T>(p), result, path);
}

/** \brief Save a resource
 *
 * Saves a resource, automatically determining the appropriate place and name.
 *
 * \param resource Resource to save.
 * \param path Sakusen oath at which to save it.
 * \return true iff a problem occurs.
 */
template<typename T>
inline bool ResourceInterface::save(
    boost::shared_ptr<const T> const& resource,
    const String& path
  )
{
  return internalSave(resource, path, typenameToResourceType<T>());
}

}

#endif // LIBSAKUSEN__RESOURCEINTERFACE_METHODS_H

