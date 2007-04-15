#include "weaponstatus.h"

#include "world.h"

using namespace sakusen;

WeaponStatus::WeaponStatus() :
  targetType(weaponTargetType_none),
  targetDirection(),
  energyCharge(0),
  metalCharge(0),
  active(false)
{
}

WeaponStatus::WeaponStatus(
    WeaponTargetType tT,
    const Point<sint16>& tD,
    uint16 eC,
    uint16 mC,
    bool a
  ) :
  targetType(tT),
  targetDirection(tD),
  energyCharge(eC),
  metalCharge(mC),
  active(a)
{
}

void WeaponStatus::setTargetDirection(const Point<sint16>& d)
{
  targetType = weaponTargetType_direction;
  targetDirection = d;
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
  archive.insertEnum(targetType) << targetDirection;
  archive << energyCharge << metalCharge << active;
}

WeaponStatus WeaponStatus::load(IArchive& archive)
{
  WeaponTargetType targetType;
  Point<sint16> targetDirection;
  uint16 energyCharge;
  uint16 metalCharge;
  bool active;

  archive.extractEnum(targetType) >> targetDirection;
  archive >> energyCharge >> metalCharge >> active;
  return WeaponStatus(
      targetType, targetDirection,
      energyCharge, metalCharge, active
    );
}

