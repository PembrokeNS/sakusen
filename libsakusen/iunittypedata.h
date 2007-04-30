#ifndef IUNITTYPEDATA_H
#define IUNITTYPEDATA_H

#include "point.h"
#include "visibility.h"
#include "hitpoints.h"
#include "region.h"

namespace sakusen {

class LIBSAKUSEN_API IUnitTypeData {
  public:
    virtual ~IUnitTypeData() {}
    virtual HitPoints getMaxHitPoints() const = 0;
    virtual uint8 getMass() const = 0;
    virtual const Point<uint32>& getSize() const = 0;
    virtual Region<sint16>::ConstPtr getPossibleAccelerations() const = 0;
    virtual Region<sint16>::ConstPtr getPossibleVelocities() const = 0;
    virtual Region<sint16>::ConstPtr getPossibleAngularVelocities() const = 0;
    virtual const Visibility& getVisibility() const = 0;
    virtual const Sensors& getVision() const = 0;
};

}

#endif // IUNITTYPEDATA_H

