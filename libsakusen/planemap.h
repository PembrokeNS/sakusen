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
#if (!defined (_MSC_VER))|| (defined (LIBSAKUSEN_EXPORTS))
/*There is a reason for this slightly bizarre line. That reason is 
that for MSVC's linking to work properly, this piece of code should 
really be in a separate file. But I can't be bothered to move it there,
and update all the links, etc. 
The problem is that, when exporting, these pieces of code need to be 
there so they can be compiled. When importing, these pieces of code 
cannot be made dllimport, because they are definitions, not declarations.
But the top level class is dllimport, so these get made it as well. 
Hence, I make them commented out when the dll is importing. */

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
#endif //Odd exporty thing workaround

}

#endif // PLANEMAP_H

