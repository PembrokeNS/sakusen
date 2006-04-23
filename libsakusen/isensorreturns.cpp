#include "isensorreturns.h"

#include "icompleteunit.h"
#include "region-methods.h"

using namespace sakusen;

Point<sint32> ISensorReturns::getBestPosition() const
{
  if (0 != (getPerception() & perception_unit)) {
    return getUnit()->getIStatus()->getPosition();
  } else if (0 != (getPerception() & perception_region)) {
    return getRegion()->getBestPosition();
  } else {
    Fatal("No best position available (perception=" << getPerception() << ")");
  }
}

Rectangle<sint32> ISensorReturns::getBoundingRectangle() const
{
  if (0 != (getPerception() & perception_unit)) {
    return getUnit()->getBoundingRectangle();
  } else if (0 != (getPerception() & perception_region)) {
    return getRegion()->getBoundingRectangle();
  } else {
    Fatal("No bounding rectangle available");
  }
}

