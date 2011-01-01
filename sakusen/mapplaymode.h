#ifndef LIBSAKUSEN__MAPPLAYMODE_H
#define LIBSAKUSEN__MAPPLAYMODE_H

#include <sakusen/global.h>
#include <sakusen/universe.h>
#include <sakusen/playertemplate.h>
#include <sakusen/playerid.h>

namespace sakusen {

class MapTemplate;

/** \brief Defines a paticular way of playing a game on a MapTemplate.
 *
 * A MapPlayMode specifies all the players, their units, victory conditions
 * and other things necessary to turn a bare heightfield into something that
 * can actually be played.
 *
 * Most of the important information is inside PlayerTemplates.
 *
 * \todo scripting / map-based actions */
class LIBSAKUSEN_API MapPlayMode {
  private:
    MapPlayMode();
  public:
    MapPlayMode(
        uint32 minPlayers,
        uint32 maxPlayers,
        const std::vector<PlayerTemplate>& players
      );
    ~MapPlayMode() {}
  private:
    uint32 minPlayers;
    uint32 maxPlayers;
    std::vector<PlayerTemplate> players;
  public:
    inline uint32 getMinPlayers() const { return minPlayers; }
    inline uint32 getMaxPlayers() const { return maxPlayers; }

    inline bool acceptableNumberOfPlayers(uint32 numPlayers) const
    {
      return numPlayers >= minPlayers && numPlayers <= maxPlayers;
    }
    inline const PlayerTemplate& getPlayer(PlayerId player) const
    {
      assert(player < maxPlayers);
      assert(player < players.size());
      return players[player];
    }

    bool sanityCheck(
        const Universe::ConstPtr& universe,
        const MapTemplate& map
      );

    void store(OArchive&) const;
    static MapPlayMode load(IArchive&, const DeserializationContext&);
};

}

#endif // LIBSAKUSEN__MAPPLAYMODE_H

