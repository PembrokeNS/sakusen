#ifndef LIBSAKUSEN__MAPS__SPHEREMAP_H
#define LIBSAKUSEN__MAPS__SPHEREMAP_H

#include "map.h"

namespace sakusen {
namespace maps {

/** \brief Represents a spherical map.
 */
template<typename MapType>
class SphereMap : public MapType {
  public:
    SphereMap(const MapTemplate& t) : MapType(t)
    {
      if (this->height() != this->width()) {
        Fatal("spherical topology incompatible with non-square map");
      }
    }
    SphereMap(
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        const Heightfield& hf
      ) :
      MapType(
          topRight, bottomLeft, gravity, hf
        )
    {
      if (this->height() != this->width()) {
        Fatal("spherical topology incompatible with non-square map");
      }
    }
    
    virtual Topology getTopology() const { return topology_sphere; }

    virtual bool resolvePosition(
        const Point<sint32>& /*pos*/, Point<sint32>& /*resolved*/,
        Orientation* /*orientation*/
      ) const {
      /** \todo implement */
      Fatal("not implemented");
    }

    virtual Point<sint32> getShortestDifference(
        const Point<sint32>& op1, const Point<sint32>& op2
      ) const {
      /** \todo implement */
      Fatal("not implemented");
      return op1 - op2;
    }
};

}}

#endif // LIBSAKUSEN__MAPS__SPHEREMAP_H

