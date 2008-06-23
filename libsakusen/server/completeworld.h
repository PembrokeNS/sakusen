#ifndef LIBSAKUSEN_SERVER__COMPLETEWORLD_H
#define LIBSAKUSEN_SERVER__COMPLETEWORLD_H

#include "world.h"
#include "hash_list.h"
#include "player.h"
#include "ispatial.h"
#include "completemap.h"
#include "layeredunit.h"
#include "fuse.h"
#include "script.h"
#include "ballistic.h"
#include "beam.h"
#include "effect.h"

namespace sakusen {
namespace server {

/** \brief Represents the entire game state.
 *
 * CompleteWorld contains everything about the game state at any given instant,
 * including all the units, Effects, etc., the map, indexes into such things
 * and so forth.
 *
 * It also provides incrementState, which moves the game state forward one tick
 * in the 'correct' manner, and various methods for adding more objects to the
 * game, intended to be used by modules (e.g. adding a Ballistic when a weapon
 * gets fired).
 */
class LIBSAKUSEN_SERVER_API CompleteWorld : public World {
  /* noncopyable */
  public:
    /** Constructor which the server is expected to use */
    CompleteWorld(
        const MapTemplate& map,
        uint32 playMode, /* what mode of the map we are using */
        const std::vector<Player>& players,
        const ResourceInterface::Ptr&
      );
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

    FuseToken lastFuseToken;
    FuseQueue fuseQueue; /* the FuseQueue is a FIFO priority queue */
    u_map<FuseToken, Fuse::Ptr>::type fuseMap;
    u_set<FuseToken>::type removedFuses;

    std::vector<Script::Ptr> scripts;

    std::vector<Player> players;

    void applyEffect(
        const Ref<Effect>&,
        void (Effect::*)(const Ref<LayeredUnit>&)
      );
    hash_list<Effect, Bounded>::iterator processEffect(
        hash_list<Effect, Bounded>::iterator
      );
  public:
    /** \name Accessors. */
    //@{
    inline Map* getMap(void) { return map; }
    inline const Map* getMap(void) const { return map; }
    inline const CompleteMap* getCompleteMap(void) const { return map; }
    inline Ref<ISensorReturns> getISensorReturns(
        PlayerId player, SensorReturnsId id
      ) {
      assert(player < players.size());
      return players[player].getSensorReturns(id).cast<ISensorReturns>();
    }
    inline Ref<ICompleteUnit> getICompleteUnit(
        PlayerId player, UnitId unitId
      ) {
      assert(player < players.size());
      const u_map<UnitId, Ref<LayeredUnit> >::type& units =
        players[player].getUnits();
      u_map<UnitId, Ref<LayeredUnit> >::type::const_iterator it =
        units.find(unitId);
      if (it == units.end()) {
        return Ref<ICompleteUnit>();
      } else {
        return it->second;
      }
    }
    //@}
    
    Ref<LayeredUnit> addUnit(const LayeredUnit::Ptr& unit, PlayerId owner);
    void removeUnit(LayeredUnit*);
    
    /** \warning This has to be a non-const return for stuff that happens in
     * Player::checkSensorReturns to work, but don't abuse it.  In particular,
     * don't add or remove elements */
    inline hash_list<LayeredUnit, Bounded>& getUnits(void) { return units; }
    
    /** \brief Add a Ballistic to the world.
     *
     * This is the appropriate function to call from module code when creating
     * a Ballistic.  Transfers ownership of \p ballistic to this.
     */
    inline void addBallistic(Ballistic* ballistic)
    {
      ballistics.push_back(ballistic);
    }
    inline const hash_list<Ballistic>& getBallistics(void) const {
      return ballistics;
    }
    
    /** \brief Add a Beam to the world.
     *
     * This is the appropriate function to call from module code when creating
     * a Beam.  Transfers ownership of \p beam to this.
     */
    inline void addBeam(Beam* beam)
    {
      beams.push_back(beam);
    }

    /** \brief Add an Effect to the world.
     *
     * This is the appropriate function to call from module code when creating
     * an Effect.  Transfers ownership of \p effect to this.
     */
    inline void addEffect(Effect* effect) {
      newEffects.push(effect);
    }

    /** \brief Add a Fuse to the world.
     *
     * This is the appropriate function to call from module code when adding a
     * Fuse.
     */
    inline FuseToken addFuse(Fuse::Ptr fuse, Time expiryTime) {
      FuseToken token = ++lastFuseToken;
      if (fuseMap.count(token) || removedFuses.count(token)) {
        Fatal("FuseToken wraparound");
      }
      fuseQueue.push(FuseEntry(expiryTime, token));
      fuseMap[token] = fuse;
      return token;
    }

    /** \brief Remove a Fuse from the world.
     *
     * Removes an existing fuse, bsed on its FuseToken.
     */
    inline void removeFuse(FuseToken token) {
      u_map<FuseToken, Fuse::Ptr>::type::iterator fuseIt =
        fuseMap.find(token);
      if (fuseIt == fuseMap.end()) {
        Fatal("Removing nonexistant Fuse");
      }
      removedFuses.insert(token);
      fuseMap.erase(fuseIt);
    }

    inline ISpatial::ConstPtr getSpatialIndex() const { return spatialIndex; }
    
    /** \brief Get pointer to player of given id. */
    inline Player* getPlayerPtr(const PlayerId& id)
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

