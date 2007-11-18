#ifndef MAP_METHODS_H
#define MAP_METHODS_H

#include "map.h"
#include "planemap.h"

namespace sakusen {

/** \brief Constructs a new map based on the given template.
 *
 * The template parameter MapType is used as a base class for the map which is
 * created. */
template<typename MapType>
MapType* Map::newMap(const MapTemplate& t)
{
  switch (t.getTopology()) {
    case topology_plane:
      return new PlaneMap<MapType>(t);
    /** \todo All the other topologies */
    default:
      throw DeserializationExn("Invalid topology");
  }
}

/** \brief Constructs a new map based on the given values.
 *
 * The template parameter MapType is used as a base class for the map which is
 * created. */
template<typename MapType>
MapType* Map::newMap(
    Topology topology,
    const Point<sint32>& topRight,
    const Point<sint32>& bottomLeft,
    uint16 gravity,
    const Heightfield& hf
  )
{
  switch (topology) {
    case topology_plane:
      return new PlaneMap<MapType>(
          topRight, bottomLeft, gravity, hf
        );
    /** \todo All the other topologies */
    default:
      throw DeserializationExn("Invalid topology");
  }
}

}

#endif // MAP_METHODS_H
