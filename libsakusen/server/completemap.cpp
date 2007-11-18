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

/** \brief Do not call.
 *
 * This ctor only exists for complicated reasons
 * related to the way Map types inherit from one
 * another and the Window DLL-import/-export stuff.
 * It will go away in the future.
 *
 * \todo Remove or fix this ctor.
 */
CompleteMap::CompleteMap(
    const Point<sint32>& topRight,
    const Point<sint32>& bottomLeft,
    uint16 gravity,
    uint32 xyHeightfieldRes,
    uint32 zHeightfieldRes
    ) :
  Map(topRight, bottomLeft, gravity),
  heightfield(xyHeightfieldRes, zHeightfieldRes, 1, 1)
{
  Fatal("this method should not be called");
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
  if (0 != width() % heightfield.getXYResolution() ||
      0 != height() % heightfield.getXYResolution()) {
    Fatal("heightfield resolution not exact factor of map dimensions");
  }
  if (heightfield.getWidth() !=
      1+width()/heightfield.getXYResolution()) {
    Fatal("heightfield width incorrect");
  }
  if (heightfield.getHeight() !=
      1+height()/heightfield.getXYResolution()) {
    Fatal("heightfield height incorrect");
  }
}

Map* CompleteMap::newMap(const MapTemplate& t)
{
  switch (t.getTopology()) {
    case topology_plane:
      return new PlaneMap<CompleteMap>(t);
    /** \todo All the other topologies. */
    default:
      throw DeserializationExn("Invalid topology");
  }
}

