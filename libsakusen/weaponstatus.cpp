#include "weaponstatus.h"

#include "world.h"

using namespace sakusen;

WeaponStatus::WeaponStatus() :
  direction(),
  energyCharge(0),
  metalCharge(0),
  active(false)
{
}

WeaponStatus::WeaponStatus(
    const Point<sint16>& d,
    uint16 eC,
    uint16 mC,
    bool a
  ) :
  direction(d), energyCharge(eC), metalCharge(mC), active(a)
{
}

void WeaponStatus::activate(void) {active = true;}
void WeaponStatus::deactivate(void) {active = false;}

bool WeaponStatus::incrementState(WeaponTypeID type)
{
  /** \todo Worry about what state weapons can be left in when inactive.  Can
   * they be charged, for example? */
  if (!active) {
    return false;
  }

  /** \todo inform clients about all this stuff */
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
  archive << direction << energyCharge << metalCharge << active;
}

WeaponStatus WeaponStatus::load(IArchive& archive)
{
  Point<sint16> direction;
  uint16 energyCharge;
  uint16 metalCharge;
  bool active;

  archive >> direction >> energyCharge >> metalCharge >> active;
  return WeaponStatus(direction, energyCharge, metalCharge, active);
}

