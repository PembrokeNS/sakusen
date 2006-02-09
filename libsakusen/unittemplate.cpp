#include "unit.h"

#include "world.h"

using namespace sakusen;

UnitTemplate::UnitTemplate(
    const Universe* u,
    const UnitTypeID& t,
    const Point<sint32>& p,
    const Orientation& o,
    const Point<sint16>& v,
    HitPoints hp,
    bool ria,
    bool sia
  ) :
  universe(u),
  type(t),
  position(p),
  orientation(o),
  velocity(v),
  hitPoints(hp),
  radarIsActive(ria),
  sonarIsActive(sia),
  subunits()
{
  assert(world==NULL); /* This constructor is for when making maps, not when
                          playing the game */
  assert (!radarIsActive ||
      universe->getUnitTypePtr(type)->getVision().radarActive.capable);
  assert (!sonarIsActive ||
      universe->getUnitTypePtr(type)->getVision().sonarActive.capable);
}

bool UnitTemplate::setRadarActive(bool active) {
  if (universe->getUnitTypePtr(type)->getVision().radarActive.capable) {
    radarIsActive = active;
    return active;
  } else return false;
}

bool UnitTemplate::setSonarActive(bool active) {
  if (universe->getUnitTypePtr(type)->getVision().sonarActive.capable) {
    sonarIsActive = active;
    return active;
  } else return false;
}

void UnitTemplate::store(OArchive& archive) const
{
  archive << universe->getUnitTypePtr(type)->getInternalName() << position;
  orientation.store(archive);
  archive << velocity << hitPoints << radarIsActive << sonarIsActive;
  /* TODO: store subunit info */
}

UnitTemplate UnitTemplate::load(IArchive& archive, const Universe* universe)
{
  String typeName;
  archive >> typeName;
  UnitTypeID type = universe->getUnitTypeID(typeName);
  Point<sint32> position;
  archive >> position;
  Orientation orientation(Orientation::load(archive));
  Point<sint16> velocity;
  HitPoints hitPoints;
  bool radarIsActive;
  bool sonarIsActive;
  archive >> velocity >> hitPoints >> radarIsActive >> sonarIsActive;
  /* TODO: load subunit info */
  return UnitTemplate(
      universe, type, position, orientation, velocity, hitPoints,
      radarIsActive, sonarIsActive
    );
}

