#include "playertemplate.h"

#include "maptemplate.h"
#include "oarchive-methods.h"

using namespace sakusen;

PlayerTemplate::PlayerTemplate(
    bool nc,
    bool rf,
    const std::vector<UnitTemplate>& u
  ) :
  noClients(nc),
  raceFixed(rf),
  units(u)
{
  assert(raceFixed || !noClients);
}

bool PlayerTemplate::sanityCheck(
    const Universe* universe,
    const MapTemplate& map
  )
{
  for (std::vector<UnitTemplate>::iterator unit = units.begin();
      unit != units.end(); unit++) {
    if (!universe->containsUnitType(unit->getStatus()->getType())) {
      Debug("MapPlayMode has unit not in universe");
      return true;
    }
    if (!map.containsPoint(unit->getStatus()->getPosition())) {
      Debug("MapPlayMode has unit outside map");
      return true;
    }
  }

  return false;
}

void PlayerTemplate::store(OArchive& archive) const
{
  archive << noClients << raceFixed << units;
}

PlayerTemplate PlayerTemplate::load(
    IArchive& archive,
    const loadArgument* arg
  )
{
  bool noClients;
  bool fixedRace;
  std::vector<UnitTemplate> units;
  (archive >> noClients >> fixedRace).extract(units, arg);
  return PlayerTemplate(noClients, fixedRace, units);
}

