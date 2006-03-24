#include "completemap.h"

using namespace std;

using namespace sakusen;
using namespace sakusen::server;

CompleteMap::CompleteMap(const MapTemplate& t) :
  Map(t),
  heightfield(t.getHeightfield())
{
}

CompleteMap::CompleteMap(
    const Point<sint32>& topRight,
    const Point<sint32>& bottomLeft,
    uint16 gravity
  ) :
  Map(topRight, bottomLeft, gravity),
  heightfield()
{
}

Map* CompleteMap::newMap(const MapTemplate& t)
{
  switch (t.getTopology()) {
    case topology_plane:
      return new PlaneMap<CompleteMap>(t);
    /* TODO: All the other topologies */
    default:
      throw new DeserializationExn("Invalid topology");
  }
}

