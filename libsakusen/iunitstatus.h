#ifndef IUNITSTATUS_H
#define IUNITSTATUS_H

#include "libsakusen-global.h"

#include <list>
#include "point.h"
#include "unittype.h"
#include "visibility.h"
#include "orientation.h"

namespace sakusen {

class LIBSAKUSEN_API IUnitStatus {
  public:
    virtual ~IUnitStatus() {}

    virtual UnitTypeID getType(void) const = 0;
    virtual const Point<sint32>& getPosition(void) const = 0;
    virtual const Orientation& getOrientation(void) const = 0;
    virtual const Point<sint16>& getVelocity(void) const = 0;
    virtual HitPoints getHitPoints(void) const = 0;
    virtual bool isRadarActive(void) const = 0;
    virtual bool isSonarActive(void) const = 0;
    
    inline Point<sint32> localToGlobal(const Point<sint32>& p) const {
      return getOrientation()*p+getPosition();
    }
    
    inline Point<sint32> globalToLocal(const Point<sint32>& p) const {
      return getOrientation().inverseMul(p-getPosition());
    }
};

}

#endif // IUNITSTATUS_H

