#ifndef REGIONDATA_H
#define REGIONDATA_H

#include "regiontype.h"
#include "point.h"
#include "rectangle.h"
#include "oarchive.h"

namespace sakusen {

class IUnitStatus;

/** \brief Abstract class representing some sort of data describing a region
 *
 * \note This class (and subclasses) should be immutable.  That is, all
 * methods should be const, and there should be no way to alter the region
 * once constructed.  With this, there should be no need to copy (the same
 * instance can instead be shared), so the newCopy method is commented out.
 */
template<typename T>
class LIBSAKUSEN_API RegionData {
  public:
    typedef boost::shared_ptr<RegionData> Ptr;
    typedef boost::shared_ptr<const RegionData> ConstPtr;
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
    //virtual RegionData* newCopy() const = 0;
    virtual void store(OArchive&) const = 0;
};

}

#endif // REGIONDATA_H

