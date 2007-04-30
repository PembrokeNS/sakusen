#include "unitcore.h"

#include "completeworld.h"
#include "player.h"
#include "unitstatus-methods.h"

using namespace __gnu_cxx;

namespace sakusen{
namespace server{

UnitCore::UnitCore(const UnitCore& copy, LayeredUnit* o) :
  UnitStatus(copy),
  outerUnit(o),
  owner(copy.owner),
  weapons()
{
  for (vector<Weapon*>::const_iterator weapon = copy.weapons.begin();
      weapon != copy.weapons.end(); ++weapon) {
    weapons.push_back((*weapon)->newCopy());
  }
}

UnitCore::UnitCore(
    LayeredUnit* o,
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity,
    const HitPoints startHP
  ) :
  UnitStatus(startType, startPosition, startOrientation, startVelocity, startHP),
  outerUnit(o),
  owner(0)
{
  initializeWeapons();
}

UnitCore::UnitCore(LayeredUnit* o, const UnitStatus& status) :
  UnitStatus(status),
  outerUnit(o),
  owner(0)
{
  initializeWeapons();
}

UnitCore::~UnitCore()
{
  while (!weapons.empty()) {
    delete weapons.back();
    weapons.pop_back();
  }
}

void UnitCore::initializeWeapons()
{
  Universe::ConstPtr universe = world->getUniverse();
  const UnitType* typePtr = universe->getUnitTypePtr(type);
  /* add weapons */
  const std::vector<WeaponTypeID>& weaponTypes = typePtr->getWeapons();
  for (std::vector<WeaponTypeID>::const_iterator weaponType =
      weaponTypes.begin(); weaponType != weaponTypes.end(); ++weaponType) {
    weapons.push_back(universe->getWeaponTypePtr(*weaponType)->spawn());
  }
}

void UnitCore::incrementWeaponsState()
{
  size_t numWeapons = weapons.size();

  for (size_t i=0; i<numWeapons; ++i) {
    weapons[i]->incrementState(outerUnit->getRefToThis(), i);
  }
}

/** \brief Kill the unit.
 *
 * \param excessDamage Amount of 'spillover' damage to apply to whatever corpse
 * replaces the unit, if any.
 *
 * \return true iff the unit was entirely removed, rather than being replaced
 * by a corpse.  In this case, the Unit pointer used to make the call should be
 * considered invalidated - don't do anything else with it! */
bool UnitCore::kill(HitPoints excessDamage) {
  /* do the destruct action */
  outerUnit->onDestruct();
  /* Find corpse type */
  const UnitType* corpseType =
    world->getUniverse()->getUnitTypePtr(getTypePtr()->getCorpseUnitType());
  if (NULL == corpseType) {
    /* No corpse, remove me */
    /** \todo deal with subunits */
    world->removeUnit(outerUnit);
    return true;
  } else {
    /* Change to corpse */
    outerUnit->changeType(corpseType, fullHitPoints);
    /* Apply excess damage to corpse */
    outerUnit->damage(excessDamage);
    /** \todo deal with subunits */
    return false;
  }
}

void UnitCore::damage(HitPoints amount) {
  if (0 == amount) {
    return;
  }
  
  if (hitPoints <= amount) {
    if (outerUnit->kill(amount-hitPoints)) {
      return;
    }
  } else {
    hitPoints -= amount;
  }
  outerUnit->setDirty();
}

void UnitCore::repair(HitPoints amount, bool superhealth) {
  if (0 == amount) {
    return;
  }
  
  if (superhealth) {
    hitPoints += amount;
  } else {
    if (hitPoints + amount > getMaxHitPoints())
      hitPoints = getMaxHitPoints();
    else
      hitPoints += amount;
  }
  outerUnit->setDirty();
}

/** \brief Change the type of this unit
 *
 * \param to New type of the unit.
 * \param hpAlteration How to compute the HP of the newly transformed unit */
void UnitCore::changeType(
    const UnitTypeID& to,
    hitPointAlteration hpAlteration
  ) {
  const UnitType* newType;
  if ((newType = world->getUniverse()->getUnitTypePtr(to)) == NULL) {
    
  } else {
    switch (hpAlteration)
    {
      case scaleHitPoints:
        /* The cast to uint64 is intended to prevent overflow */
        hitPoints = static_cast<HitPoints>((static_cast<uint64>(hitPoints) *
          static_cast<uint64>(newType->getDynamicData().getMaxHitPoints())) /
          static_cast<uint64>(type->getDynamicData().getMaxHitPoints()));
        break;
      case fixHitPoints:
        hitPoints =
          static_cast<HitPoints>(std::min(hitPoints, newType->getDynamicData().getMaxHitPoints()));
        break;
      default:
        Debug("Invalid hitpointAlteration");
        /* intentionally no break */
      case fullHitPoints:
        hitPoints = newType->getDynamicData().getMaxHitPoints();
        break;
    }
    type = to;

    /** \todo Check everything is within the new bounds (e.g. speed)
     * (unless the engine does that for us at next tick) */
    /** \todo Deal with subunits */
    outerUnit->setDirty();
  }
}

/** \brief Change unit ownership
 *
 * \param to New owner of the unit.
 * \param why Reason for the ownership change */
void UnitCore::changeOwner(PlayerID to, enum changeOwnerReason why) {
  /* existence check goes here */
  Player* fromPtr = world->getPlayerPtr(owner);
  Player* toPtr = world->getPlayerPtr(to);
  if (why != changeOwnerReason_created) {
    /* We clear the dirty flag so that the old owner gets to see the very last
     * changes to the unit, if any */
    outerUnit->clearDirty();
    fromPtr->removeUnit(outerUnit->getId(), why);
  }
  owner = to;
  if (why != changeOwnerReason_destroyed) {
    toPtr->addUnit(outerUnit->getRefToThis(), why);
    
    /* We also need to ensure that sensor returns from this unit are flagged so
     * that the change of ownership is transmitted. */
    for (hash_map<PlayerID, DynamicSensorReturnsRef>::iterator returns =
        outerUnit->getSensorReturns().begin();
        returns != outerUnit->getSensorReturns().end(); ++returns) {
      Ref<DynamicSensorReturns> r = returns->second->second;
      if (0 != (r->getPerception() & (perception_unit | perception_owner))) {
        r->setDirty();
      }
    }
  }
  
  /* \todo Deal with subunits */
}

}
}//End Namespaces

