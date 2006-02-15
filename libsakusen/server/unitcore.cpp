#include "unitcore.h"

#include "completeworld.h"
#include "player.h"
#include "unit-methods.h"

namespace sakusen{
    namespace server{

UnitCore::UnitCore(const UnitCore& copy, LayeredUnit* o) :
  Unit(copy),
  outerUnit(o),
  owner(copy.owner)
{
}

UnitCore::UnitCore(
    LayeredUnit* o,
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity,
    HitPoints startHitPoints,
    bool startRadarActive,
    bool startSonarActive
  ) :
  Unit(
      startType, startPosition, startOrientation, startVelocity,
      startHitPoints, startRadarActive, startSonarActive
    ),
  outerUnit(o),
  owner(0)
{
}

UnitCore::UnitCore(
    LayeredUnit* o,
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity
  ) :
  Unit(startType, startPosition, startOrientation, startVelocity),
  outerUnit(o),
  owner(0)
{
}

void UnitCore::kill(HitPoints excessDamage) {
  /* do the destruct action */
  onDestruct();
  /* Change to corpse */
  changeType(getTypePtr()->getCorpseUnitType(), fullHitPoints);
  /* Apply excess damage to corpse */
  damage(excessDamage);
  /* TODO: deal with subunits */
}

void UnitCore::damage(HitPoints amount) {
  if (0 == amount) {
    return;
  }
  
  if (hitPoints <= amount) {
    kill(amount-hitPoints);
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
        hitPoints = uint64(hitPoints) * newType->getMaxHitPoints() /
          type->getMaxHitPoints();
        break;
      case fixHitPoints:
        hitPoints = std::min(hitPoints, newType->getMaxHitPoints());
        break;
      default:
        Debug("[Unit::changeType] Invalid hitpointAlteration");
      case fullHitPoints:
        hitPoints = newType->getMaxHitPoints();
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
    fromPtr->removeUnit(unitId, why);
  }
  owner = to;
  toPtr->addUnit(outerUnit, why);
  /* TODO: deal with subunits */
}

}
}//End Namespace