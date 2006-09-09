#include "ballistic.h"
#include "world.h"

using namespace sakusen::server;

Ballistic::Ballistic(
      Time startTime,
      Point<sint32> startPosition,
      Point<sint32> startVelocity) :
  path(startPosition, startVelocity, startTime)
{
}

Ballistic::Ballistic(Quadratic p) :
  path(p)
{
}

