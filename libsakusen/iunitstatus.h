#ifndef IUNITSTATUS_H
#define IUNITSTATUS_H

#include "libsakusen-global.h"

#include <list>
#include "point.h"
#include "unittype.h"
#include "visibility.h"
#include "frame.h"
#include "weaponstatus.h"

namespace sakusen {

/** \brief Interface to all unit status information.
 *
 * See \ref units for more details.
 */
class LIBSAKUSEN_API IUnitStatus {
  public:
    virtual ~IUnitStatus() {}

    virtual UnitTypeId getType(void) const = 0;
    inline const Point<sint32>& getPosition(void) const {
      return getFrame().getPosition();
    }
    virtual const Frame& getFrame(void) const = 0;
    virtual const Point<sint16>& getVelocity(void) const = 0;
    virtual HitPoints getHitPoints(void) const = 0;
    virtual bool isRadarActive(void) const = 0;
    virtual bool isSonarActive(void) const = 0;
    virtual const std::vector<WeaponStatus>& getWeaponsStatus(void) const = 0;
};

}

#endif // IUNITSTATUS_H

