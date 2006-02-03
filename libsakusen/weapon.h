#ifndef WEAPON_H
#define WEAPON_H

#include "libsakusen-global.h"
#include "point.h"
#include "weapontype.h"

namespace sakusen {

class LIBSAKUSEN_API Weapon {
  private:
    WeaponTypeID type;
    uint16 energyCharge;
    uint16 metalCharge;
    bool active; /* true for currently firing or charging, false for inactive */
  
  public:
    Weapon();
    Weapon(const WeaponType* t);
    virtual ~Weapon() {}

    PLACEHOLDER_OPERATORS(Weapon)

    /* accessors */
    WeaponTypeID getType(void) {return type;}
    uint16 getEnergy(void) {return energyCharge;}
    uint16 getMetal(void) {return metalCharge;}
    uint32 getCharge(void) {return (energyCharge<<16)|metalCharge;}
    bool isActive(void) {return active;}

    /* some functions to fire it go here */
    virtual void activate(void);
    virtual void deactivate(void);

    /* callbacks */
    virtual void onFire(void) {} /* provisional - probably should take some arguments */
    virtual void onDestruct(Point <sint32>) {} /* for when the unit is destroyed, if we want to have ammo exploding or anything like that */
};

#ifdef NEED_TEMPLATE_INSTANTIATION
#include <list>
#pragma warning (disable: 4231)
EXPORT_LIST(Weapon)
#endif

}

#endif
