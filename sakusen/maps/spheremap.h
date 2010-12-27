#ifndef LIBSAKUSEN__MAPS__SPHEREMAP_H
#define LIBSAKUSEN__MAPS__SPHEREMAP_H

#include <sakusen/map.h>

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
        SAKUSEN_FATAL("spherical topology incompatible with non-square map");
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
        SAKUSEN_FATAL("spherical topology incompatible with non-square map");
      }
    }
    
    virtual Topology getTopology() const { return topology_sphere; }

    virtual bool resolvePosition(
        const Point<sint32>& pos, Point<sint32>& p,
        Orientation* orientation
      ) const {
      p = pos;
      
      bool altered;
      sint32 newX;
      sint32 newY;
      
      do {
        altered = false;

        if (p.x<this->left()) {
          altered = true;
          newY = this->top() - (this->left() - p.x);
          p.x = this->left() + (this->top() - p.y - 1);
          p.y = newY;
          if (orientation != NULL) {
            *orientation =
              Orientation(rotation_anticlockwise, 90) * *orientation;
          }
        }

        if (p.x>=this->right()) {
          altered = true;
          newY = this->bottom() + (p.x - this->right());
          p.x = this->left() + (this->top() - p.y - 1);
          p.y = newY;
          if (orientation != NULL) {
            *orientation =
              Orientation(rotation_anticlockwise,90) * *orientation;
          }
        }

        if (p.y<this->bottom()) {
          altered = true;
          newX = this->right() - (this->bottom() - p.y);
          p.y = this->bottom() + (this->right() - p.x - 1);
          p.x = newX;
          if (orientation != NULL) {
            *orientation =
              Orientation(rotation_anticlockwise, 270) * *orientation;
          }
        }

        if (p.y>=this->top()) {
          altered = true;
          newX = this->left() + (p.y - this->bottom());
          p.y = this->bottom() + (this->right() - p.x - 1);
          p.x = newX;
          if (orientation != NULL) {
            *orientation =
              Orientation(rotation_anticlockwise, 270) * *orientation;
          }
        }
      } while (altered);

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

#endif // LIBSAKUSEN__MAPS__SPHEREMAP_H

