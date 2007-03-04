#ifndef LIBSAKUSEN_SERVER__COMPLETEWORLD_H
#define LIBSAKUSEN_SERVER__COMPLETEWORLD_H

#include "world.h"
#include "hash_list.h"
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
    hash_list<LayeredUnit> units; /* this list includes subunits */
    hash_list<Ballistic> ballistics;
    hash_list<Beam> beams; /* owned by this */
    
    std::list<Effect*> effects; /* owned by this */
    /** New effects which have been added this tick and still have to have
     * their methods called
     *
     * Pointers owned by this.  Effects should not appear in this container and
     * effects above at the same time. */
    std::queue<Effect*> newEffects;
    FuseQueue fuseQueue; /* the FuseQueue is a FIFO priority queue */
    std::vector<Player> players;

    void applyEffect(Effect*, void (Effect::*)(const Ref<LayeredUnit>&));
    std::list<Effect*>::iterator processEffect(std::list<Effect*>::iterator);
  public:
    /* accessors */
    inline Map* getMap(void) { return map; }
    inline const Map* getMap(void) const { return map; }
    inline const CompleteMap* getCompleteMap(void) const { return map; }
    inline Ref<ISensorReturns> getISensorReturns(
        PlayerID player, SensorReturnsID id
      ) {
      assert(player < players.size());
      return players[player].getSensorReturns(id).cast<ISensorReturns>();
    }
    
    void addUnit(const LayeredUnit::Ptr& unit, PlayerID owner);
    void removeUnit(LayeredUnit*);
    
    /** \warning This has to be a non-const return for stuff that happens in
     * Player::checkSensorReturns to work, but don't abuse it.  In particular,
     * don't add or remove elements */
    inline hash_list<LayeredUnit>& getUnits(void) { return units; }
    
    inline void addBallistic(Ballistic* ballistic)
    {
      ballistics.push_back(ballistic);
    }
    inline const hash_list<Ballistic>& getBallistics(void) const {
      return ballistics;
    }
    
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
        const Ref<LayeredUnit>& unit,
        const Point<sint32>& oldPosition,
        const Point<sint32>& newPosition
      );

    void save(OArchive&) const;
    static void loadNew(IArchive&, ResourceInterface::Ptr);
};

extern LIBSAKUSEN_SERVER_API CompleteWorld* world;

}}

#endif // LIBSAKUSEN_SERVER__COMPLETEWORLD_H

