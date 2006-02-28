#ifndef RECTANGLE_METHODS_H
#define RECTANGLE_METHODS_H

#include "rectangle.h"
#include "icompleteunit.h"

namespace sakusen {

/** \brief Conducts a quick test as to whether the given unit intersects this
 * Rectangle.
 * 
 * This might return true when the unit is only near the Rectangle, rather
 * than actually intersecting it, but it won't return false when the unit
 * does in fact intersect. */
template<>
bool Rectangle<sint32>::fastIntersects(const ICompleteUnit* unit) const
{
  if (contains(unit->getStatus()->getPosition())) {
    return true;
  }
  return intersects(unit->getBoundingRectangle());
}

}

#endif // RECTANGLE_METHODS_H

