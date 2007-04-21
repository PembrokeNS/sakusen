#ifndef WEAPON_H
#define WEAPON_H

#include "libsakusen-global.h"
#include "point.h"
#include "weapontype.h"
#include "layeredunit.h"

namespace sakusen {
namespace server {

class LIBSAKUSEN_SERVER_API Weapon {
  private:
    Weapon();
  protected:
    Weapon(const Weapon&);
    Weapon(const WeaponType* type);
  public:
    virtual ~Weapon() {}
  private:
    WeaponTypeID type;

    /* We here remember the targets for the weapon for those targets which
     * cannot be remembered by WeaponStatus because of serialization issues */
    Ref<LayeredUnit> targetUnit;
  protected:
    void setTarget(const Ref<LayeredUnit>& target, WeaponStatus* status) {
      targetUnit = target;
      status->setTarget(target.cast<ICompleteUnit>());
    }
    Ref<LayeredUnit> getTargetUnit() { return targetUnit; }

    /* callbacks */
    virtual bool aim(
        const Ref<LayeredUnit>& firer,
        WeaponStatus* status,
        const WeaponOrders&
      );
    /** \brief Allow subclasses to tell this class the speed of the fired
     * projectile.
     *
     * If you override aimAt, then this method is irrelevant.
     */
    virtual uint32 getProjectileSpeed() const = 0;
    virtual void onFire(const Ref<LayeredUnit>& firer, uint16 weaponIndex) = 0;
  public:
    /* accessors */
    WeaponTypeID getType(void) const {return type;}

    virtual void incrementState(
        const Ref<LayeredUnit>& firer,
        uint16 weaponIndex
      );
    
    /* for when the unit is destroyed, if we want to have ammo exploding or
     * anything like that */
    virtual void onDestruct(const Point<sint32>&) {}

    /** Create a new copy of this Weapon which must be deleted by the caller */
    virtual Weapon* newCopy() const = 0;
};

}}

#endif // WEAPON_H

