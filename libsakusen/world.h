#ifndef WORLD_H
#define WORLD_H

#include "libsakusen-global.h"
#include "unit.h"
#include "ballistic.h"
#include "beam.h"
#include "player.h"
#include "map.h"
#include "effect.h"
#include "universe.h"
#include "action.h"

class Region;

class LIBSAKUSEN_API World {
  private:
    World(const World& copy); /* Shouldn't use the copy constructor - mayhem
                                 will ensue because it will invalidate all
                                 pointers to units */
  public:
    World(); /* Constructs a very dull World with nothing in it */
    World(Map* map,
        uint32 playMode, /* what mode of the map we are using */
        const std::vector<Player>& players
      ); /* The constructor which the server is actually expected to use */
    ~World();
  private:
    Map* map;
    std::list<Unit> units; /* this list includes subunits */
    std::list<Ballistic> ballistics;
    std::list<Beam> beams;
    std::list<Effect*> effects;
    ActionQueue actionQueue; /* the ActionQueue is a FIFO priority queue */
    std::vector<Player> players;
    Time timeNow; /* Current game time */
  public:
  /* accessors */
  inline const Map* getMap(void) const { return map; }
  inline const Universe* getUniverse(void) const { return map->getUniverse(); }
  inline void addUnit(const Unit& unit, PlayerID owner)
  {
    units.push_back(unit);
    units.back().changeOwner(owner, changeOwnerReason_created);
  }
  inline void addBallistic(const Ballistic& ballistic)
  {
    ballistics.push_back(ballistic);
  }
  inline void addBeam(const Beam& beam)
  {
    beams.push_back(beam);
  }
  /* By calling this function, you transfer ownership of the pointer to the
   * World.  The effect shall be deleted by the World when it is removed or
   * when the World is destructed */
  inline void addEffect(Effect* effect)
  {
    effects.push_back(effect);
    /* TODO: foreach (unit in effect's region) { effect.onUnitPresent(unit);} */
  }
  /* Calling this function does *not* transfer ownership of the pointer to the
   * World.  The action shall *not* be deleted by the World. */
  inline void enqueueAction(Action* action)
  {
    actionQueue.push(action);
  }
  inline const Time& getTimeNow(void) { return timeNow; }

  /* methods to convert between pointers and internal or external reps */
  inline const UnitType* getUnitTypePtr(const UnitTypeID id) const
  {
    return getUniverse()->getUnitTypePtr(id);
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
      Unit& unit,
      const Point<sint32> oldPosition,
      const Point<sint32> newPosition);
};

extern LIBSAKUSEN_API World* world;
#endif

