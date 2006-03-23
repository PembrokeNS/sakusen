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

#endif // IUNITTYPEDATA_H

