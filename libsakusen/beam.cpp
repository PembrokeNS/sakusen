#include "beam.h"
#include "world.h"

using namespace sakusen;

void Beam::spawn(
    BeamTypeID type,
    Time creationTime,
    Point<sint32> position,
    Point<sint32> direction)
{
  world->addBeam(Beam(type, creationTime, position, direction));
}

void Beam::resolveIntersections(void) {
  /** This is going to be a busy function. We need to check the flags on type,
   * see what we need to intersect with, and get Ray to intersect all three of
   * them for us. Then we see what things stop us, and discard (or stop testing
   * for) intersections behind those things. Then we call the appropriate hit
   * action on each object we hit.
   */
}
