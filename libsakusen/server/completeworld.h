#ifndef LIBSAKUSEN_SERVER__COMPLETEWORLD_H
#define LIBSAKUSEN_SERVER__COMPLETEWORLD_H

#include "world.h"
#include "player.h"
#include "completemap.h"
#include "layeredunit.h"
#include "fuse.h"
#include "ballistic.h"
#include "beam.h"
#include "effect.h"

namespace sakusen {
namespace server {

class LIBSAKUSEN_SERVER_API CompleteWorld : public World {
  private:
    CompleteWorld();
    CompleteWorld(const CompleteWorld& copy);
      /* Shouldn't use the copy constructor - mayhem will ensue because it will
       * invalidate all pointers to game objects stored here */
  public:
    CompleteWorld(
        const MapTemplate& map,
        uint32 playMode, /* what mode of the map we are using */
        const std::vector<Player>& players
      ); /* The constructor which the server is actually expected to use */
    ~CompleteWorld();
  private:
    CompleteMap* map; /* owned by this */
    std::list<LayeredUnit> units; /* this list includes subunits */
    std::list<Ballistic*> ballistics;
    std::list<Beam*> beams;
    std::list<Effect*> effects;
    FuseQueue fuseQueue; /* the FuseQueue is a FIFO priority queue */
    std::vector<Player> players;
  public:
    /* accessors */
    inline Map* getMap(void) { return map; }
    inline const Map* getMap(void) const { return map; }
    inline const CompleteMap* getCompleteMap(void) const { return map; }
    inline void addUnit(const LayeredUnit& unit, PlayerID owner) {
      units.push_back(unit);
      units.back().changeOwner(owner, changeOwnerReason_created);
    }
    inline std::list<LayeredUnit>& getUnits(void) { return units; }
    
    inline void addBallistic(Ballistic* ballistic)
    {
      ballistics.push_back(ballistic);
    }
    inline void addBeam(Beam* beam)
    {
      beams.push_back(beam);
    }
    /* By calling this function, you transfer ownership of the pointer to the
     * World.  The effect shall be deleted by the World when it is removed or
     * when the World is destructed */
    inline void addEffect(Effect* effect) {
      effects.push_back(effect);
      /* TODO: foreach (unit in effect's region) { effect.onUnitPresent(unit);} */
    }
    
    inline Player* getPlayerPtr(const PlayerID& id)
    {
      /* assert(id>=0); (Always true) */
      assert(id<players.size());
      return &players[id];
    }

    /* methods to perform game mechanics */
    void advanceGameState(Time timeToReach);
    void incrementGameState(void);
    void applyEntryExitEffects(
        LayeredUnit& unit,
        const Point<sint32> oldPosition,
        const Point<sint32> newPosition
      );
};

extern LIBSAKUSEN_SERVER_API CompleteWorld* world;
}
}

#endif // LIBSAKUSEN_SERVER__COMPLETEWORLD_H

