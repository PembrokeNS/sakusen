#include "world.h"
#include "effect.h"
#include "mapplaymode.h"

#include <list>
#include <vector>

World::World() :
  map(NULL),
  units(),
  ballistics(),
  beams(),
  effects(),
  actionQueue(),
  players(),
  timeNow(0)
{
  Debug("World::World() called (probably a bad thing)");
}

World::World(Map* m,
    uint32 mode,
    const std::vector<Player>& p
  ) :
  map(m),
  units(),
  ballistics(),
  beams(),
  effects(),
  actionQueue(),
  players(p),
  timeNow(0)
{
  /* Some sanity checks */
  if (world) {
    Debug("World constructed when world != NULL");
  }
  world = this;

  if (players.empty()) {
    Fatal("A World must have at least a neutral player.");
  }
  const MapPlayMode* playMode = &map->getPlayMode(mode);
  
  if (!playMode->acceptableNumberOfPlayers(players.size())) {
    Fatal("Number of players not acceptable for this map and mode");
  }
  /* TODO: more sanity checks */
  
  /* assign player ids */
  sint32 i;
  
  for (i=players.size()-1; i>=0; i--) {
    players[i].setPlayerId(i);
  }
  
  /* put units on the map as specified in map */
  for (i=players.size()-1; i>=0; i--) {
    std::vector<Unit> playersUnits =
      playMode->getPlayersUnits(players.size(), i);
    for (std::vector<Unit>::iterator unit = playersUnits.begin();
        unit != playersUnits.end(); unit++) {
      units.push_back(*unit);
      units.back().changeOwner(i, changeOwnerReason_created);
    }
  }
}

World::~World()
{
  /* Free memory for effects */
  while (!effects.empty()) {
    delete effects.front();
    effects.pop_front();
  }

  /* Possibly we should free memory for actions, but I suspect not, and for the
   * moment I have declared that it shall not be so. */
}

/* Advances the game state to the specified time, taking into
 * account all the orders received from the players, performing all appropriate
 * actions, implementing all appropriate effects, and accumulating all data to
 * be sent out to clients, saved to replay, etc.
 */
void World::advanceGameState(Time timeToReach)
{
  /* What needs to be done:
   * - apply incoming orders
   * - apply active effects
   * - perform actions
   * - do game physics
   * - send to clients:
   *   - game state changes
   *   - new/updated SensorReturns
   *   - newly explored/altered portions of the map
   * Furthermore, all of the above needs to be done in the correct order,
   * to ensure that everything works out consistently.  In other words, we
   * can't do all actions and then all effects, because some effects might
   * cancel some actions, etc.
   *
   * This is all simpler if we advance only one unit of game time, rather than
   * an arbitrary number of units, so for the moment that is all I implement
   * (in World::incrementGameState), which is then called by this function.
   */
  if (timeToReach < timeNow)
  {
    Debug("Attempting to move the game backwards in time");
  }
  while (timeNow < timeToReach)
  {
    incrementGameState();
  }
}

void World::incrementGameState(void)
{
  bool effectHappened;
  
  /* sanity check */
  assert(world == this);
  
  /* apply incoming orders */
  for (std::vector<Player>:: iterator player = players.begin();
      player != players.end(); player++) {
    player->applyIncomingOrders();
  }
  
  /* advance game time */
  timeNow++;
  
  /* process units */
  for (std::list<Unit>::iterator j=units.begin(); j!=units.end(); j++) {
    /* this moves the unit according to its
     * current speed.  This method also responsible for
     * ensuring that all appropriate client messages caused by the things
     * that happen in it are sent */
    j->incrementState(timeNow);
  }

  /* process ballistics */
  for (std::list<Ballistic>::iterator k=ballistics.begin();
      k!=ballistics.end(); k++) {
    /* TODO: collision detection. */
  }

  /* TODO: something to deal with beams */
  
  do {
    /* perform actions */
    const Action* action;
    while (!actionQueue.empty() &&
        (action = actionQueue.top())->getTime() <= timeNow) {
      actionQueue.pop();
      if (action->getTime() < timeNow) {
        Debug("Action occurance time missed");
      }
      /* perform action, replicate and tell clients */
      action->act();
    }

    effectHappened = false;
    
    /* TODO: Do something with effects that calls the Effect::unitPresent
     * method.  Also provide for effects coming to an end (and delete them when
     * that happens!) */
  } while (effectHappened);
    /* We need to keep repeating this loop until no Effects happen because
     * Effects can cause Actions which can cause Effects
     * again, all in the course of one game cycle, not to mention
     * the fact that dying units also cause actions, etc. */

  /* game state changes caused by all of this are generally sent to clients by
   * the innermost relevant method. */
  
  /* TODO: create the appropriate sensor returns and send them to clients,
   * as well as informing them of the removal of old ones */
  
  /* send to clients:
   * - newly explored/altered portions of the map */
}

void World::applyEntryExitEffects(
    Unit& unit,
    const Point<sint32> oldPosition,
    const Point<sint32> newPosition)
{
  /* TODO: make this more efficient by storing the data differently */
  for (std::list<Effect*>::iterator i=effects.begin(); i!=effects.end(); i++) {
    if ((*i)->getRegion().contains(oldPosition) &&
        !(*i)->getRegion().contains(newPosition)) {
      (*i)->onUnitLeave(unit);
    }
    if (!(*i)->getRegion().contains(oldPosition) &&
        (*i)->getRegion().contains(newPosition)) {
      (*i)->onUnitEnter(unit);
    }
  }
}

LIBSAKUSEN_API World* world = NULL;

