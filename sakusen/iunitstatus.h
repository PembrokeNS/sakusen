#ifndef LIBSAKUSEN__IUNITSTATUS_H
#define LIBSAKUSEN__IUNITSTATUS_H

#include <sakusen/libsakusen-global.h>

#include <list>
#include <sakusen/point.h>
#include <sakusen/unittype.h>
#include <sakusen/visibility.h>
#include <sakusen/frame.h>
#include <sakusen/weaponstatus.h>

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
    virtual const Velocity& getVelocity(void) const = 0;
    virtual const AngularVelocity& getAngularVelocity(void) const = 0;
    virtual HitPoints getHitPoints(void) const = 0;
    virtual bool isRadarActive(void) const = 0;
    virtual bool isSonarActive(void) const = 0;
    virtual const std::vector<WeaponStatus>& getWeaponsStatus(void) const = 0;
};

}

#endif // LIBSAKUSEN__IUNITSTATUS_H

