#ifndef PLANEMAP_H
#define PLANEMAP_H

#include "map.h"

namespace sakusen {

/** \brief Represents a planar map
 *
 * This class implements Map for a planar map (all map edges impassable).
 *
 * The template paramater MapType should be a class inheriting from Map, and
 * serves as an intermediate class between Map and PlaneMap.  Examples of
 * appropriate classes are sakusen::server::CompleteMap and
 * sakusen::client::PartialMap.
 */
template<typename MapType>
class LIBSAKUSEN_API PlaneMap : public MapType {
  private:
    PlaneMap();
  public:
    PlaneMap(const MapTemplate& t) : MapType(t) {}
    PlaneMap(
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity
      ) : MapType(topRight, bottomLeft, gravity) {}

    Topology getTopology(void) const { return topology_plane; }
    bool resolvePosition(
        const Point<sint32>& pos, Point<sint32>* resolved,
        Orientation* orientation
      ) const;
    Point<sint32> getShortestDifference(
        const Point<sint32>& op1, const Point<sint32>& op2
      ) const;
};

template<typename MapType>
bool PlaneMap<MapType>::resolvePosition(
    const Point<sint32>& pos, Point<sint32>* resolved,
    Orientation* /*orientation*/
  ) const
{
  Point <sint32> p = pos;
  
  /* Here's where we endeavour to actually implement our multitudinous array of
   * topologies.  However, this function is relatively trivial compared to
   * getShortestDifference below */
  bool truncated = false;
  
  if (p.x<this->left()) {
    p.x = this->left();
    truncated = true;
  }

  if (p.x>=this->right()) {
    p.x = this->right() - 1;
    truncated = true;
  }

  if (p.y<this->bottom()) {
    p.y = this->bottom();
    truncated = true;
  }

  if (p.y>=this->top()) {
    p.y = this->top() - 1;
    truncated = true;
  }

  if (resolved != NULL) {
    *resolved = p;
  }
  
  return truncated;
}

template<typename MapType>
Point<sint32> PlaneMap<MapType>::getShortestDifference(
    const Point<sint32>& op1, const Point<sint32>& op2
  ) const
{
  return op1-op2;
}

}

#endif // PLANEMAP_H

