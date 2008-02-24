#ifndef WEAPONSTATUS_H
#define WEAPONSTATUS_H

#include "iarchive.h"
#include "oarchive.h"
#include "weapontype.h"
#include "weapontargettype.h"
#include "ref.h"
#include "frame.h"
#include "serializationhandler.h"
#include "materielprovider.h"

namespace sakusen {

class ICompleteUnit;

/** \brief This class describes the state of a Weapon in a form suitable to be
 * sent to a client.
 *
 * This is the primary feedback the client has about the
 * actions of its units' weapons.  Because a client does not have access to the
 * module which implements a Weapon, this is how a client can tell that its
 * Weapons are actually doing anything.
 *
 * \note The information here will also be visible to players other than the
 * owner when they recieve a SensorReturns from the unit with perception_unit.
 */
class LIBSAKUSEN_API WeaponStatus {
  public:
    WeaponStatus();
    WeaponStatus(
        WeaponTargetType targetType,
        const Point<sint32>& direction,
        uint16 energyCharge,
        uint16 metalCharge,
        bool active
      );
  private:
    WeaponTargetType targetType;
    Point<sint32> targetDirection;
    Position targetPosition;
    Frame targetFrame;
    /* There was a targetUnit here for a while, which would have been nice,
     * but it can't be, because this object has to be serialised as part of a
     * SensorReturns which is in turn sent to a player other than the owner,
     * who can't deserialise anything about other units owned by the player
     * who owns this weapon.  Similar comments apply to putting a
     * targetSensorReturns here.  So, we'll just have to make do without them.
     * */
    uint16 energyCharge;
    uint16 metalCharge;
    bool active; /* true for currently firing or charging, false otherwise */
  public:
    /** \name Accessors */
    //@{
    WeaponTargetType getTargetType() const { return targetType; }
    const Point<sint32>& getTargetDirection() const { return targetDirection; }
    const Position& getTargetPosition() const { return targetPosition; }
    const Frame& getTargetFrame() const {return targetFrame; }
    void setTargetDirection(const Point<sint32>& d);
    void setTarget(const Frame&);
    void setTarget(const Ref<ICompleteUnit>&);
    uint16 getEnergy(void) const {return energyCharge;}
    uint16 getMetal(void) const {return metalCharge;}
    uint32 getCharge(void) const {return (energyCharge<<16)|metalCharge;}
    bool isActive(void) const {return active;}

    void activate(void);
    void deactivate(void);
    //@}

    bool incrementState(WeaponTypeId type, MaterielProvider& provider);

    void store(OArchive&) const;
    static WeaponStatus load(IArchive&);
};

}

#endif // WEAPONSTATUS_H

