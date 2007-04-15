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

Box<sint32> ISensorReturns::getBoundingBox() const
{
  if (0 != (getPerception() & perception_unit)) {
    return getUnit()->getBoundingBox();
  } else if (0 != (getPerception() & perception_region)) {
    return getRegion()->getBoundingBox();
  } else {
    Fatal("No bounding box available");
  }
}

double ISensorReturns::intersect(const Ray& r) const
{
  if (0 != (getPerception() & perception_unit)) {
    return getUnit()->intersect(r);
  } else if (0 != (getPerception() & perception_region)) {
    return getRegion()->intersect(r);
  } else {
    Fatal("No intersection test available");
  }
}

bool ISensorReturns::contains(const Position& p) const
{
  if (0 != (getPerception() & perception_unit)) {
    return getUnit()->contains(p);
  } else if (0 != (getPerception() & perception_region)) {
    return getRegion()->contains(p);
  } else {
    Fatal("No containment test available");
  }
}

