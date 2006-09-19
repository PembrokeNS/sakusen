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
    __gnu_cxx::hash_map<
        MaskedPtr<LayeredUnit>,
        std::list<LayeredUnit>::iterator,
        MaskedPtrHash<LayeredUnit>
      > unitIts;
    __gnu_cxx::hash_multimap<
        MaskedPtr<LayeredUnit>,
        Ref<LayeredUnit>*,
        MaskedPtrHash<LayeredUnit>
      > unitRefs; /* not owned by this */
    
    std::list<Ballistic*> ballistics; /* owned by this */
    __gnu_cxx::hash_multimap<
        MaskedPtr<Ballistic>,
        Ref<Ballistic>*,
        MaskedPtrHash<Ballistic>
      > ballisticRefs; /* not owned by this */
    
    std::list<Beam*> beams; /* owned by this */
    __gnu_cxx::hash_multimap<
        MaskedPtr<Beam>,
        Ref<Beam>*,
        MaskedPtrHash<Beam>
      > beamRefs; /* not owned by this */
    
    std::list<Effect*> effects; /* owned by this */
    /** New effects which have been added this tick and still have to have
     * their methods called
     *
     * Pointers owned by this.  Effects should not appear in this container and
     * effects above at the same time. */
    std::queue<Effect*> newEffects;
    FuseQueue fuseQueue; /* the FuseQueue is a FIFO priority queue */
    std::vector<Player> players;

    void invalidateRefs(const MaskedPtr<LayeredUnit>& id);
    void invalidateRefs(const MaskedPtr<Ballistic>& id);
    void invalidateRefs(const MaskedPtr<Beam>& id);
    void applyEffect(Effect*, void (Effect::*)(Ref<LayeredUnit>&));
    std::list<Effect*>::iterator processEffect(std::list<Effect*>::iterator);
  public:
    /* accessors */
    inline Map* getMap(void) { return map; }
    inline const Map* getMap(void) const { return map; }
    inline const CompleteMap* getCompleteMap(void) const { return map; }
    inline ISensorReturns* getISensorReturns(
        PlayerID player, SensorReturnsID id
      ) {
      assert(player < players.size());
      return players[player].getSensorReturns(id);
    }
    
    void addUnit(const LayeredUnit& unit, PlayerID owner);
    void removeUnit(LayeredUnit*);
    inline std::list<LayeredUnit>& getUnits(void) { return units; }
    
    inline void addBallistic(Ballistic* ballistic)
    {
      ballistics.push_back(ballistic);
    }
    inline std::list<Ballistic*>& getBallistics(void) { return ballistics; }
    
    inline void addBeam(Beam* beam)
    {
      beams.push_back(beam);
    }
    /* By calling this function, you transfer ownership of the pointer to the
     * World.  The effect shall be deleted by the World when it is removed or
     * when the World is destructed */
    inline void addEffect(Effect* effect) {
      newEffects.push(effect);
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
        LayeredUnit* unit,
        const Point<sint32>& oldPosition,
        const Point<sint32>& newPosition
      );

    /* methods for reference management */
    void registerRef(Ref<ISensorReturns>* ref);
    void unregisterRef(Ref<ISensorReturns>* ref);
    void registerRef(Ref<LayeredUnit>* ref);
    void unregisterRef(Ref<LayeredUnit>* ref);
    void registerRef(Ref<Ballistic>* ref);
    void unregisterRef(Ref<Ballistic>* ref);
};

extern LIBSAKUSEN_SERVER_API CompleteWorld* world;

}
}

#endif // LIBSAKUSEN_SERVER__COMPLETEWORLD_H

