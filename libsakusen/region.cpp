#include "region.h"
#include "unit.h"
#include "world.h"

using namespace sakusen;

Region::Region() :
  centre(),
  radius(0)
{
}

Region::Region(const Region& copy, const Point<sint32> position) :
  centre(position),
  radius(copy.radius)
{
}

bool Region::contains(const Point<sint32>& point) const
{
  return
    world->getMap()->getShortestDifference(point, centre).squareLength() <
    squareRadius();
}

bool Region::contains(const Unit& unit) const
{
  /* TODO: maybe we want to check whether it intersects the unit or completely
   * contains the unit, rather than simply whether it contains the centre, as
   * here */
  return contains(unit.getPosition());
}

