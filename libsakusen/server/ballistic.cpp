#include "ballistic.h"
#include "completeworld.h"

namespace sakusen {
  namespace server {

Ballistic::Ballistic(
    PlayerID o,
    Time startTime,
    Point<sint32> startPosition,
    Point<sint32> startVelocity) :
  owner(o), path(startPosition, startVelocity, startTime)
{
}

Ballistic::Ballistic(PlayerID o, Quadratic p) :
  owner(o),
  path(p)
{
}

/** \brief Check for collisions of this Ballistic over the last tick
 *
 * \return true iff the Ballistic should be removed */
bool Ballistic::resolveIntersections()
{
  const Position oldPos = getPosition(world->getTimeNow()-1);
  const Position newPos = getPosition(world->getTimeNow());

  const Ray path(oldPos, newPos-oldPos);
  const GameObject canHit =
    gameObject_unit | gameObject_land | gameObject_water;

  Ray::IntersectionSet collisions;
  path.getAllInteractionsTo(1.0, canHit, canHit, collisions);

  if (!collisions.empty()) {
    onCollision(path.evaluate(collisions.begin()->getPosition()));
    return true;
  }
  
  return false;
}

}}//close namespaces

