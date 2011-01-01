#include <sakusen/server/ballistic.h>
#include <sakusen/server/completeworld.h>

namespace sakusen {
namespace server {

Ballistic::Ballistic(
    const Ref<LayeredUnit>& s,
    Time startTime,
    Point<sint32> startPosition,
    Point<sint32> startVelocity) :
  owner(s->getOwner()),
  source(s),
  path(startPosition, startVelocity, startTime)
{
}

Ballistic::Ballistic(const Ref<LayeredUnit>& s, Quadratic p) :
  owner(s->getOwner()),
  source(s),
  path(p)
{
}

/** \brief Check for collisions of this Ballistic over the last tick
 *
 * \return true iff the Ballistic should be removed */
bool Ballistic::resolveIntersections()
{
  Time timeNow = world->getTimeNow();
  /* If this is the first tick of existence for the Ballistic, then don't check
   * anything (otherwise we'd be checking along a path backwards from the
   * start, and also we might underflow the Time if this is the zeroth tick) */
  if (timeNow == path.getStartTime()) {
    return false;
  }
  const Position oldPos = getPosition(timeNow-1);
  const Position newPos = getPosition(timeNow);

  const Ray path(oldPos, newPos-oldPos);
  const GameObject canHit =
    gameObject_unit | gameObject_land | gameObject_water;

  Ray::IntersectionSet collisions;
  path.getAllInteractionsTo(1.0, canHit, canHit, source, collisions);

  if (!collisions.empty()) {
    onCollision(path.evaluate(collisions.begin()->getPosition()));
    return true;
  }

  return false;
}

}}//close namespaces

