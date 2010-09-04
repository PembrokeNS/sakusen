#ifndef LIBSAKUSEN__REGION_METHODS_H
#define LIBSAKUSEN__REGION_METHODS_H

#include "region.h"
#include "world.h"
#include "iunitstatus.h"
#include "sphereregion.h"
#include "rectangleregion.h"

namespace sakusen {

template<typename T>
inline bool Region<T>::contains(const IUnitStatus& unit) const
{
  return contains(unit.getPosition());
}

#ifdef LIBSAKUSEN_METHOD_DEFINITIONS

template<typename T>
void Region<T>::store(OArchive& archive) const
{
  archive.magicValue("R");
  archive.insertEnum(getType());
  storeData(archive);
}

template<typename T>
typename Region<T>::Ptr Region<T>::loadNew(IArchive& archive)
{
  archive.magicValue("R");
  RegionType type;
  typename Region<T>::Ptr region;
  archive.extractEnum(type);
  switch(type) {
    case regionType_sphere:
      region.reset(SphereRegion<T>::loadNew(archive));
      break;
    case regionType_rectangle:
      region.reset(RectangleRegion<T>::loadNew(archive));
      break;
    default:
      throw EnumDeserializationExn("type", type);
  }
  return region;
}

#endif


#ifdef _MSC_VER
template class LIBSAKUSEN_API Region<sint32>;
#endif

}

#endif // LIBSAKUSEN__REGION_METHODS_H

