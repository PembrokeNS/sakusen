#ifndef REGION_H
#define REGION_H

#include "point.h"
#include "oarchive.h"
#include "iarchive.h"

namespace sakusen {

class IUnitStatus;

template<typename T>
class Region {
  public:
    Region() : centre(), radius(0) {}
    Region(Point<T> c, uint32 r) : centre(c), radius(r) {}
    ~Region() {}
  private:
    /* TODO: Somehow store appropriate region info.
     * For the present a sphere (centre and radius) will probably suffice
     * In the long term we may want to make this an abstract class with various
     * subclasses, although that may cause some confusion.  
     */
    Point<T> centre;
    uint32 radius;
  public:
    /* accessors */
    inline uint64 squareRadius(void) const { return uint64(radius) * radius; }
    /* game mechanics */
    inline bool contains(const Point<T>& point) const;
    inline bool contains(const IUnitStatus* unit) const;
    inline Point<T> truncateToFit(const Point<T>&) const;
    
    void store(OArchive&) const;
    static Region<T> load(IArchive&);
};

}

#endif // REGION_H

