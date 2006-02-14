#include "unit-methods.h"

using namespace sakusen;

Unit::Unit(
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity,
    HitPoints startHitPoints,
    bool startRadarActive,
    bool startSonarActive
  ) :
  type(startType),
  unitId(0),
  position(startPosition),
  orientation(startOrientation),
  velocity(startVelocity),
  hitPoints(startHitPoints),
  radarIsActive(startRadarActive),
  sonarIsActive(startSonarActive),
  subunits(),
  weapons(),
  linearTarget(linearTargetType_none),
  rotationalTarget(rotationalTargetType_none)
{
  initializeWeapons(
      getTypePtr(), world->getUniverse()
    );
}

Unit::Unit(
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity
  ) :
  type(startType),
  unitId(0),
  position(startPosition),
  orientation(startOrientation),
  velocity(startVelocity),
  radarIsActive(false),
  sonarIsActive(false),
  subunits(),
  weapons(),
  linearTarget(linearTargetType_none),
  rotationalTarget(rotationalTargetType_none)
{
  initializeWeapons(
      getTypePtr(), world->getUniverse()
    );
  hitPoints = getTypePtr()->getMaxHitPoints();
}

void Unit::initializeWeapons(const UnitType* typePtr, const Universe* universe)
{
  /* add weapons */
  const std::list<WeaponTypeID>& weaponTypes = typePtr->getWeapons();
  for (std::list<WeaponTypeID>::const_iterator weaponType = weaponTypes.begin();
      weaponType != weaponTypes.end(); weaponType++) {
    weapons.push_back(Weapon(universe->getWeaponTypePtr(*weaponType)));
  }
}

