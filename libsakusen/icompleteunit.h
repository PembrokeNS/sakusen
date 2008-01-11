#ifndef LIBSAKUSEN__ICOMPLETEUNIT_H
#define LIBSAKUSEN__ICOMPLETEUNIT_H

#include "libsakusen-global.h"

#include <list>
#include "point.h"
#include "rectangle.h"
#include "box.h"
#include "unittype.h"
#include "iunitstatus.h"
#include "ref.h"
#include "bounded.h"
#include "unitid.h"

namespace sakusen {

/** \brief An interface providing all proerties of a Unit
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
    double fastIntersect(const Ray& r) const { return intersect(r); }
    double intersect(const Ray& r) const;
    bool fastContains(const Position& p) const { return contains(p); }
    bool contains(const Position& p) const;
};

}

#endif // LIBSAKUSEN__ICOMPLETEUNIT_H

