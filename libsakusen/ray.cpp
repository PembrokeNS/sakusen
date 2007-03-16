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

double Ray::intersectBox(const Box<sint32>& box) const
{
  double t_near_x, t_far_x, t_near_y, t_far_y, t_near_z, t_far_z, t_near;
  const double inf = std::numeric_limits<double>::infinity();

  /* First off, take each pair of parallel planes, and find the interval of t
   * for which the ray is between the planes.
   */
  if (d.x != 0) {
    t_near_x = double(box.getMin().x - origin.x) / d.x;
    t_far_x = double(box.getMax().x - origin.x) / d.x;
    /* unless the ray is parallel to those planes, in which case check whether
     * it is between them
     */
  } else if (origin.x <= box.getMax().x && origin.x >= box.getMin().x) {
    /* the ray is between the planes for all t */
    t_near_x = -inf; t_far_x = inf;
  } else
    return inf;
  /* ^ the ray misses the box entirely */

  if (d.y != 0) {
    t_near_y = double(box.getMin().y - origin.y) / d.y;
    t_far_y = double(box.getMax().y - origin.y) / d.y;
  } else if (origin.y <= box.getMax().y && origin.y >= box.getMax().y) {
    t_near_y = -inf; t_far_y = inf;
  } else
    return inf;

  if (d.z != 0) {
    t_near_z = double(box.getMin().z - origin.z) / d.z;
    t_far_z = double(box.getMax().z - origin.z) / d.z;
  } else if (origin.z <= box.getMax().z && origin.z >= box.getMax().z) {
    t_near_z = -inf; t_far_z = inf;
  } else
    return inf;

  /* now we know the interval for each, check that all three overlap */
  if (t_near_x > t_far_y || t_near_y > t_far_z || t_near_z > t_far_x
      || t_near_x > t_far_z || t_near_y > t_far_x || t_near_z > t_far_y)
    return inf;

  /* The entry intersection is the farthest one, i.e. the point where we enter
   * the intersection of all three ray segments.
   */
  t_near = std::max(t_near_x, std::max(t_near_y, t_near_z));

  /* If the entry point is behind the origin, we must be inside the box, so use
   * the exit point.
   */
  if (t_near > 0.0)
    return t_near;
  else
    return std::min(t_far_x, std::min(t_far_y, t_far_z));
}

/** \brief Find and return all interactions of this ray to a given extent.
 *
 * \param      extent       The length (in units of d) to scan along the ray.
 *                          If extent is infinity or NaN, then behaviour is
 *                          undefined.
 * \param      interact     Bitfield of objects with which interactions should
 *                          be sought.
 * \param      stop         Bitfield of objects which stop the ray, and past
 *                          which we need not search.
 * \param[out] interactions The interactions found.
 */
void Ray::getAllInteractionsTo(
    double extent,
    GameObject interact,
    GameObject stop,
    IntersectionSet& interactions
  ) const
{
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
