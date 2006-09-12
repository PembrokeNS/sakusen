#ifndef WEAPONSTATUS_H
#define WEAPONSTATUS_H

#include "iarchive.h"
#include "oarchive.h"
#include "weapontype.h"

namespace sakusen {

class WeaponStatus {
  public:
    WeaponStatus();
    WeaponStatus(
        const Point<sint16>& direction,
        uint16 energyCharge,
        uint16 metalCharge,
        bool active
      );
  private:
    Point<sint16> direction;
    uint16 energyCharge;
    uint16 metalCharge;
    bool active; /* true for currently firing or charging, false for inactive */
  public:
    /* accessors */
    const Point<sint16>& getDirection() const { return direction; }
    void setDirection(const Point<sint16>& d) { direction = d; }
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

