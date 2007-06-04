#ifndef RAY_H
#define RAY_H

#include "libsakusen-global.h"

#include <set>

#include "point.h"
#include "box.h"
#include "gameobject.h"
#include "intersection.h"

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
 *
 * Rays are immutable objects with all their fields public. This makes them
 * convenient to use as basic values, and thread-safe once the ctor has
 * finished.
 */
class Ray {
  public:
    const Point<sint32> origin; /**< origin (start) */
    const Point<sint32> d; /**< direction */
    const double length; /**< length of \c d */

    Ray(Point<sint32> start, Point<sint32> direction) :
      origin(start), d(direction), length(direction.length()) {}
    virtual ~Ray() {}
    Point<sint32> evaluate(double t) const;
    double distance(double t) const;
    double intersectWater() const;
    double intersectBox(const Box<sint32>&) const;

    /** \brief Typedef to aid those calling getAllInteractionsTo */
    typedef std::set<Intersection, LessThanIntersectionPosition>
      IntersectionSet;
    void getAllInteractionsTo(
        double extent,
        GameObject interactWith,
        GameObject stoppedBy,
        Ref<Bounded> cannotHit,
        IntersectionSet& interactions
      ) const;
};

}

#endif

