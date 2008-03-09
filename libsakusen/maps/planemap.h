#ifndef LIBSAKUSEN__MAPS__PLANEMAP_H
#define LIBSAKUSEN__MAPS__PLANEMAP_H

#include "map.h"

namespace sakusen {
namespace maps {

/** \brief Represents a planar map
 *
 * This class implements Map for a planar map (all map edges impassable).
 *
 * The template paramater MapType should be a class inheriting from Map, and
 * serves as an intermediate class between Map and PlaneMap.  Examples of
 * appropriate classes are sakusen::server::CompleteMap and
 * sakusen::client::PartialMap.
 *
 * \warning Windows programmers note: This class should <b>not</b> have a
 * LIBSAKUSEN_API attribute.  The import/export status must be determined at
 * instantiation time (as is done in libsakusen-client and -server)
 */
template<typename MapType>
class PlaneMap : public MapType {
  public:
    PlaneMap(const MapTemplate& t) : MapType(t) {}
    PlaneMap(
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        const Heightfield& hf
      ) :
      MapType(
          topRight, bottomLeft, gravity, hf
        )
    {}

    Topology getTopology(void) const { return topology_plane; }
    bool resolvePosition(
        const Point<sint32>& pos, Point<sint32>& p,
        Orientation*
      ) const
    {
      p = pos;
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

      return truncated;
    }

    inline Point<sint32> getShortestDifference(
        const Point<sint32>& op1, const Point<sint32>& op2
      ) const
    {
      return op1-op2;
    }
};

}}

#endif // LIBSAKUSEN__MAPS__PLANEMAP_H

