#ifndef IUNITSTATUS_H
#define IUNITSTATUS_H

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

class IUnitStatus {
  public:
    virtual ~IUnitStatus() {}

    virtual UnitTypeID getType(void) const = 0;
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
};

}

#endif // IUNITSTATUS_H

