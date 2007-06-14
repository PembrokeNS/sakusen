#include "unitstatus-methods.h"

#include "oarchive-methods.h"

using namespace std;
using namespace sakusen;

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
}

UnitStatus::UnitStatus(
    UnitTypeID startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity,
    HitPoints startHitPoints,
    bool startRadarActive,
    bool startSonarActive,
    const std::vector<WeaponStatus>& startWeaponsStatus
  ) :
  type(startType),
  position(startPosition),
  orientation(startOrientation),
  velocity(startVelocity),
  hitPoints(startHitPoints),
  radarIsActive(startRadarActive),
  sonarIsActive(startSonarActive),
  weaponsStatus(startWeaponsStatus)
{
  assert(type->getWeapons().size() == weaponsStatus.size());
}

UnitStatus::UnitStatus(
    UnitTypeID startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity,
    const HitPoints startHP
  ) :
  type(startType),
  position(startPosition),
  orientation(startOrientation),
  velocity(startVelocity),
  radarIsActive(false),
  sonarIsActive(false),
  weaponsStatus()
{
  assert(world != NULL);
  initializeWeapons(getTypePtr());
  if (startHP == HitPoints(-1)) {
    hitPoints = getTypePtr()->getDynamicData().getMaxHitPoints();
  } else {
    hitPoints = startHP;
  }
}

UnitStatus::UnitStatus(
    const Universe::ConstPtr& universe,
    UnitTypeID startType,
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
  weaponsStatus()
{
  initializeWeapons(universe->getUnitTypePtr(type));
  hitPoints =
    universe->getUnitTypePtr(type)->getDynamicData().getMaxHitPoints();
}

/** \brief Create default statuses for each of the weapons */
void UnitStatus::initializeWeapons(const UnitType* typePtr)
{
  while (weaponsStatus.size() < typePtr->getWeapons().size()) {
    weaponsStatus.push_back(WeaponStatus());
  }
}

void UnitStatus::store(OArchive& out, const Universe::ConstPtr& universe) const
{
  out << universe->getUnitTypePtr(type)->getInternalName();
  out << position;
  orientation.store(out);
  out << velocity << hitPoints << radarIsActive << sonarIsActive <<
    weaponsStatus;
}

UnitStatus UnitStatus::load(
    IArchive& in,
    const DeserializationContext& context
  )
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
  std::vector<WeaponStatus> weaponsStatus;
  
  in >> typeName;
  type = context.getUniverse()->getUnitTypeId(typeName);
  in >> position;
  orientation = Orientation::load(in);
  in >> velocity >> hitPoints >> radarIsActive >> sonarIsActive;
  in >> weaponsStatus;

  return UnitStatus(
      type, position, orientation, velocity,
      hitPoints, radarIsActive, sonarIsActive,
      weaponsStatus
    );
}

