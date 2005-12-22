#ifndef REGION_H
#define REGION_H

class Region;

#include "point.h"

class Unit;

class Region {
  public:
    Region();
    Region(const Region& copy, const Point<sint32> position);
    ~Region() {}
  private:
    /* TODO: Somehow store appropriate region info.
     * For the present a sphere (centre and radius) will probably suffice
     * In the long term we may want to make this an abstract class with various
     * subclasses, although that may cause some confusion.  
     */
    Point<sint32> centre;
    uint32 radius;
  public:
    /* accessors */
    inline uint64 squareRadius(void) const { return uint64(radius) * radius; }
    /* game mechanics */
    bool contains(const Point<sint32>& point) const;
    bool contains(const Unit& unit) const; /* Can't inline these due to cyclic
                                              include problems */
};

#endif // REGION_H

