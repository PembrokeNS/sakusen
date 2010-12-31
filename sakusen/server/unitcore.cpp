#include <sakusen/server/unitcore.h>

#include <sakusen/server/completeworld.h>
#include <sakusen/server/player.h>
#include <sakusen/unitstatus-methods.h>

namespace sakusen{
namespace server{

UnitCore::UnitCore(
    LayeredUnit* o,
    const UnitTypeId& startType,
    const Frame& startFrame,
    const Point<sint16>& startVelocity,
    const HitPoints startHP
  ) :
  UnitStatus(startType, startFrame, startVelocity, startHP),
  outerUnit(o),
  owner()
{
  initializeWeapons();
}

UnitCore::UnitCore(
    LayeredUnit* o,
    const UnitTypeId& startType,
    const Frame& startFrame,
    const Point<sint16>& startVelocity
  ) :
  UnitStatus(startType, startFrame, startVelocity),
  outerUnit(o),
  owner()
{
  initializeWeapons();
}

UnitCore::UnitCore(LayeredUnit* o, const UnitStatus& status) :
  UnitStatus(status),
  outerUnit(o),
  owner()
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
  const std::vector<WeaponTypeId>& weaponTypes = typePtr->getWeapons();
  for (std::vector<WeaponTypeId>::const_iterator weaponType =
      weaponTypes.begin(); weaponType != weaponTypes.end(); ++weaponType) {
    weapons.push_back(universe->getWeaponTypePtr(*weaponType)->spawn());
  }
}

void UnitCore::incrementWeaponsState()
{
  size_t numWeapons = weapons.size();
  Ref<LayeredUnit> refToThis = outerUnit->getRefToThis();

  for (size_t i=0; i<numWeapons; ++i) {
    weapons[i]->incrementState(refToThis, static_cast<uint16>(i));
    if(not refToThis) return;
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
    outerUnit->onRemoval();
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
  if (0 != amount) {
    if (hitPoints <= amount) {
      if (outerUnit->kill(amount-hitPoints)) {
        return;
      }
    } else {
      hitPoints -= amount;
    }
    outerUnit->setDirty();
  }
}

void UnitCore::repair(HitPoints amount, bool superhealth) {
  if (0 != amount) {
    HitPoints max = getMaxHitPoints();

    if (superhealth) {
      hitPoints += amount;
    } else if (hitPoints + amount > max) {
      if (hitPoints < max)
        hitPoints = max;
      /* o/w, superhealth is false but hitPoints is already more than the
       * maximum, so do nothing.
       */
    }
    else
    {
      hitPoints += amount;
    }
    outerUnit->setDirty();
  }
}

/** \brief Change the type of this unit
 *
 * \param to New type of the unit.
 * \param hpAlteration How to compute the HP of the newly transformed unit */
void UnitCore::changeType(
    const UnitTypeId& to,
    hitPointAlteration hpAlteration
  ) {
  const UnitType* newType;
  if ((newType = world->getUniverse()->getUnitTypePtr(to)) == NULL) {

  } else {
    switch (hpAlteration)
    {
      case scaleHitPoints:
        /** \todo Edit the HitPoints class to make this unnecessary. */
        /* The cast to uint64 is intended to prevent overflow */
        hitPoints = HitPoints(uint32(
              (uint64(hitPoints) *
              uint64(newType->getDynamicData().getMaxHitPoints())) /
              uint64(type->getDynamicData().getMaxHitPoints())
            ));
        break;
      case fixHitPoints:
        hitPoints =
          std::min<HitPoints>(hitPoints, newType->getDynamicData().getMaxHitPoints());
        break;
      default:
        SAKUSEN_DEBUG("Invalid hitpointAlteration");
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
void UnitCore::changeOwner(PlayerId to, enum changeOwnerReason why) {
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
    for (u_map<PlayerId, DynamicSensorReturnsRef>::type::iterator returns =
        outerUnit->getSensorReturns().begin();
        returns != outerUnit->getSensorReturns().end(); ++returns) {
      Ref<DynamicSensorReturns> r = *returns->second;
      if (0 != (r->getPerception() & (perception_unit | perception_owner))) {
        r->setDirty();
      }
    }
  }

  /* \todo Deal with subunits */
}

}
}//End Namespaces

