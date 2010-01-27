#include "playertemplate.h"

#include "maptemplate.h"
#include "oarchive-methods.h"

using namespace sakusen;

PlayerTemplate::PlayerTemplate(
    bool nc,
    bool rf,
    const std::vector<UnitTemplate>& u,
    const Position& p
  ) :
  noClients(nc),
  raceFixed(rf),
  units(u),
  startPos(p)
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
    if (!universe->containsUnitType(unit->getStatus().getType())) {
      SAKUSEN_DEBUG("MapPlayMode has unit not in universe");
      return true;
    }
    if (!map.containsPoint(unit->getStatus().getPosition())) {
      SAKUSEN_DEBUG("MapPlayMode has unit outside map");
      return true;
    }
  }

  return false;
}

void PlayerTemplate::store(OArchive& archive) const
{
  archive << noClients << raceFixed << units << startPos;
}

PlayerTemplate PlayerTemplate::load(
    IArchive& archive,
    const DeserializationContext& context
  )
{
  bool noClients;
  bool fixedRace;
  std::vector<UnitTemplate> units;
  Position p;
  (archive >> noClients >> fixedRace).extract(units, context) >> p;
  return PlayerTemplate(noClients, fixedRace, units, p);
}

