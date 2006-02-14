#include "completemap.h"

#include "planemap.h"

using namespace std;

using namespace sakusen;
using namespace sakusen::server;

CompleteMap::CompleteMap(const MapTemplate& t) :
  Map(t),
  heightfield(t.getHeightfield())
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

