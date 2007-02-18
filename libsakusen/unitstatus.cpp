#include "unitstatus-methods.h"

#include "oarchive-methods.h"

using namespace std;
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
    const std::vector<WeaponStatus>& w
  ) :
  type(t),
  position(p),
  orientation(o),
  velocity(v),
  hitPoints(hP),
  radarIsActive(rIA),
  sonarIsActive(sIA),
  subunits(su),
  weaponsStatus(w)
{
  assert(t->getWeapons().size() == weaponsStatus.size());
}

UnitStatus::UnitStatus(const IUnitStatus* copy) :
  type(copy->getType()),
  position(copy->getPosition()),
  orientation(copy->getOrientation()),
  velocity(copy->getVelocity()),
  hitPoints(copy->getHitPoints()),
  radarIsActive(copy->isRadarActive()),
  sonarIsActive(copy->isSonarActive()),
  weaponsStatus(copy->getWeaponsStatus())
{
  assert(getTypePtr()->getWeapons().size() == weaponsStatus.size());
  /* TODO: subunits */
}

UnitStatus::UnitStatus(
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity,
    HitPoints startHitPoints,
    bool startRadarActive,
    bool startSonarActive,
    vector<WeaponStatus> startWeaponsStatus
  ) :
  type(startType),
  position(startPosition),
  orientation(startOrientation),
  velocity(startVelocity),
  hitPoints(startHitPoints),
  radarIsActive(startRadarActive),
  sonarIsActive(startSonarActive),
  subunits(),
  weaponsStatus(startWeaponsStatus)
{
  assert(getTypePtr()->getWeapons().size() == weaponsStatus.size());
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
  weaponsStatus()
{
  assert(world != NULL);
  initializeWeapons(getTypePtr());
  hitPoints = getTypePtr()->getDynamicData().getMaxHitPoints();
}

UnitStatus::UnitStatus(
    const Universe::ConstPtr& universe,
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
  weaponsStatus()
{
  initializeWeapons(universe->getUnitTypePtr(type));
  hitPoints =
    universe->getUnitTypePtr(type)->getDynamicData().getMaxHitPoints();
}

void UnitStatus::initializeWeapons(const UnitType* typePtr)
{
  /* Create default statuses for each of the weapons */
  while (weaponsStatus.size() < typePtr->getWeapons().size())
    weaponsStatus.push_back(WeaponStatus());
}

void UnitStatus::store(OArchive& out, const Universe::ConstPtr& universe) const
{
  out << universe->getUnitTypePtr(type)->getInternalName();
  out << position;
  orientation.store(out);
  out << velocity << hitPoints << radarIsActive << sonarIsActive << subunits <<
    weaponsStatus;
}

UnitStatus UnitStatus::load(IArchive& in, const Universe::ConstPtr* universe)
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
  std::vector<WeaponStatus> weaponsStatus;
  
  in >> typeName;
  type = (*universe)->getUnitTypeID(typeName);
  in >> position;
  orientation = Orientation::load(in);
  in >> velocity >> hitPoints >> radarIsActive >> sonarIsActive >> subunits >>
    weaponsStatus;

  return UnitStatus(
      type, position, orientation, velocity,
      hitPoints, radarIsActive, sonarIsActive, subunits,
      weaponsStatus
    );
}

