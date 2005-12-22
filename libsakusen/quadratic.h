#ifndef QUADRATIC_H
#define QUADRATIC_H

#include "libsakusen-global.h"
#include "point.h"

/* store a ballistic curve, x and y linear in T, z quadratic in T
 * x(T) = sx + dx.(T-t)
 * y(T) = sy + dy.(T-t)
 * z(T) = sz + dz.(T-t) - g.(T-t)^2
 */
class Quadratic {
  private:
  Point<sint32> s;
  Point<sint32> d;
  Time t;
  public:
  Quadratic(): s(0,0,0), d(0,0,0), t(0) {}
  Quadratic(Point<sint32> start, Point<sint32> velocity, Time startTime): s(start), d(velocity), t(startTime) {}
  ~Quadratic() {}
  Point<sint32> evaluate(Time tt);
};

#endif // QUADRATIC_H

