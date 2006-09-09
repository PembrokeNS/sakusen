#include "weaponstatus.h"

#include "world.h"

using namespace sakusen;

WeaponStatus::WeaponStatus() :
  energyCharge(0),
  metalCharge(0),
  active(false)
{
}

WeaponStatus::WeaponStatus(uint16 eC, uint16 mC, bool a) :
  energyCharge(eC), metalCharge(mC), active(a)
{
}

void WeaponStatus::activate(void) {active = true;}
void WeaponStatus::deactivate(void) {active = false;}

bool WeaponStatus::incrementState(WeaponTypeID type)
{
  const WeaponType* typePtr = world->getUniverse()->getWeaponTypePtr(type);

  /** \bug The resources should not appear from thin air */
  energyCharge += typePtr->getEnergyRate();
  metalCharge += typePtr->getMetalRate();

  bool fired = false;

  if (energyCharge >= typePtr->getEnergyCost() &&
      metalCharge >= typePtr->getMetalCost()) {
    fired = true;
    energyCharge -=typePtr->getEnergyCost();
    metalCharge -=typePtr->getMetalCost();
  }

  /** \todo Do we need a separate cap for charge?  For now we use the cost. */
  if (energyCharge > typePtr->getEnergyCost()) {
    energyCharge = typePtr->getEnergyCost();
  }
  if (metalCharge > typePtr->getMetalCost()) {
    metalCharge = typePtr->getMetalCost();
  }

  return fired;
}

void WeaponStatus::store(OArchive& archive) const
{
  archive << energyCharge << metalCharge << active;
}

WeaponStatus WeaponStatus::load(IArchive& archive)
{
  uint16 energyCharge;
  uint16 metalCharge;
  bool active;

  archive >> energyCharge >> metalCharge >> active;
  return WeaponStatus(energyCharge, metalCharge, active);
}

