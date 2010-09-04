#ifndef LIBSAKUSEN__MAPS__CROSSCUTMAP_H
#define LIBSAKUSEN__MAPS__CROSSCUTMAP_H

#include "map.h"

namespace sakusen {
namespace maps {

/** \brief Represents a map on a real projective plane, or crosscut.
 */
template<typename MapType>
class CrosscutMap : public MapType {
  public:
    CrosscutMap(const MapTemplate& t) : MapType(t) {}
    CrosscutMap(
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        const Heightfield& hf
      ) :
      MapType(
          topRight, bottomLeft, gravity, hf
        )
    {}
    
    virtual Topology getTopology() const { return topology_crosscut; }

    virtual bool resolvePosition(
        const Point<sint32>& pos, Point<sint32>& p,
        Orientation* orientation
      ) const {
      p = pos;
      
      while (p.x < this->left()) {
        p.x += this->width();
        p.y = this->reflectY(p.y);
        if (orientation != NULL) {
          *orientation = Orientation::reflectionY * *orientation;
        }
      }
      
      while (p.x >= this->right()) {
        p.x -= this->width();
        p.y = this->reflectY(p.y);
        if (orientation != NULL) {
          *orientation = Orientation::reflectionY * *orientation;
        }
      }
      
      while (p.y < this->bottom()) {
        p.y += this->height();
        p.x = this->reflectX(p.x);
        if (orientation != NULL) {
          *orientation = Orientation::reflectionX * *orientation;
        }
      }
      
      while (p.y >= this->top()) {
        p.y -= this->height();
        p.x = this->reflectX(p.x);
        if (orientation != NULL) {
          *orientation = Orientation::reflectionY * *orientation;
        }
      }

      return false;
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

#endif // LIBSAKUSEN__MAPS__CROSSCUTMAP_H

