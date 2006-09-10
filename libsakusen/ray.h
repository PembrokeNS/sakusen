#ifndef RAY_H
#define RAY_H

#include "libsakusen-global.h"

#include "point.h"

namespace sakusen {

class ICompleteUnit;

/** \brief A virtual ray in game-space.
 *
 * Represents a ray in game-space, but not a physical object. The Ray can be
 * used to intersect with game-world objects, but doesn't interact in other
 * ways with game-world objects.
 *
 * It is intended for use for collision-avoidance, targeting, visibility, and
 * such things, and it forms the basis of Beam, which represents a physical
 * object that has the geometry of a ray but affects other objects in its path.
 *
 * The ray itself is the locus of \em s + \em d.t, where \c s is the origin of
 * the ray, \c d is its direction, and \em t is the 'ray parameter'. For an
 * infinite ray, \em t varies from 0 to infinity (though -ve values are
 * sometimes meaningful); for a finite ray, \em t varies from 0 to 1
 * (open-ended) by convention. Finite rays are not explicitly handled by Ray,
 * and callers must filter out intersections whose parameter is greater than 1.
 */
class Ray {
  private:
    Point<sint32> s; /**< origin (start) */
    Point<sint32> d; /**< direction */
    double length; /**< length of \c d */

  public:
    Ray() {}
    Ray(Point<sint32> start, Point<sint32> direction): s(start), d(direction), length(direction.length()) {}
    virtual ~Ray() {}
    Point<sint32> evaluate(double t) const;
    double distance(double t) const;
    double intersectUnit(ICompleteUnit* u) const;
    double intersectUnits() const;
    double intersectLand() const;
    double intersectWater() const;
};

}

#endif

