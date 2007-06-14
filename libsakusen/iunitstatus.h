#ifndef IUNITSTATUS_H
#define IUNITSTATUS_H

#include "libsakusen-global.h"

#include <list>
#include "point.h"
#include "unittype.h"
#include "visibility.h"
#include "orientation.h"
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
    virtual const Point<sint32>& getPosition(void) const = 0;
    virtual const Orientation& getOrientation(void) const = 0;
    virtual const Point<sint16>& getVelocity(void) const = 0;
    virtual HitPoints getHitPoints(void) const = 0;
    virtual bool isRadarActive(void) const = 0;
    virtual bool isSonarActive(void) const = 0;
    virtual const std::vector<WeaponStatus>& getWeaponsStatus(void) const = 0;
    
    /** \brief Converts an position in local to absolute global coordinates.
     *
     * Local coordinates are centred on the unit and have the unit aligned with
     * the axes in the "normal" way.
     * 
     * Absolute global coordinates are the usual coordinate system.
     */
    inline Point<sint32> localToGlobal(const Point<sint32>& p) const {
      return getOrientation()*p+getPosition();
    }
    
    /** \brief Converts an position in local to relative global coordinates.
     *
     * Local coordinates are centred on the unit and have the unit aligned with
     * the axes in the "normal" way.
     * 
     * Relative global coordinates are aligned with the usual coordinates, but
     * with their origin at the centre of the unit.
     */
    inline Point<sint32> localToGlobalRelative(const Point<sint32>& p) const {
      return getOrientation()*p;
    }
    
    /** \brief Converts an position in absolute global to local coordinates.
     *
     * Local coordinates are centred on the unit and have the unit aligned with
     * the axes in the "normal" way.
     * 
     * Absolute global coordinates are the usual coordinate system.
     */
    inline Point<sint32> globalToLocal(const Point<sint32>& p) const {
      return getOrientation().inverseMul(p-getPosition());
    }
    
    /** \brief Converts an position in relative global to local coordinates.
     *
     * Local coordinates are centred on the unit and have the unit aligned with
     * the axes in the "normal" way.
     * 
     * Relative global coordinates are aligned with the usual coordinates, but
     * with their origin at the centre of the unit.
     */
    inline Point<sint32> globalToLocalRelative(const Point<sint32>& p) const {
      return getOrientation().inverseMul(p);
    }
};

}

#endif // IUNITSTATUS_H

