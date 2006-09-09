#ifndef WEAPONSTATUS_H
#define WEAPONSTATUS_H

#include "iarchive.h"
#include "oarchive.h"
#include "weapontype.h"

namespace sakusen {

class WeaponStatus {
  public:
    WeaponStatus();
    WeaponStatus(uint16 energyCharge, uint16 metalCharge, bool active);
  private:
    uint16 energyCharge;
    uint16 metalCharge;
    bool active; /* true for currently firing or charging, false for inactive */
  public:
    /* accessors */
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

