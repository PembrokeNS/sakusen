#include "weapon.h"

#include "layeredunit.h"

using namespace sakusen::server;

Weapon::Weapon(const Weapon& copy) :
  type(copy.type)
{
}

Weapon::Weapon(const WeaponType* t) :
  type(t->getId())
{
}

void Weapon::incrementState(LayeredUnit* firer, uint16 weaponIndex)
{
  UnitStatus* unitStatus = firer->getStatus();
  assert(weaponIndex < unitStatus->getWeaponsStatus().size());
  WeaponStatus* status = &unitStatus->getWeaponsStatus()[weaponIndex];

  /** \todo Worry about what state weapons can be left in when inactive.  Can
   * they be charged, for example? */
  if (!status->isActive()) {
    return;
  }
  
  if (status->incrementState(type)) {
    /* Return value of true means that there were enough resources to fire */
    onFire(firer, weaponIndex);
  }
}

