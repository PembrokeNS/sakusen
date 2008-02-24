#include "weaponstatus.h"

#include "world.h"
#include "icompleteunit.h"

using namespace std;
using namespace sakusen;

WeaponStatus::WeaponStatus() :
  targetType(weaponTargetType_none),
  targetDirection(),
  targetFrame(),
  energyCharge(0),
  metalCharge(0),
  active(false)
{
}

WeaponStatus::WeaponStatus(
    WeaponTargetType tT,
    const Point<sint32>& tD,
    uint16 eC,
    uint16 mC,
    bool a
  ) :
  targetType(tT),
  targetDirection(tD),
  targetFrame(),
  energyCharge(eC),
  metalCharge(mC),
  active(a)
{
}

void WeaponStatus::setTargetDirection(const Point<sint32>& d)
{
  targetType = weaponTargetType_direction;
  targetDirection = d;
  targetPosition = Position();
  targetFrame = Frame();
}

void WeaponStatus::setTarget(const Frame& f)
{
  targetType = weaponTargetType_frame;
  targetDirection = Point<sint32>();
  targetPosition = Position();
  targetFrame = f;
}

void WeaponStatus::setTarget(const Ref<ICompleteUnit>& u)
{
  targetType = weaponTargetType_unit;
  targetDirection = Point<sint32>();
  targetPosition = u->getIStatus().getPosition();
  targetFrame = Frame();
}

void WeaponStatus::activate(void) {active = true;}
void WeaponStatus::deactivate(void) {active = false;}

/** \brief Increment the state of the weapon
 *
 * If the Weapon is inactive, does nothing and returns false.
 *
 * If the Weapon is active, adds as much energy and metal to it as possible
 * (subject to constraints of charging speed and availability).  If it is fully
 * charged, then remove firing cost from energy and weapon charges and return
 * true (in this case the Weapon should be fired), otherwise return false.
 *
 * \param type     Type id of this weapon.
 * \param provider ResourceProvider from which requests for resources should be
 *                 made.
 */
bool WeaponStatus::incrementState(WeaponTypeId type, MaterielProvider& provider)
{
  /** \todo Worry about what state weapons can be left in when inactive.  Can
   * they be charged, for example? */
  if (!active) {
    return false;
  }

  /** \todo inform clients about all this stuff */
  const WeaponType* typePtr = world->getUniverse()->getWeaponTypePtr(type);

  /** \todo Do we need a separate cap for charge?  For now we use the cost. */
  const uint32 energyCap = typePtr->getEnergyCost();
  const uint32 metalCap = typePtr->getMetalCost();

  /** \bug The resources should not appear from thin air */
  energyCharge += provider.requestEnergy(
      min<uint32>(energyCap-energyCharge, typePtr->getEnergyRate())
    );
  metalCharge += provider.requestMetal(
      min<uint32>(metalCap-metalCharge, typePtr->getMetalRate())
    );

  bool fired = false;

  if (energyCharge >= typePtr->getEnergyCost() &&
      metalCharge >= typePtr->getMetalCost()) {
    fired = true;
    energyCharge -=typePtr->getEnergyCost();
    metalCharge -=typePtr->getMetalCost();
  }

  if (energyCharge > energyCap) {
    energyCharge = energyCap;
  }
  if (metalCharge > metalCap) {
    metalCharge = metalCap;
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
  Point<sint32> targetDirection;
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

