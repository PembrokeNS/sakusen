#ifndef LIBSAKUSEN__MAPS__MOBIUSMAP_H
#define LIBSAKUSEN__MAPS__MOBIUSMAP_H

#include "map.h"

namespace sakusen {
namespace maps {

/** \brief Represents a map on a Mobius strip.
 */
template<typename MapType>
class MobiusMap : public MapType {
  public:
    MobiusMap(const MapTemplate& t) : MapType(t) {}
    MobiusMap(
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        const Heightfield& hf
      ) :
      MapType(
          topRight, bottomLeft, gravity, hf
        )
    {}
    
    virtual Topology getTopology() const { return topology_mobius; }

    virtual bool resolvePosition(
        const Point<sint32>& pos, Point<sint32>& p,
        Orientation* orientation
      ) const {
      p = pos;
      bool truncated = false;
      
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
      /* Compute three conceivable differences and pick out the shortest */
      const Point<sint32> diff1 = op1 - op2;
      const Point<sint32> diff2 =
        Point<sint32>(op1.x-this->width(), this->reflectY(op1.y), op1.z) - op2;
      Point<sint32> diff3 = diff2;
      diff3.x += 2*this->width();

      if (diff1.squareLength() <= diff2.squareLength() &&
          diff1.squareLength() <= diff3.squareLength()) {
        return diff1;
      }
      if (diff2.squareLength() <= diff3.squareLength()) {
        return diff2;
      }
      return diff3;
    }
};

}}

#endif // LIBSAKUSEN__MAPS__MOBIUSMAP_H

