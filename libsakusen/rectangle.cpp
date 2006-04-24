#include "rectangle.h"
#include "icompleteunit.h"
#include "isensorreturns.h"

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
  if (contains(unit->getIStatus()->getPosition())) {
    return true;
  }
  return intersects(unit->getBoundingRectangle());
}

template<>
bool Rectangle<sint32>::fastIntersects(const ISensorReturns* returns) const
{
  if (contains(returns->getBestPosition())) {
    return true;
  }
  return intersects(returns->getBoundingRectangle());
}

/* Added to stop VC giving warnings about lack of suitable functions */
template<>
bool Rectangle<uint16>::fastIntersects(const ICompleteUnit*) const
{
  Fatal("this method should not be called");
  return true;
}

template<>
bool Rectangle<uint16>::fastIntersects(const ISensorReturns*) const
{
  Fatal("this method should not be called");
  return true;
}

template<>
bool Rectangle<double>::fastIntersects(const ICompleteUnit*) const
{
  Fatal("this method should not be called");
  return true;
}

template<>
bool Rectangle<double>::fastIntersects(const ISensorReturns*) const
{
  Fatal("this method should not be called");
  return true;
}

#ifdef _MSC_VER
template LIBSAKUSEN_API Rectangle<uint16>;
template LIBSAKUSEN_API Rectangle<sint32>;
template LIBSAKUSEN_API Rectangle<double>;
#endif

}

