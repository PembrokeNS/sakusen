#include "completemap.h"

using namespace std;

using namespace sakusen;
using namespace sakusen::server;

CompleteMap::CompleteMap(const MapTemplate& t) :
  Map(t),
  heightfield(t.getHeightfield())
{
  sanityCheck();
}

CompleteMap::CompleteMap(
    const Point<sint32>& topRight,
    const Point<sint32>& bottomLeft,
    uint16 gravity,
    const Heightfield& h
  ) :
  Map(topRight, bottomLeft, gravity),
  heightfield(h)
{
  sanityCheck();
}

void CompleteMap::sanityCheck()
{
  if (0 != width() % heightfield.getHorizontalResolution() ||
      0 != height() % heightfield.getHorizontalResolution()) {
    Fatal("heightfield resolution not eact factor of map dimensions");
  }
  if (heightfield.getWidth() !=
      1+width()/heightfield.getHorizontalResolution()) {
    Fatal("heightfield width incorrect");
  }
  if (heightfield.getHeight() !=
      1+height()/heightfield.getHorizontalResolution()) {
    Fatal("heightfield height incorrect");
  }
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

