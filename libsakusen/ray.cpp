#include "ray.h"
#include "icompleteunit.h"
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
  return s + Point<sint32>(d * t);
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

/** These functions are not written yet. */
/*@{*/
double Ray::intersectUnit(ICompleteUnit* u) const {
  static_cast<void>(u);
  Fatal("not implemented");
  return 0.0;
}
double Ray::intersectUnits() const {Fatal("not implemented"); return 0.0;}
double Ray::intersectLand() const {Fatal("not implemented"); return 0.0;}
double Ray::intersectWater() const {Fatal("not implemented"); return 0.0;}
/*@}*/
}
