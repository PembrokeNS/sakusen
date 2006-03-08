#include "unitcore.h"

#include "completeworld.h"
#include "player.h"
#include "unitstatus-methods.h"

namespace sakusen{
namespace server{

UnitCore::UnitCore(const UnitCore& copy, LayeredUnit* o) :
  UnitStatus(copy),
  outerUnit(o),
  owner(copy.owner)
{
}

UnitCore::UnitCore(
    LayeredUnit* o,
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity
  ) :
  UnitStatus(startType, startPosition, startOrientation, startVelocity),
  outerUnit(o),
  owner(0)
{
}

UnitCore::UnitCore(LayeredUnit* o, const UnitStatus& status) :
  UnitStatus(status),
  outerUnit(o),
  owner(0)
{
}

void UnitCore::kill(HitPoints excessDamage) {
  /* do the destruct action */
  outerUnit->onDestruct();
  /* Change to corpse */
  outerUnit->changeType(getTypePtr()->getCorpseUnitType(), fullHitPoints);
  /* Apply excess damage to corpse */
  outerUnit->damage(excessDamage);
  /* TODO: deal with subunits */
}

void UnitCore::damage(HitPoints amount) {
  if (0 == amount) {
    return;
  }
  
  if (hitPoints <= amount) {
    outerUnit->kill(amount-hitPoints);
  } else {
    hitPoints -= amount;
  }
  /* TODO: inform clients */
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
  /* TODO: inform clients */
}

void UnitCore::changeType(
    const UnitTypeID& to,
    hitPointAlteration hpAlteration
  ) {
  const UnitType* newType;
  if ((newType = world->getUniverse()->getUnitTypePtr(to)) == NULL) {
    Debug("[Unit::changeType] Invalid UnitTypeID");
  } else {
    switch (hpAlteration)
    {
      case scaleHitPoints:
        /* The cast to uint64 is intended to prevent overflow */
        hitPoints = uint64(hitPoints) *
          newType->getDynamicData().getMaxHitPoints() /
          type->getDynamicData().getMaxHitPoints();
        break;
      case fixHitPoints:
        hitPoints =
          std::min(hitPoints, newType->getDynamicData().getMaxHitPoints());
        break;
      default:
        Debug("[Unit::changeType] Invalid hitpointAlteration");
        /* intentionally no break */
      case fullHitPoints:
        hitPoints = newType->getDynamicData().getMaxHitPoints();
        break;
    }
    type = to;

    /* TODO: check everything is within the new bounds (e.g. speed)
     * (unless the engine does that for us at next tick) */
    /* TODO: deal with subunits */
    /* TODO: inform clients */
  }
}

void UnitCore::changeOwner(PlayerID to, enum changeOwnerReason why) {
  /* existence check goes here */
  Player* fromPtr = world->getPlayerPtr(owner);
  Player* toPtr = world->getPlayerPtr(to);
  if (why != changeOwnerReason_created) {
    fromPtr->removeUnit(outerUnit->getId(), why);
  }
  owner = to;
  toPtr->addUnit(outerUnit, why);
  /* TODO: deal with subunits */
}

}
}//End Namespace
