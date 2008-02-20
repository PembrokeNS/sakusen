#include "unitstatus-methods.h"

#include "oarchive-methods.h"

using namespace std;
using namespace sakusen;

UnitStatus::UnitStatus(const IUnitStatus& copy) :
  type(copy.getType()),
  frame(copy.getFrame()),
  velocity(copy.getVelocity()),
  hitPoints(copy.getHitPoints()),
  radarIsActive(copy.isRadarActive()),
  sonarIsActive(copy.isSonarActive()),
  weaponsStatus(copy.getWeaponsStatus())
{
  assert(getTypePtr()->getWeapons().size() == weaponsStatus.size());
}

UnitStatus::UnitStatus(
    UnitTypeId startType,
    const Frame& startFrame,
    const Point<sint16>& startVelocity,
    HitPoints startHitPoints,
    bool startRadarActive,
    bool startSonarActive,
    const std::vector<WeaponStatus>& startWeaponsStatus
  ) :
  type(startType),
  frame(startFrame),
  velocity(startVelocity),
  hitPoints(startHitPoints),
  radarIsActive(startRadarActive),
  sonarIsActive(startSonarActive),
  weaponsStatus(startWeaponsStatus)
{
  assert(type->getWeapons().size() == weaponsStatus.size());
}

UnitStatus::UnitStatus(
    UnitTypeId startType,
    const Frame& startFrame,
    const Point<sint16>& startVelocity,
    const HitPoints startHP
  ) :
  type(startType),
  frame(startFrame),
  velocity(startVelocity),
  hitPoints(startHP),
  radarIsActive(false),
  sonarIsActive(false),
  weaponsStatus()
{
  assert(world != NULL);
  initializeWeapons(getTypePtr());
}

UnitStatus::UnitStatus(
    UnitTypeId startType,
    const Frame& startFrame,
    const Point<sint16>& startVelocity
  ) :
  type(startType),
  frame(startFrame),
  velocity(startVelocity),
  radarIsActive(false),
  sonarIsActive(false),
  weaponsStatus()
{
  assert(world != NULL);
  hitPoints = getTypePtr()->getDynamicData().getMaxHitPoints();
  initializeWeapons(getTypePtr());
}

UnitStatus::UnitStatus(
    const Universe::ConstPtr& universe,
    UnitTypeId startType,
    const Frame& startFrame,
    const Point<sint16>& startVelocity
  ) :
  type(startType),
  frame(startFrame),
  velocity(startVelocity),
  hitPoints(universe->getUnitTypePtr(type)->getDynamicData().getMaxHitPoints()),
  radarIsActive(false),
  sonarIsActive(false),
  weaponsStatus()
{
  initializeWeapons(universe->getUnitTypePtr(type));
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
  frame.store(out);
  out << velocity << hitPoints << radarIsActive << sonarIsActive <<
    weaponsStatus;
}

UnitStatus UnitStatus::load(
    IArchive& in,
    const DeserializationContext& context
  )
{
  String typeName;
  UnitTypeId type;
  Point<sint16> velocity;
  
  /* status stuff */
  HitPoints hitPoints;
  bool radarIsActive;
  bool sonarIsActive;
  std::vector<WeaponStatus> weaponsStatus;
  
  in >> typeName;
  type = context.getUniverse()->getUnitTypeId(typeName);
  Frame frame = Frame::load(in);
  in >> velocity >> hitPoints >> radarIsActive >> sonarIsActive;
  in >> weaponsStatus;

  return UnitStatus(
      type, frame, velocity,
      hitPoints, radarIsActive, sonarIsActive,
      weaponsStatus
    );
}

