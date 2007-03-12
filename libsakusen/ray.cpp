#include "ray.h"
#include "icompleteunit.h"
#include "heightfield.h"
#include "world.h"

using namespace std;

namespace sakusen {

/** \brief Get a point the Ray passes through.
 *
 * Evaluates the ray equation \em x = \em s + \em d . \em t for the given value
 * of t, and returns the Point the Ray is passing through at that point. Useful
 * for calculating the location in game space of a previously found
 * intersection.
 *
 * \param t The ray parameter, as returned by an intersection function. May be
 * -ve, in which case the result will be before the start of the ray.
 * \return A Point<sint32> in game-space.
 */
Point<sint32> Ray::evaluate(double t) const {
  return origin + Point<sint32>(Point<double>(d) * t);
}

/** \brief How far along the Ray am I?
 *
 * Given the parameter \p t for the ray equation, works out how far along the
 * Ray that is. (This is not always the same as \em t because \c d does not
 * have to be a unit vector.)
 *
 * Passing 1 gets you the length of \c d, which (for finite rays) is the length
 * of the ray.
 *
 * \param t The ray parameter, as returned by an intersection function. May be
 * -ve, in which case the result will also be -ve.
 * \return A distance in dex.
 */
double Ray::distance(double t) const {
  return t * length;
}

/** \brief Intersect Ray with water surface.
 *
 * \note If the ray starts underwater, then this returns the first point at
 * which it would leave the water, rather than 0.0.
 *
 * \return Smallest positive value d for which evaluate(d) is at the water
 * surface, or infinity if no such d exists. */
double Ray::intersectWater() const {
  /** \todo Support maps with water on them */
  return numeric_limits<double>::infinity();
}

/** \brief Find and return all interactions of this ray to a given extent.
 *
 * \param      extent       The length (in units of d) to scan along the ray.
 *                          If extent is infinity or NaN, then behaviour is
 *                          undefined.
 * \param[out] interactions The interactions found.
 */
void Ray::getAllInteractionsTo(
    double extent,
    IntersectionSet& interactions
  ) const
{
  GameObject interact = interactsWith();
  GameObject stop = stoppedBy();
  GameObject interesting = interact | stop;

  assert(interactions.empty());

  /* Test for intersection with the water surface */
  if (0 != (interesting & gameObject_water)) {
    double waterIntersect = intersectWater();
    if (waterIntersect <= extent) {
      if (0 != (stop & gameObject_water)) {
        extent = waterIntersect;
      }
      if (0 != (interact & gameObject_water)) {
        interactions.insert(Intersection(gameObject_water, waterIntersect));
      }
    }
  }

  /* Test for intersection with the ground */
  if (0 != (interesting & gameObject_land)) {
    double landIntersect =
      world->getMap()->getHeightfield().intersectRay(*this, extent);
    if (landIntersect <= extent) {
      if (0 != (stop & gameObject_land)) {
        extent = landIntersect;
      }
      if (0 != (interact & gameObject_land)) {
        interactions.insert(Intersection(gameObject_land, landIntersect));
      }
    }
  }
  
  /** \todo Intersections with units and Effects */
}

}
