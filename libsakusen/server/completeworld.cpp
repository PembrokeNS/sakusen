#include "completeworld.h"
#include "effect.h"
#include "mapplaymode.h"
#include "region-methods.h"
#include "map-methods.h"

#include <list>
#include <vector>

using namespace std;
using namespace __gnu_cxx;

namespace sakusen{
namespace server{

CompleteWorld::CompleteWorld(
    const MapTemplate& m,
    uint32 mode,
    const std::vector<Player>& p
  ) :
  World(m.getUniverse()),
  map(Map::newMap<CompleteMap>(m)),
  units(),
  fuseQueue(),
  players(p)
{
  /* yes, these first two variables hold the same number. But it makes
   * type-checking easier, and the optimizer will deal with them.
   */
  size_t vectorSize;
  uint32 numPlayers;
  uint8 i;

  /* Some sanity checks */
  if (world) {
    Debug("World constructed when world != NULL");
  }
  world = this;

  if (players.empty()) {
    Fatal("A World must have at least a neutral player.");
  }
  const MapPlayMode* playMode = &m.getPlayModes()[mode];
  
  vectorSize = players.size();
  numPlayers = static_cast<uint32>(vectorSize);
  if (!playMode->acceptableNumberOfPlayers(numPlayers)) {
    Fatal("Number of players not acceptable for this map and mode");
  }
  /* TODO: more sanity checks */
  
  /* assign player ids */
  
  for (i=0; i < numPlayers; i--) {
    players[i].setPlayerId(i);
  }
  
  /* put units on the map as specified in map */
  for (i=0; i < numPlayers; i--) {
    std::vector<UnitTemplate> playersUnits =
      playMode->getPlayer(i).getUnits();
    for (std::vector<UnitTemplate>::iterator unit = playersUnits.begin();
        unit != playersUnits.end(); unit++) {
      LayeredUnit::spawn(i, *unit);
    }
  }
}

CompleteWorld::~CompleteWorld()
{
  /* Free memory for effects */
  while (!effects.empty()) {
    delete effects.front();
    effects.pop_front();
  }
  
  /* TODO: something about freeing Fuse memory. */

  delete map;
  map = NULL;

  world = NULL;
}

/** \brief Advances the game state to the specified time
 *
 * This method will take into
 * account all the orders received from the players, performing all appropriate
 * actions, implementing all appropriate effects, and accumulating all data to
 * be sent out to clients, saved to replay, etc.
 *
 * However, this method is not intended to be used during normal play, because
 * a server is likely to want to do other things each tick, such as poll the
 * network.  It is appropriate for circumstances when you wish to run the game
 * as fast as possible, such as when doing load testing or seeking in a replay.
 */
void CompleteWorld::advanceGameState(Time timeToReach)
{
  if (timeToReach < timeNow)
  {
    Debug("Attempting to move the game backwards in time");
  }
  while (timeNow < timeToReach)
  {
    incrementGameState();
  }
}

void CompleteWorld::incrementGameState(void)
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
  for (std::list<LayeredUnit>::iterator j=units.begin(); j!=units.end(); j++) {
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
    /* check fuses */
    FuseEntry fuseEntry;
    while (!fuseQueue.empty() &&
        (fuseEntry = fuseQueue.top()).time <= timeNow) {
      fuseQueue.pop();
      if (fuseEntry.time < timeNow) {
        Debug("Fuse occurance time missed");
      }
      /* perform action, delete if necessary */
      if (fuseEntry.fuse->expire(fuseEntry.token))
      {
        delete fuseEntry.fuse;
      }
    }

    effectHappened = false;
    
    /* TODO: Do something with effects that calls the Effect::unitPresent
     * method.  Also provide for effects coming to an end (and delete them when
     * that happens!) */
  } while (effectHappened);
    /* We need to keep repeating this loop until no Effects happen because
     * Effects can cause Fuses which can cause Effects
     * again, all in the course of one game cycle, not to mention
     * the fact that dying units also cause things, etc. */
  
  /* create the appropriate sensor returns and send them to clients,
   * as well as informing them of the removal of old ones */
  for (vector<Player>::iterator player = players.begin();
      player != players.end(); ++player) {
    player->checkSensorReturns();
  }
  
  /* TODO: send to clients:
   * - newly explored/altered portions of the map */

  /* Once everything has happened that can happen, we induce all units to send
   * to clients any information about their changedness */
  for (std::list<LayeredUnit>::iterator j=units.begin(); j!=units.end(); j++) {
    j->clearDirty();
  }
}

void CompleteWorld::applyEntryExitEffects(
    LayeredUnit& unit,
    const Point<sint32> oldPosition,
    const Point<sint32> newPosition
  )
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

LIBSAKUSEN_SERVER_API CompleteWorld* world = NULL;

}
} //End Namespaces

