#ifndef WEAPON_H
#define WEAPON_H

#include "libsakusen-global.h"
#include "point.h"
#include "weapontype.h"
#include "layeredunit.h"

namespace sakusen {
namespace server {

/** \brief Abstract base class for weapons
 *
 * The Weapon class provides the way in to module code.  The key methods are
 * aim, which has a default implementation suitable for a Weapon that fires a
 * Ballistic, but can be overriden to aim other sorts of Weapon; and onFire,
 * which is called when the Weapon actually fires.  The firing of Weapons is
 * managed by Weapon::incrementState, which calls both the aforementioned
 * functions.
 */
class LIBSAKUSEN_SERVER_API Weapon : private boost::noncopyable {
  private:
    Weapon();
  protected:
    Weapon(const WeaponType* type);
  public:
    virtual ~Weapon() {}
  private:
    WeaponTypeId type;

    /* We here remember the targets for the weapon for those targets which
     * cannot be remembered by WeaponStatus because of serialization issues */
    Ref<LayeredUnit> targetUnit;
  protected:
    /** \brief Set the Weapon's target to a unit.
     *
     * This method can be used by subclasses of weapon when they want to record
     * that the weapon is acting on a unit.  It is analagous to the setTarget
     * methods of WeaponStatus, but is here because of serialization issues. */
    void setTarget(const Ref<LayeredUnit>& target, WeaponStatus& status) {
      targetUnit = target;
      status.setTarget(target.cast<ICompleteUnit>());
    }
    Ref<LayeredUnit> getTargetUnit() { return targetUnit; }

    /* callbacks */
    virtual bool aim(
        const Ref<LayeredUnit>& firer,
        WeaponStatus& status,
        const WeaponOrders&
      );
    /** \brief Allow subclasses to tell this class the speed of the fired
     * projectile.
     *
     * If you override aim, then this method is irrelevant.
     */
    virtual uint32 getProjectileSpeed() const = 0;

    /** \brief Callback called when the Weapon fires
     *
     * \param firer The unit firing
     * \param status The status of the Weapon
     * \param orders The orders of the Weapon
     * \param weaponIndex The index of the Weapon being fired
     *
     * A override of this method should consult the appropriate WeaponStatus
     * and perform the appropriate action for this Weapon.  It is permissible
     * to update the WeaponOrders if appropriate. For example, if the action of
     * firing the Weapon creates a building, then it will presumably be
     * imposible to create another in the same spot, so it is reasonable to
     * clear the WeaponOrders.
     */
    virtual void onFire(
        const Ref<LayeredUnit>& firer,
        const WeaponStatus& status,
        WeaponOrders& orders,
        uint16 weaponIndex
      ) = 0;
  public:
    /* accessors */
    WeaponTypeId getType(void) const {return type;}

    virtual void incrementState(
        const Ref<LayeredUnit>& firer,
        uint16 weaponIndex
      );
    
    /** \brief Callback called when a unit carrying this Weapon dies
     *
     * This might be used if a Weapon wants to have ammo exploding or
     * anything like that */
    virtual void onDestruct(const Point<sint32>&) {}
};

}}

#endif // WEAPON_H

