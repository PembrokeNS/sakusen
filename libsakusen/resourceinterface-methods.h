#ifndef RESOURCE_INTERFACE_METHODS_H
#define RESOURCE_INTERFACE_METHODS_H

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
inline T* ResourceInterface::search(
    const String& name,
    const typename T::loadArgument* arg,
    ResourceSearchResult* result
  ) {
  return static_cast<T*>(
      internalSearch(name, typenameToResourceType<T>(), arg, result)
    );
}

template<typename T>
inline bool ResourceInterface::save(const T* resource)
{
  return internalSave(resource, typenameToResourceType<T>());
}

}

#endif // RESOURCE_INTERFACE_METHODS_H

