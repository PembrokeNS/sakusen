#ifndef REGIONDATA_H
#define REGIONDATA_H

#include "regiontype.h"
#include "point.h"
#include "rectangle.h"
#include "oarchive.h"

namespace sakusen {

class IUnitStatus;

/** \brief Abstract class representing some sort of data describing a region */
template<typename T>
class LIBSAKUSEN_API RegionData {
  protected:
    RegionData() {}
    RegionData(const RegionData&) {}
  public:
    virtual ~RegionData() {}
  public:
    virtual bool contains(const Point<T>& point) const = 0;
    virtual Point<T> truncateToFit(const Point<T>&) const = 0;
    virtual Point<T> getBestPosition() const = 0;
    virtual Rectangle<T> getBoundingRectangle() const = 0;
    
    virtual RegionType getType() const = 0;
    virtual RegionData* newCopy() const = 0;
    virtual void store(OArchive&) const = 0;
};

}

#endif // REGIONDATA_H

