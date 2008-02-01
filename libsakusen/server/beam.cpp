#include "beam.h"
#include "world.h"
#include "serializationhandler.h"

using namespace std;
using namespace sakusen;

using namespace sakusen::server;

Beam::Beam(
    const Point<sint32>& start,
    const Point<sint32>& direction,
    const Ref<LayeredUnit>& s,
    Time creation,
    TimeSpan duration
  ) :
  Ray(start, direction),
  source(s),
  startTime(creation),
  endTime(creation+duration)
{
}

/** The destructor currently does nothing. */
Beam::~Beam()
{
}

/** \brief Callback for a general interaction of the Beam.  Should not
 * typically need to be overridden by subclasses */
void Beam::onInteract(const Intersection& in)
{
  switch (in.getType())
  {
    case gameObject_land:
      onInteractLand(in.getPosition());
      break;
    case gameObject_water:
      onInteractWater(in.getPosition());
      break;
    case gameObject_unit:
      onInteractUnit(in.getPosition(), in.getRef<LayeredUnit>(),in.isLeaving());
      break;
    case gameObject_effect:
      onInteractEffect(in.getPosition(), in.getRef<Effect>(), in.isLeaving());
      break;
    default:
      Fatal("unexpected interaction type: "<<in.getType());
  }
}

/** \brief Tests whether the Beam should be removed
 *
 * \return true iff the Beam should be removed
 *
 * The default implementation will return true if either the endTime has been
 * reached of the source unit has been destroyed.
 *
 * If we want to provide for Beams from other sources (e.g. laser-head shells),
 * then we should either scrap the second test (and the \c source member along
 * with it) or replace it with a more general test. */
bool Beam::onRemovalTest()
{
  return world->getTimeNow() >= endTime || !source.isValid();
}

/** \brief cause the Ray to be intersected with objects
 *
 * This method is probably called every tick by the world. It takes no
 * arguments, finds objects that cross the Beam's path, and does the
 * appropriate actions on those objects. It handles the penetration of the
 * Beam (i.e. what things it can go through).
 *
 * Note that this method is *not* virtual, so subclasses of Beam can only
 * change their behaviour within certain bounds.
 */
void Beam::resolveIntersections(void) {
  /** This is going to be a busy function. We need to check the flags on type,
   * see what we need to intersect with, and get Ray to intersect all three of
   * them for us. Then we see what things stop us, and discard (or stop testing
   * for) intersections behind those things. Then we call the appropriate hit
   * action on each object we hit.
   */

  /* Call the Ray method to find all pertinent intersections */
  IntersectionSet interactions;
  getAllInteractionsTo(1.0, interactsWith(), stoppedBy(), source, interactions);

  /* iterate over all these intersections, calling the callbacks as we go */
  for (IntersectionSet::iterator in = interactions.begin();
      in != interactions.end(); ++in) {
    onInteract(*in);
  }
}

