#include <sakusen/quadratic.h>
#include <sakusen/world.h>

using namespace sakusen;

Point<sint32> Quadratic::evaluate(Time tt) const
{
  if (tt < t) {
    SAKUSEN_DEBUG("evaluating quadratic before its creation");
  }
  /** \todo Possibly change this so that when it discovers that it has moved off
   * a map edge it automatically recomputes the quadratic parameters to be the
   * quadratic that appears on the corresponding map edge */
  Point<sint32> resolved;
  Point<sint32> p(s.x+d.x*(tt-t), s.y+d.y*(tt-t),
        s.z+d.z*(tt-t) - world->getMap()->getGravity()*(tt-t)*(tt-t));
  if (world->getMap()->resolvePosition(p, resolved)) {
    /** \todo deal with the fact that the point computed is off the map (note
     * that this means completely off the map, not just wrapped onto some other
     * part of it). */
  }
  return resolved;
}

void Quadratic::store(OArchive& archive) const
{
  archive << s << d << t;
}

Quadratic Quadratic::load(IArchive& archive)
{
  Point<sint32> s;
  Point<sint32> d;
  Time t;

  archive >> s >> d >> t;
  return Quadratic(s, d, t);
}

