#ifndef LIBSAKUSEN__ICOMPLETEUNIT_H
#define LIBSAKUSEN__ICOMPLETEUNIT_H

#include "libsakusen-global.h"

#include <list>
#include "point.h"
#include "unittype.h"
#include "iunitstatus.h"

namespace sakusen {

/** \brief An interface providing all proertied of a Unit
 *
 * This class is effectively an interface, giving accessors to all the data
 * that goes with a Unit.  It is used to provide a common ground between client
 * and server as to what a Unit is for communication between them.
 */
class LIBSAKUSEN_API ICompleteUnit {
  public:
    virtual ~ICompleteUnit() {}
  public:
    /* accessors */
    virtual uint32 getId(void) const = 0;
    virtual const IUnitStatus* getStatus(void) const = 0;
    virtual IUnitStatus* getStatus(void) = 0;
    
    virtual HitPoints getMaxHitPoints(void) const = 0;
    virtual uint8 getMass(void) const = 0;
    virtual const Point<uint32>& getSize(void) const = 0;
    virtual const Region<sint16>& getPossibleAccelerations(void) const = 0;
    virtual const Region<sint16>& getPossibleVelocities(void) const = 0;
    virtual const Region<sint16>& getPossibleAngularVelocities(void) const = 0;
    virtual const Visibility& getVisibility(void) const = 0;
    virtual const Sensors& getVision(void) const = 0;
};

}

#endif // LIBSAKUSEN__ICOMPLETEUNIT_H

