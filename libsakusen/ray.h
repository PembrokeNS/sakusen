#ifndef RAY_H
#define RAY_H

#include "libsakusen-global.h"
#include "unit.h"

class Ray {
  private:
  /* representation of location: x = s + d.t, 0<t<{1 iff finite, 0 o/w} */
  Point<sint32> s;
  Point<sint32> d;

  public:
  Ray();
  Ray(Point<sint32> start, Point<sint32> direction): s(start), d(direction) {}
  virtual ~Ray() {}
  double intersectUnit(Unit* u);
  double intersectUnits();
  double intersectLand();
  double intersectWater();
};

#endif

