#ifndef LIBSAKUSEN_SERVER__COMPLETEWORLD_H
#define LIBSAKUSEN_SERVER__COMPLETEWORLD_H

#include "world.h"
#include "hash_list.h"
#include "player.h"
#include "ispatial.h"
#include "completemap.h"
#include "layeredunit.h"
#include "fuse.h"
#include "ballistic.h"
#include "beam.h"
#include "effect.h"

namespace sakusen {
namespace server {

class LIBSAKUSEN_SERVER_API CompleteWorld : public World {
  /* noncopyable */
  public:
    CompleteWorld(
        const MapTemplate& map,
        uint32 playMode, /* what mode of the map we are using */
        const std::vector<Player>& players
      ); /* The constructor which the server is actually expected to use */
    ~CompleteWorld();
  private:
    CompleteMap* map; /* owned by this */
    hash_list<LayeredUnit, Bounded> units; /* this list includes subunits */
    hash_list<Ballistic> ballistics;
    hash_list<Beam> beams;
    hash_list<Effect, Bounded> effects;

    /** \brief Spatial index to the objects in the game universe
     * (including at least units and effects) */
    ISpatial::Ptr spatialIndex;
    
    /** New effects which have been added this tick and still have to have
     * their methods called
     *
     * Pointers owned by this.  Effects should not appear in this container and
     * effects above at the same time. */
    std::queue<Effect*> newEffects;
    FuseQueue fuseQueue; /* the FuseQueue is a FIFO priority queue */
    std::vector<Player> players;

    void applyEffect(
        const Ref<Effect>&,
        void (Effect::*)(const Ref<LayeredUnit>&)
      );
    hash_list<Effect, Bounded>::iterator processEffect(
        hash_list<Effect, Bounded>::iterator
      );
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
    inline Ref<ICompleteUnit> getICompleteUnit(
        PlayerID player, uint32 unitId
      ) {
      assert(player < players.size());
      const __gnu_cxx::hash_map<uint32, Ref<LayeredUnit> >& units =
        players[player].getUnits();
      __gnu_cxx::hash_map<uint32, Ref<LayeredUnit> >::const_iterator it =
        units.find(unitId);
      if (it == units.end()) {
        return Ref<ICompleteUnit>();
      } else {
        return it->second;
      }
    }
    
    Ref<LayeredUnit> addUnit(const LayeredUnit::Ptr& unit, PlayerID owner);
    void removeUnit(LayeredUnit*);
    
    /** \warning This has to be a non-const return for stuff that happens in
     * Player::checkSensorReturns to work, but don't abuse it.  In particular,
     * don't add or remove elements */
    inline hash_list<LayeredUnit, Bounded>& getUnits(void) { return units; }
    
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

    inline ISpatial::ConstPtr getSpatialIndex() const { return spatialIndex; }
    
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

