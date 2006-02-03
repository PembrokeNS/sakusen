#ifndef MAPPLAYMODE_H
#define MAPPLAYMODE_H

#include "libsakusen-global.h"
#include "unit.h"
#include "universe.h"
#include "heightfield.h"

namespace sakusen {

class Map;

class LIBSAKUSEN_API MapPlayMode {
  private:
    MapPlayMode();
  public:
    MapPlayMode(
        uint32 minPlayers,
        uint32 maxPlayers,
        const std::vector< std::vector<Unit> >& playersUnits
      );
    ~MapPlayMode() {}
  private:
    uint32 minPlayers;
    uint32 maxPlayers;
    std::vector< std::vector<Unit> > playersUnits;
    /* TODO: scripting / map-based actions */
  public:
    inline uint32 getMinPlayers() const { return minPlayers; }
    inline uint32 getMaxPlayers() const { return maxPlayers; }

    inline bool acceptableNumberOfPlayers(uint32 numPlayers) const
    {
      return numPlayers >= minPlayers && numPlayers <= maxPlayers;
    }
    inline const std::vector<Unit>& getPlayersUnits(
        uint32 numPlayers,
        PlayerID player) const
    {
      assert(player <= numPlayers);
      assert(player < maxPlayers);
      assert(player < playersUnits.size());
      /* TODO: allow for a given player to get different units based on the
       * number of players */
      return playersUnits[player];
    }
    bool sanityCheck(const Universe* universe, const Map& map);
      /* Confirms that all specified units exist in the given universe, and
       * that they all fit onto the given heightfield.  Returns true if a
       * problem is found */
    typedef Universe loadArgument;
    void store(OArchive&) const;
    static MapPlayMode load(IArchive&, const loadArgument*);
};

}

#endif // MAPPLAYMODE_H

