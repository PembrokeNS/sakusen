#include "weapon.h"

#include <ltdl.h>

using namespace sakusen::server;

Weapon::Weapon(const Weapon& copy) :
  type(copy.type),
  energyCharge(copy.energyCharge),
  metalCharge(copy.metalCharge),
  active(copy.active)
{
}

Weapon::Weapon(const WeaponType* t) :
  type(t->getId()),
  energyCharge(0),
  metalCharge(0),
  active(false)
{
}

void Weapon::activate(void) {active = true;}
void Weapon::deactivate(void) {active = false;}

