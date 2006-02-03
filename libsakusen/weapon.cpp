#include "weapon.h"

using namespace sakusen;

Weapon::Weapon(const WeaponType* t) :
  type(t->getId()),
  energyCharge(0),
  metalCharge(0),
  active(false)
{
}

void Weapon::activate(void) {active = true;}
void Weapon::deactivate(void) {active = false;}

