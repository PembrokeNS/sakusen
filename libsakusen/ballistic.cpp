#include "ballistic.h"
#include "ballistictypeid.h"
#include "world.h"

Ballistic::Ballistic(
      BallisticTypeID t,
      Time startTime,
      Point<sint32> startPosition,
      Point<sint32> startVelocity) :
  type(t),
  path(startPosition, startVelocity, startTime)
{
}

Ballistic::Ballistic(BallisticTypeID t, Quadratic p) :
  type(t),
  path(p)
{
}

void Ballistic::spawn(
    const BallisticTypeID type,
    const Time startTime,
    const Point<sint32>& startPosition,
    const Point<sint16>& startVelocity)
{
  world->addBallistic(Ballistic(type, startTime, startPosition, startVelocity));
}

