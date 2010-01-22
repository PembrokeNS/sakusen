#ifndef LIBSAKUSEN__MAP_METHODS_H
#define LIBSAKUSEN__MAP_METHODS_H

#include "map.h"
#include "maps/planemap.h"
#include "maps/cylindermap.h"
#include "maps/torusmap.h"
#include "maps/mobiusmap.h"
#include "maps/kleinmap.h"
#include "maps/spheremap.h"
#include "maps/crosscutmap.h"

namespace sakusen {

/** \brief Constructs a new map based on the given template.
 *
 * The template parameter MapType is used as a base class for the map which is
 * created. */
template<typename MapType>
MapType* Map::newMap(const MapTemplate& t)
{
  /* To remind that update here is needed by causing a compile error when a
   * new enum value is added. */
  SAKUSEN_STATIC_ASSERT(topology_max == 7);
  
  switch (t.getTopology()) {
#define SAKUSEN_MAP_CASE(ltopology, utopology) \
    case topology_##ltopology:     \
      return new maps::utopology##Map<MapType>(t);
    SAKUSEN_MAP_CASE(plane, Plane)
    SAKUSEN_MAP_CASE(cylinder, Cylinder)
    SAKUSEN_MAP_CASE(torus, Torus)
    SAKUSEN_MAP_CASE(mobius, Mobius)
    SAKUSEN_MAP_CASE(klein, Klein)
    SAKUSEN_MAP_CASE(sphere, Sphere)
    SAKUSEN_MAP_CASE(crosscut, Crosscut)
    default:
      throw DeserializationExn("Invalid topology");
#undef SAKUSEN_MAP_CASE
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
#define SAKUSEN_MAP_CASE(ltopology, utopology)  \
    case topology_##ltopology:                  \
      return new maps::utopology##Map<MapType>( \
          topRight, bottomLeft, gravity, hf     \
        );
    SAKUSEN_MAP_CASE(plane, Plane)
    SAKUSEN_MAP_CASE(cylinder, Cylinder)
    SAKUSEN_MAP_CASE(torus, Torus)
    SAKUSEN_MAP_CASE(mobius, Mobius)
    SAKUSEN_MAP_CASE(klein, Klein)
    SAKUSEN_MAP_CASE(sphere, Sphere)
    SAKUSEN_MAP_CASE(crosscut, Crosscut)
    default:
      throw DeserializationExn("Invalid topology");
#undef SAKUSEN_MAP_CASE
  }
}

}

#endif // LIBSAKUSEN__MAP_METHODS_H
