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

/** \brief Performs some sanity checks on the PlayerTemplate
 *
 * Confirms that all specified units exist in the given universe, and
 * that they all fit onto the given map.  Returns true if a
 * problem is found */
bool PlayerTemplate::sanityCheck(
    const Universe::ConstPtr& universe,
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
    const DeserializationContext& context
  )
{
  bool noClients;
  bool fixedRace;
  std::vector<UnitTemplate> units;
  (archive >> noClients >> fixedRace).extract(units, context);
  return PlayerTemplate(noClients, fixedRace, units);
}

