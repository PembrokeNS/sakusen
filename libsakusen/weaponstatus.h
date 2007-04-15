#ifndef WEAPONSTATUS_H
#define WEAPONSTATUS_H

#include "iarchive.h"
#include "oarchive.h"
#include "weapontype.h"
#include "weapontargettype.h"
#include "ref.h"
#include "orientation.h"
#include "serializationhandler.h"

namespace sakusen {

class ICompleteUnit;

class LIBSAKUSEN_API WeaponStatus {
  public:
    WeaponStatus();
    WeaponStatus(
        WeaponTargetType targetType,
        const Point<sint16>& direction,
        uint16 energyCharge,
        uint16 metalCharge,
        bool active
      );
  private:
    WeaponTargetType targetType;
    Point<sint16> targetDirection;
    Position targetPosition;
    Orientation targetOrientation;
    /* I did have a targetUnit here for a while, which would have been nice,
     * but it can't be, because this object has to be serialised as part of a
     * SensorReturns which is in turn sent to a player other than the owner,
     * who can't deserialise anything about other units owned by the player
     * who owns this weapon.  Similar comments apply to putting a
     * targetSensorReturns here.  So, we'll just have to make do without them.
     * */
    uint16 energyCharge;
    uint16 metalCharge;
    bool active; /* true for currently firing or charging, false for inactive */
  public:
    /* accessors */
    WeaponTargetType getTargetType() const { return targetType; }
    const Point<sint16>& getTargetDirection() const { return targetDirection; }
    void setTargetDirection(const Point<sint16>& d);
    void setTarget(const Position&, const Orientation&);
    void setTarget(const Ref<ICompleteUnit>&);
    uint16 getEnergy(void) const {return energyCharge;}
    uint16 getMetal(void) const {return metalCharge;}
    uint32 getCharge(void) const {return (energyCharge<<16)|metalCharge;}
    bool isActive(void) const {return active;}

    void activate(void);
    void deactivate(void);

    bool incrementState(WeaponTypeID type);

    void store(OArchive&) const;
    static WeaponStatus load(IArchive&);
};

}

#endif // WEAPONSTATUS_H

