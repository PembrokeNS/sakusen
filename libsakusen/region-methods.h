#ifndef REGION_METHODS_H
#define REGION_METHODS_H

#include "region.h"
#include "world.h"
#include "iunitstatus.h"
#include "sphereregiondata.h"
#include "rectangleregiondata.h"

namespace sakusen {

template<typename T>
inline bool Region<T>::contains(const Point<T>& point) const
{
  return data->contains(point);
}

template<>
inline bool Region<sint32>::contains(const IUnitStatus* unit) const
{
  /* TODO: maybe we want to check whether it intersects the unit or completely
   * contains the unit, rather than simply whether it contains the centre, as
   * here */
  return contains(unit->getPosition());
}

template<typename T>
inline Point<T> Region<T>::truncateToFit(const Point<T>& p) const
{
  return data->truncateToFit(p);
}

template<typename T>
void Region<T>::store(OArchive& archive) const
{
  archive.magicValue<1>("R");
  archive << uint8(data->getType());
  data->store(archive);
}

template<typename T>
Region<T> Region<T>::load(IArchive& archive)
{
  archive.magicValue<1>("R");
  RegionType type;
  RegionData<T>* data = NULL;
  archive.extractEnum(type);
  switch(type) {
    case regionType_sphere:
      data = SphereRegionData<T>::loadNew(archive);
      break;
    case regionType_rectangle:
      data = RectangleRegionData<T>::loadNew(archive);
      break;
    default:
      throw new DeserializationExn(
          "unexpected RegionType: " + numToString(type)
        );
  }
  return Region(data);
}


}

#endif // REGION_METHODS_H

