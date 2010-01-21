#include "isensorreturns.h"

#include "icompleteunit.h"
#include "region-methods.h"

using namespace sakusen;

/** \brief Get the best guess of the position of the unit from which this is a
 * return
 *
 * If there is no position information in the returns at all (which probably
 * shouldn't happen), then Fatals.
 */
Point<sint32> ISensorReturns::getBestPosition() const
{
  if (0 != (getPerception() & perception_unit)) {
    return getUnit()->getIStatus().getPosition();
  } else if (0 != (getPerception() & perception_region)) {
    return getRegion()->getBestPosition();
  } else {
    SAKUSEN_FATAL("No best position available (perception=" << getPerception() << ")");
  }
}

/** \brief Get the best guess of a bounding rectangle of the unit from which
 * this is a return
 *
 * Note that if the sensor return only has approximate location information
 * then the rectangle returned bounds the confidence region.  It may not bound
 * the unit itself.
 *
 * If there is no position information in the returns at all (which probably
 * shouldn't happen), then Fatals.
 */
Rectangle<sint32> ISensorReturns::getBoundingRectangle() const
{
  if (0 != (getPerception() & perception_unit)) {
    return getUnit()->getBoundingRectangle();
  } else if (0 != (getPerception() & perception_region)) {
    return getRegion()->getBoundingRectangle();
  } else {
    SAKUSEN_FATAL("No bounding rectangle available");
  }
}

Box<sint32> ISensorReturns::getBoundingBox() const
{
  if (0 != (getPerception() & perception_unit)) {
    return getUnit()->getBoundingBox();
  } else if (0 != (getPerception() & perception_region)) {
    return getRegion()->getBoundingBox();
  } else {
    SAKUSEN_FATAL("No bounding box available");
  }
}

boost::tuple<double,double> ISensorReturns::intersect(const Ray& r) const
{
  if (0 != (getPerception() & perception_unit)) {
    return getUnit()->intersect(r);
  } else if (0 != (getPerception() & perception_region)) {
    return getRegion()->intersect(r);
  } else {
    SAKUSEN_FATAL("No intersection test available");
  }
}

bool ISensorReturns::contains(const Position& p) const
{
  if (0 != (getPerception() & perception_unit)) {
    return getUnit()->contains(p);
  } else if (0 != (getPerception() & perception_region)) {
    return getRegion()->contains(p);
  } else {
    SAKUSEN_FATAL("No containment test available");
  }
}

