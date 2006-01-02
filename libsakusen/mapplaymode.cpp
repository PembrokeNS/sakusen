#include "mapplaymode.h"

#include "iarchive-methods.h"
#include "oarchive-methods.h"
#include "map.h"

MapPlayMode::MapPlayMode(
    uint32 minP,
    uint32 maxP,
    const std::vector< std::vector<Unit> >& pu) :
  minPlayers(minP),
  maxPlayers(maxP),
  playersUnits(pu)
{
  //Debug("[MapPlayMode::MapPlayMode(uint32, uint32, std::vector< std::vector<Unit> >)]");
  if (minPlayers > maxPlayers) {
    Fatal("[MapPlayMode::MapPlayMode] minPlayers > maxPlayers");
  }
  if (playersUnits.size() != maxPlayers) {
    Fatal("[MapPlayMode::MapPlayMode] maxPlayers != playersUnits.size()");
  }
}

bool MapPlayMode::sanityCheck(
    const Universe* universe,
    const Map& map)
{
  for (std::vector< std::vector<Unit> >::iterator player = playersUnits.begin();
      player != playersUnits.end(); player++) {
    for (std::vector<Unit>::iterator unit = player->begin();
        unit != player->end(); unit++) {
      if (!universe->containsUnitType(unit->getType())) {
        Debug("MapPlayMode has unit not in universe");
        return true;
      }
      if (!map.containsPoint(unit->getPosition())) {
        Debug("MapPlayMode has unit outside map");
        return true;
      }
    }
  }
  return false;
}

void MapPlayMode::store(OArchive& archive) const
{
  (archive << minPlayers << maxPlayers).insert(playersUnits);
}

MapPlayMode MapPlayMode::load(IArchive& archive, const Universe* universe)
{
  uint32 minPlayers;
  uint32 maxPlayers;
  std::vector< std::vector<Unit> > playersUnits;
  (archive >> minPlayers >> maxPlayers).extract(playersUnits, universe);
  return MapPlayMode(minPlayers, maxPlayers, playersUnits);
}

