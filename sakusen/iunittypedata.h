#ifndef LIBSAKUSEN__IUNITTYPEDATA_H
#define LIBSAKUSEN__IUNITTYPEDATA_H

#include <sakusen/point.h>
#include <sakusen/visibility.h>
#include <sakusen/hitpoints.h>
#include <sakusen/region.h>

namespace sakusen {

/** \brief Interface to all dynamic UnitType data.
 *
 * See \ref units for more details.
 */
class LIBSAKUSEN_API IUnitTypeData {
  public:
    virtual ~IUnitTypeData() {}
    virtual HitPoints getMaxHitPoints() const = 0;
    virtual uint8 getMass() const = 0;
    virtual const Point<uint32>& getSize() const = 0;
    virtual Region<sint16>::ConstPtr getPossibleAccelerations() const = 0;
    virtual Region<sint16>::ConstPtr getPossibleVelocities() const = 0;
    virtual Region<sint32>::ConstPtr getPossibleAngularVelocities() const = 0;
    virtual const Visibility& getVisibility() const = 0;
    virtual const Sensors& getVision() const = 0;
};

}

#endif // LIBSAKUSEN__IUNITTYPEDATA_H

