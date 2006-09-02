#ifndef WEAPON_H
#define WEAPON_H

#include "libsakusen-global.h"
#include "point.h"
#include "weapontype.h"

namespace sakusen {
namespace server {

class LIBSAKUSEN_API Weapon {
  private:
    Weapon();
  protected:
    Weapon(const Weapon&);
    Weapon(const WeaponType* type);
  public:
    virtual ~Weapon() {}
  private:
    WeaponTypeID type;
    uint16 energyCharge;
    uint16 metalCharge;
    bool active; /* true for currently firing or charging, false for inactive */
  
  public:
    /* accessors */
    WeaponTypeID getType(void) const {return type;}
    uint16 getEnergy(void) const {return energyCharge;}
    uint16 getMetal(void) const {return metalCharge;}
    uint32 getCharge(void) const {return (energyCharge<<16)|metalCharge;}
    bool isActive(void) const {return active;}

    /* some functions to fire it go here */
    virtual void activate(void);
    virtual void deactivate(void);

    /* callbacks */
    
    /* provisional - probably should take some arguments */
    virtual void onFire(void) {}
    
    /* for when the unit is destroyed, if we want to have ammo exploding or
     * anything like that */
    virtual void onDestruct(const Point<sint32>&) {}

    /** Create a new copy of this Weapon which must be deleted by the caller */
    virtual Weapon* newCopy() const = 0;
};

}}

#endif
