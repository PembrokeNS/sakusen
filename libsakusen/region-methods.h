#ifndef REGION_METHODS_H
#define REGION_METHODS_H

#include "region.h"
#include "world.h"
#include "iunitstatus.h"

namespace sakusen {

template<typename T>
inline bool Region<T>::contains(const Point<T>& point) const
{
  return
    world->getMap()->getShortestDifference(point, centre).squareLength() <
    squareRadius();
}

template<>
inline bool Region<sint32>::contains(const IUnitStatus* unit) const
{
  /* TODO: maybe we want to check whether it intersects the unit or completely
   * contains the unit, rather than simply whether it contains the centre, as
   * here */
  return contains(unit->getPosition());
}

template<>
inline Point<sint16> Region<sint16>::truncateToFit(const Point<sint16>& p) const
{
  if (contains(p)) {
    return p;
  }
  return (p * radius / p.length()).truncate16();
}

template<>
inline Point<sint32> Region<sint32>::truncateToFit(const Point<sint32>& p) const
{
  if (contains(p)) {
    return p;
  }
  return (p * radius / p.length()).truncate32();
}

template<typename T>
void Region<T>::store(OArchive& archive) const
{
  archive << centre << radius;
}

template<typename T>
Region<T> Region<T>::load(IArchive& archive)
{
  Point<T> centre;
  uint32 radius;
  archive >> centre >> radius;
  return Region<T>(centre, radius);
}

}

#endif // REGION_METHODS_H

