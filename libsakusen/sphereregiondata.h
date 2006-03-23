#ifndef SPHEREREGIONDATA_H
#define SPHEREREGIONDATA_H

#include "regiondata.h"
#include "world.h"
#include "libsakusen-global.h"

namespace sakusen {

/** \brief Describes a region which is spherical */
template<typename T>
class LIBSAKUSEN_API SphereRegionData : public RegionData<T> {
  public:
    SphereRegionData() : centre(), radius(0) {}
    SphereRegionData(const Point<T>& c, uint32 r) : centre(c), radius(r) {}
  private:
    Point<T> centre;
    uint32 radius;
  public:
    inline uint64 squareRadius() const { return uint64(radius) * radius; }
    inline bool contains(const Point<T>& point) const;
    inline Point<T> truncateToFit(const Point<T>&) const;
    
    RegionType getType() const { return regionType_sphere; }
    RegionData<T>* newCopy() const { return new SphereRegionData<T>(*this); }
    void store(OArchive& archive) const
    {
       archive << centre << radius;
    }
    static SphereRegionData<T>* loadNew(IArchive& archive)
   {
       Point<T> centre;
       uint32 radius;
       archive >> centre >> radius;
       return new SphereRegionData<T>(centre, radius);
   }
};

template<typename T>
inline bool SphereRegionData<T>::contains(const Point<T>& point) const {
    return world->getMap()->getShortestDifference(point, centre).
    squareLength() < squareRadius();
}

template<>
inline Point<sint16> SphereRegionData<sint16>::truncateToFit(
    const Point<sint16>& p
  ) const
{
  if (contains(p)) {
    return p;
  }
  return (p * radius / p.length()).truncate16();
}

template<>
inline Point<sint32> SphereRegionData<sint32>::truncateToFit(
    const Point<sint32>& p
  ) const
{
  if (contains(p)) {
    return p;
  }
  return (p * radius / p.length()).truncate32();
}

template LIBSAKUSEN_API SphereRegionData<sint16>;
}

#endif // SPHEREREGIONDATA_H

