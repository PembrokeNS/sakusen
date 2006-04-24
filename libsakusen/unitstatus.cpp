#include "unitstatus-methods.h"

#include "oarchive-methods.h"

using namespace sakusen;

UnitStatus::UnitStatus(
    UnitTypeID t,
    const Point<sint32>& p,
    const Orientation& o,
    const Point<sint16>& v,
    /* status stuff */
    HitPoints hP,
    bool rIA,
    bool sIA,
    const std::list<uint32>& su,
    const std::list<Weapon>& w
  ) :
  type(t),
  position(p),
  orientation(o),
  velocity(v),
  hitPoints(hP),
  radarIsActive(rIA),
  sonarIsActive(sIA),
  subunits(su),
  weapons(w)
{
}

UnitStatus::UnitStatus(const IUnitStatus* copy) :
  type(copy->getType()),
  position(copy->getPosition()),
  orientation(copy->getOrientation()),
  velocity(copy->getVelocity()),
  hitPoints(copy->getHitPoints()),
  radarIsActive(copy->isRadarActive()),
  sonarIsActive(copy->isSonarActive())
{
  /* TODO: subunits, weapons */
}

UnitStatus::UnitStatus(
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity,
    HitPoints startHitPoints,
    bool startRadarActive,
    bool startSonarActive
  ) :
  type(startType),
  position(startPosition),
  orientation(startOrientation),
  velocity(startVelocity),
  hitPoints(startHitPoints),
  radarIsActive(startRadarActive),
  sonarIsActive(startSonarActive),
  subunits(),
  weapons()
{
  initializeWeapons(
      getTypePtr(), world->getUniverse()
    );
}

UnitStatus::UnitStatus(
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity
  ) :
  type(startType),
  position(startPosition),
  orientation(startOrientation),
  velocity(startVelocity),
  radarIsActive(false),
  sonarIsActive(false),
  subunits(),
  weapons()
{
  assert(world != NULL);
  initializeWeapons(
      getTypePtr(), world->getUniverse()
    );
  hitPoints = getTypePtr()->getDynamicData().getMaxHitPoints();
}

UnitStatus::UnitStatus(
    const Universe* universe,
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity
  ) :
  type(startType),
  position(startPosition),
  orientation(startOrientation),
  velocity(startVelocity),
  radarIsActive(false),
  sonarIsActive(false),
  subunits(),
  weapons()
{
  initializeWeapons(
      universe->getUnitTypePtr(type), universe
    );
  hitPoints =
    universe->getUnitTypePtr(type)->getDynamicData().getMaxHitPoints();
}

void UnitStatus::initializeWeapons(
    const UnitType* typePtr,
    const Universe* universe
  )
{
  /* add weapons */
  const std::list<WeaponTypeID>& weaponTypes = typePtr->getWeapons();
  for (std::list<WeaponTypeID>::const_iterator weaponType = weaponTypes.begin();
      weaponType != weaponTypes.end(); weaponType++) {
    weapons.push_back(Weapon(universe->getWeaponTypePtr(*weaponType)));
  }
}

void UnitStatus::store(OArchive& out, const Universe* universe) const
{
  out << universe->getUnitTypePtr(type)->getInternalName();
  out << position;
  orientation.store(out);
  out << velocity << hitPoints << radarIsActive << sonarIsActive << subunits;
  /* TODO: weapons */
}

UnitStatus UnitStatus::load(IArchive& in, const Universe* universe)
{
  String typeName;
  UnitTypeID type;
  Point<sint32> position;
  Orientation orientation;
  Point<sint16> velocity;
  
  /* status stuff */
  HitPoints hitPoints;
  bool radarIsActive;
  bool sonarIsActive;
  std::list<uint32> subunits;
  std::list<Weapon> weapons;
  
  in >> typeName;
  type = universe->getUnitTypeID(typeName);
  in >> position;
  orientation = Orientation::load(in);
  in >> velocity >> hitPoints >> radarIsActive >> sonarIsActive >> subunits;
  /* TODO: weapons */

  return UnitStatus(
      type, position, orientation, velocity,
      hitPoints, radarIsActive, sonarIsActive, subunits,
      weapons
    );
}

