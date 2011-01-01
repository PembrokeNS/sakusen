#ifndef LIBSAKUSEN__MAPS__TORUSMAP_H
#define LIBSAKUSEN__MAPS__TORUSMAP_H

#include <sakusen/map.h>

namespace sakusen {
namespace maps {

/** \brief Represents a toroidal map.
 */
template<typename MapType>
class TorusMap : public MapType {
  public:
    TorusMap(const MapTemplate& t) : MapType(t) {}
    TorusMap(
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        const Heightfield& hf
      ) :
      MapType(
          topRight, bottomLeft, gravity, hf
        )
    {}

    virtual Topology getTopology() const { return topology_torus; }

    virtual bool resolvePosition(
        const Point<sint32>& pos, Point<sint32>& p,
        Orientation*
      ) const {
      p = pos;

      while (p.x < this->left()) {
        p.x += this->width();
      }

      while (p.x >= this->right()) {
        p.x -= this->width();
      }

      while (p.y < this->bottom()) {
        p.y += this->height();
      }

      while (p.y >= this->top()) {
        p.y -= this->height();
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

#endif // LIBSAKUSEN__MAPS__TORUSMAP_H

