#ifndef LIBSAKUSEN__IUNIT_H
#define LIBSAKUSEN__IUNIT_H

#include "libsakusen-global.h"

#include <list>
#include "point.h"
#include "unittype.h"
#include "visibility.h"
#include "orientation.h"
#include "order.h"
#include "ordercondition.h"
#include "targettype.h"

namespace sakusen {

/** \brief An interface providing all proertied of a Unit
 *
 * This class is effectively an interface, giving accessors to all the data
 * that goes with a Unit.  It is used to provide a common ground between client
 * and server as to what a Unit is for communication between them.
 */
class LIBSAKUSEN_API IUnit {
  public:
    virtual ~IUnit() {}
  public:
    /* accessors */
    virtual UnitTypeID getType(void) const = 0;
    virtual uint32 getId(void) const = 0;
    virtual const Point<sint32>& getPosition(void) const = 0;
    virtual const Orientation& getOrientation(void) const = 0;
    virtual const Point<sint16>& getVelocity(void) const = 0;
    virtual HitPoints getHitPoints(void) const = 0;
    virtual bool isRadarActive(void) const = 0;
    virtual bool isSonarActive(void) const = 0;

    virtual const Order& getOrder(OrderCondition c) const = 0;
    virtual const Order& getCurrentOrder(void) const = 0;
    virtual LinearTargetType getLinearTarget(void) const = 0;
    virtual const Point<sint32>& getTargetPosition(void) const = 0;
    virtual const Point<sint16>& getTargetVelocity(void) const = 0;
    virtual RotationalTargetType getRotationalTarget(void) const = 0;
    virtual const Orientation& getTargetOrientation(void) const = 0;
    virtual const AngularVelocity& getTargetAngularVelocity(void) const = 0;
    
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

#endif // LIBSAKUSEN__IUNIT_H

