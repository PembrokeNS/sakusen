#ifndef LIBSAKUSEN__ICOMPLETEUNIT_H
#define LIBSAKUSEN__ICOMPLETEUNIT_H

#include <sakusen/libsakusen-global.h>

#include <list>
#include <sakusen/point.h>
#include <sakusen/rectangle.h>
#include <sakusen/box.h>
#include <sakusen/unittype.h>
#include <sakusen/iunitstatus.h>
#include <sakusen/ref.h>
#include <sakusen/bounded.h>
#include <sakusen/unitid.h>

namespace sakusen {

/** \brief An interface providing all properties of a Unit
 *
 * This class is effectively an interface, giving accessors to all the data
 * that goes with a Unit.  It is used to provide a common ground between client
 * and server as to what a Unit is for communication between them.
 */
class LIBSAKUSEN_API ICompleteUnit : public Bounded {
  public:
    virtual ~ICompleteUnit() {}
  public:
    /* accessors */
    virtual UnitId getId(void) const = 0;
    virtual const IUnitStatus& getIStatus(void) const = 0;
    virtual const IUnitTypeData& getITypeData(void) const = 0;
    
    GameObject getObjectType() const { return gameObject_unit; }
    virtual Rectangle<sint32> getBoundingRectangle(void) const;
    virtual Box<sint32> getBoundingBox(void) const;
    boost::tuple<double,double> fastIntersect(const Ray& r) const {
      return intersect(r);
    }
    boost::tuple<double,double> intersect(const Ray& r) const;
    bool fastContains(const Position& p) const { return contains(p); }
    bool contains(const Position& p) const;
};

}

#endif // LIBSAKUSEN__ICOMPLETEUNIT_H

