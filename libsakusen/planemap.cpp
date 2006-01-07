#include "planemap.h"

PlaneMap::PlaneMap(
    const Universe* universe,
    const String& internalName,
    const Point<uint32>& volume,
    const Heightfield& heightfield,
    uint16 gravity,
    const std::vector<MapPlayMode>& playModes
  ) :
  Map(
      universe, internalName, volume/uint32(2), volume/uint32(2) - volume,
      heightfield, gravity, playModes
    )
{
}

PlaneMap::PlaneMap(
    const Universe* universe,
    const String& internalName,
    const Point<uint32>& topRight,
    const Point<uint32>& bottomLeft,
    const Heightfield& heightfield,
    uint16 gravity,
    const std::vector<MapPlayMode>& playModes
  ) :
  Map(
      universe, internalName, topRight, bottomLeft, heightfield,
      gravity, playModes
    )
{
}

bool PlaneMap::resolvePosition(
    const Point<sint32>& pos, Point<sint32>* resolved,
    Orientation* /*orientation*/
  ) const
{
  Point <sint32> p = pos;
  
  /* Here's where we endeavour to actually implement our multitudinous array of
   * topologies.  However, this function is relatively trivial compared to
   * getShortestDifference below */
  bool truncated = false;
  
  if (p.x<left()) {
    p.x = left();
    truncated = true;
  }

  if (p.x>=right()) {
    p.x = right() - 1;
    truncated = true;
  }

  if (p.y<bottom()) {
    p.y = bottom();
    truncated = true;
  }

  if (p.y>=top()) {
    p.y = top() - 1;
    truncated = true;
  }

  if (resolved != NULL) {
    *resolved = p;
  }
  
  return truncated;
}

Point<sint32> PlaneMap::getShortestDifference(
    const Point<sint32>& op1, const Point<sint32>& op2
  ) const
{
  return op1-op2;
}
