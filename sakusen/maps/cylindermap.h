#ifndef LIBSAKUSEN__MAPS__CYLINDERMAP_H
#define LIBSAKUSEN__MAPS__CYLINDERMAP_H

#include <sakusen/map.h>

namespace sakusen {
namespace maps {

/** \brief Represents a map on a cylider.
 */
template<typename MapType>
class CylinderMap : public MapType {
  public:
    CylinderMap(const MapTemplate& t) : MapType(t) {}
    CylinderMap(
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        const Heightfield& hf
      ) :
      MapType(
          topRight, bottomLeft, gravity, hf
        )
    {}

    virtual Topology getTopology() const { return topology_cylinder; }

    virtual bool resolvePosition(
        const Point<sint32>& pos, Point<sint32>& p,
        Orientation*
      ) const {
      p = pos;
      bool truncated = false;

      while (p.x < this->left()) {
        p.x += this->width();
      }

      while (p.x >= this->right()) {
        p.x -= this->width();
      }

      if (p.y < this->bottom()) {
        p.y = this->bottom();
        truncated = true;
      } else if (p.y >= this->top()) {
        p.y = this->top() - 1;
        truncated = true;
      }

      return truncated;
    }

    virtual Point<sint32> getShortestDifference(
        const Point<sint32>& op1, const Point<sint32>& op2
      ) const {
      /** \todo implement */
      SAKUSEN_FATAL("not implemented");
      return op1 - op2;
    }
};

}}

#endif // LIBSAKUSEN__MAPS__CYLINDERMAP_H

