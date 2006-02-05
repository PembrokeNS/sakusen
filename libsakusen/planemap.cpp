#include "planemap.h"

using namespace sakusen;

PlaneMap::PlaneMap(const MapTemplate& t) :
  Map(t)
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
