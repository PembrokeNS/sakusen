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
  /** \todo More sanity checks */
  
  /* assign player ids */
  
  for (i=0; i < numPlayers; i++) {
    players[i].setPlayerId(i);
  }
  
  /* put units on the map as specified in map */
  for (i=0; i < numPlayers; i++) {
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
  /* Free memory for ballistics */
  while (!ballistics.empty()) {
    invalidateRefs(ballistics.front());
    delete ballistics.front();
    ballistics.pop_front();
  }
  
  /* Free memory for beams */
  while (!beams.empty()) {
    delete beams.front();
    beams.pop_front();
  }
  
  /* Free memory for effects */
  while (!effects.empty()) {
    delete effects.front();
    effects.pop_front();
  }
  
  /** \todo Something about freeing Fuse memory. */

  delete map;
  map = NULL;

  world = NULL;
}

void CompleteWorld::invalidateRefs(const MaskedPtr<Ballistic>& id)
{
  pair<
      __gnu_cxx::hash_multimap<
          MaskedPtr<Ballistic>, Ref<Ballistic>*, MaskedPtrHash<Ballistic>
        >::iterator,
      __gnu_cxx::hash_multimap<
          MaskedPtr<Ballistic>, Ref<Ballistic>*, MaskedPtrHash<Ballistic>
        >::iterator
    > refRange = ballisticRefs.equal_range(id);
  
  for (__gnu_cxx::hash_multimap<
          MaskedPtr<Ballistic>, Ref<Ballistic>*, MaskedPtrHash<Ballistic>
        >::iterator refIt = refRange.first; refIt != refRange.second;
      ++refIt) {
    refIt->second->invalidate();
  }
  ballisticRefs.erase(refRange.first, refRange.second);
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
  for (std::list<Ballistic*>::iterator k = ballistics.begin();
      k != ballistics.end(); ) {
    /* Check the ballistic for collisions */
    if ((*k)->resolveIntersections()) {
      invalidateRefs(*k);
      delete *k;
      k = ballistics.erase(k);
    } else {
      ++k;
    }
  }

  /** \todo something to deal with beams */
  
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
    
    /** \todo Do something with effects that calls the Effect::unitPresent
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
  
  /** \todo send to clients:
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
  /** \todo make this more efficient by storing the data differently */
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

void CompleteWorld::registerRef(Ref<ISensorReturns>* ref)
{
  players[(*ref)->getSenserOwner()].registerRef(ref);
}

void CompleteWorld::unregisterRef(Ref<ISensorReturns>* ref)
{
  players[(*ref)->getSenserOwner()].unregisterRef(ref);
}

void CompleteWorld::registerRef(Ref<Ballistic>* ref)
{
  MaskedPtr<Ballistic> id(**ref);
  pair<MaskedPtr<Ballistic>, Ref<Ballistic>*> item(id, ref);
  ballisticRefs.insert(item);
}

void CompleteWorld::unregisterRef(Ref<Ballistic>* ref)
{
  pair<
      __gnu_cxx::hash_multimap<
          MaskedPtr<Ballistic>, Ref<Ballistic>*, MaskedPtrHash<Ballistic>
        >::iterator,
      __gnu_cxx::hash_multimap<
          MaskedPtr<Ballistic>, Ref<Ballistic>*, MaskedPtrHash<Ballistic>
        >::iterator
    > refRange = ballisticRefs.equal_range(**ref);
  
  /* This becomes slow if there are many Refs to the same Ballistic object.
   * If that is a problem we could ID the refs too and have O(1) lookups for
   * them, but it's unlikely to be of help overall */
  for (__gnu_cxx::hash_multimap<
          MaskedPtr<Ballistic>, Ref<Ballistic>*, MaskedPtrHash<Ballistic>
        >::iterator refIt = refRange.first; refIt != refRange.second;
      ++refIt) {
    if (refIt->second == ref) {
      ballisticRefs.erase(refIt);
      return;
    }
  }
  Fatal("tried to unregister a not-registered Ref");
}

LIBSAKUSEN_SERVER_API CompleteWorld* world = NULL;

}
} //End Namespaces

