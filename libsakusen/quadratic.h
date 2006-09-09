#ifndef QUADRATIC_H
#define QUADRATIC_H

#include "libsakusen-global.h"
#include "point.h"

namespace sakusen {

/** \brief A virtual quadratic curve.
 *
 * Just as the Ray class represents a ray in game-space that doesn't interact
 * with objects, this class represents a quadratic in game-space. It is
 * intended to store the path of a ballistic projectile, so it is linear in x
 * and y, and the z co-ordinate is acted on by gravity.
 *
 * The Ballistic class extends this one to represent an actual object in the
 * game-world that has Quadratic geometry but interacts with other objects.
 *
 * The equation for the quadratic is:
 *
 * x(T) = sx + dx.(T-t)
 * y(T) = sy + dy.(T-t)
 * z(T) = sz + dz.(T-t) - g.(T-t)^2
 *
 * where \c s is the origin (or start point), \c d is the initial velocity,
 * \c t is the time of firing (the time at which the curve passes through the
 * origin), and \em g is the acceleration due to gravity (depends on map).
 */
class Quadratic {
  private:
  Point<sint32> s; /**< origin */
  Point<sint32> d; /**< direction */
  Time t; /**< time origin */
  public:
  Quadratic(): s(0,0,0), d(0,0,0), t(0) {}
  Quadratic(Point<sint32> start, Point<sint32> velocity, Time startTime): s(start), d(velocity), t(startTime) {}
  ~Quadratic() {}
  Point<sint32> evaluate(Time tt);
};

}

#endif // QUADRATIC_H

