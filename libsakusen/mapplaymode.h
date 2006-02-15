#ifndef MAPPLAYMODE_H
#define MAPPLAYMODE_H

#include "libsakusen-global.h"
#include "universe.h"
#include "playertemplate.h"
#include "playerid.h"

namespace sakusen {

class MapTemplate;

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
    /* TODO: scripting / map-based actions */
  public:
    inline uint32 getMinPlayers() const { return minPlayers; }
    inline uint32 getMaxPlayers() const { return maxPlayers; }

    inline bool acceptableNumberOfPlayers(uint32 numPlayers) const
    {
      return numPlayers >= minPlayers && numPlayers <= maxPlayers;
    }
    inline const PlayerTemplate& getPlayer(PlayerID player) const
    {
      assert(player < maxPlayers);
      assert(player < players.size());
      return players[player];
    }
    bool sanityCheck(const Universe* universe, const MapTemplate& map);
      /* Confirms that all specified units exist in the given universe, and
       * that they all fit onto the given map.  Returns true if a
       * problem is found */
    
    typedef Universe loadArgument;
    void store(OArchive&) const;
    static MapPlayMode load(IArchive&, const loadArgument*);
};

}

#endif // MAPPLAYMODE_H

