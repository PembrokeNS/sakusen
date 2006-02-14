#ifndef MAP_METHODS_H
#define MAP_METHODS_H

#include "map.h"
#include "planemap.h"

namespace sakusen {

template<typename MapType>
MapType* Map::newMap(const MapTemplate& t)
{
  switch (t.getTopology()) {
    case topology_plane:
      return new PlaneMap<MapType>(t);
    /* TODO: All the other topologies */
    default:
      throw new DeserializationExn("Invalid topology");
  }
}

template<typename MapType>
MapType* Map::newMap(
    Topology topology,
    const Point<sint32>& topRight,
    const Point<sint32>& bottomLeft,
    uint16 gravity
  )
{
  switch (topology) {
    case topology_plane:
      return new PlaneMap<MapType>(topRight, bottomLeft, gravity);
    /* TODO: All the other topologies */
    default:
      throw new DeserializationExn("Invalid topology");
  }
}

}

#endif // MAP_METHODS_H
