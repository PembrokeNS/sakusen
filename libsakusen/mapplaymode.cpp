#include "mapplaymode.h"

#include "map.h"

using namespace sakusen;

MapPlayMode::MapPlayMode(
    uint32 minP,
    uint32 maxP,
    const std::vector<PlayerTemplate>& p
  ) :
  minPlayers(minP),
  maxPlayers(maxP),
  players(p)
{
  assert(minPlayers <= maxPlayers);
  assert(players.size() == maxPlayers);
}

bool MapPlayMode::sanityCheck(
    const Universe* universe,
    const MapTemplate& map
  )
{
  for (std::vector<PlayerTemplate>::iterator player = players.begin();
      player != players.end(); player++) {
    if (player->sanityCheck(universe, map)) {
      return true;
    }
  }
  return false;
}

void MapPlayMode::store(OArchive& archive) const
{
	archive << minPlayers << maxPlayers << players;
}

MapPlayMode MapPlayMode::load(IArchive& archive, const Universe* universe)
{
  uint32 minPlayers;
  uint32 maxPlayers;
  std::vector<PlayerTemplate> players;
  (archive >> minPlayers >> maxPlayers).extract(players, universe);
  return MapPlayMode(minPlayers, maxPlayers, players);
}

