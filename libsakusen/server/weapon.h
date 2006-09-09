#ifndef WEAPON_H
#define WEAPON_H

#include "libsakusen-global.h"
#include "point.h"
#include "weapontype.h"
#include "layeredunit.h"

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
  
  public:
    /* accessors */
    WeaponTypeID getType(void) const {return type;}

    /* callbacks */
    
    virtual void incrementState(LayeredUnit* firer, uint16 weaponIndex);
    virtual void onFire(LayeredUnit* firer, uint16 weaponIndex) = 0;
    
    /* for when the unit is destroyed, if we want to have ammo exploding or
     * anything like that */
    virtual void onDestruct(const Point<sint32>&) {}

    /** Create a new copy of this Weapon which must be deleted by the caller */
    virtual Weapon* newCopy() const = 0;
};

}}

#endif
