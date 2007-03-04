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
  /* Free memory for effects */
  while (!effects.empty()) {
    delete effects.front();
    effects.pop_front();
  }

  /* Clear units lest they outlive the sensor returns from them */
  units.clear();
  
  /** \todo Something about freeing Fuse memory. */

  delete map;
  map = NULL;

  world = NULL;
}

void CompleteWorld::applyEffect(
    Effect* effect,
    void (Effect::*method)(const Ref<LayeredUnit>&)
  )
{
  /** \bug Using point-to-member-function here because it has the minimal code
   * duplication.  This is possibly too slow.  To speed it up this method could
   * be inlined, so that \p method becomes a constant */
  
  /** \todo Speed this up by storing data sensibly */

  /* We can't just iterate over the units and mess with them directly, because
   * if they get destroyed then the itereator will be invalidated, and
   * segfaults ensue.  Instead, we first collect Refs to all the affected
   * units, and then process them all. */
  queue<Ref<LayeredUnit> > affectedUnits;
  for (hash_list<LayeredUnit>::iterator unit = units.begin();
      unit != units.end(); ++unit) {
    if (effect->getRegion()->contains((*unit)->getStatus())) {
      affectedUnits.push(*unit);
    }
  }
  
  while (!affectedUnits.empty()) {
    Ref<LayeredUnit> unit = affectedUnits.front();
    affectedUnits.pop();
    if (unit.isValid()) {
      (effect->*method)(unit);
    }
  }
}

/** \brief Perform regular tasks for an Effect
 *
 * \param effect Iterator from container effects pointing to the effect to
 * process.
 *
 * \return Iterator pointing to the next Effect to process
 *
 * This method will call the Effect::onUnitPresent method, and then test the
 * Effect to see whether it should be removed */
list<Effect*>::iterator CompleteWorld::processEffect(
    list<Effect*>::iterator effect
  )
{
  /* Call the 'present' method on Units */
  applyEffect(*effect, &Effect::onUnitPresent);

  if ((*effect)->onRemovalTest()) {
    /* We need to remove the effect */
    applyEffect(*effect, &Effect::onUnitLeave);
    delete *effect;
    return effects.erase(effect);
  } else {
    return ++effect;
  }
}

void CompleteWorld::addUnit(const LayeredUnit::Ptr& unit, PlayerID owner) {
  units.push_back(unit);
  units.back()->changeOwner(owner, changeOwnerReason_created);
}

/** Removes \p unit from the World.
 *
 * \warning This is for internal use only.  To kill a unit, use the
 * LayeredUnit::kill method */
void CompleteWorld::removeUnit(LayeredUnit* unit)
{
  unit->changeOwner(0, changeOwnerReason_destroyed);
  hash_list<LayeredUnit>::iterator it = units.find(unit);
  if (it == units.end()) {
    Fatal("removing unit not present");
  }
  units.erase(it);
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
  for (hash_list<LayeredUnit>::iterator j=units.begin(); j!=units.end(); j++) {
    /* this moves the unit according to its
     * current speed.  This method also responsible for
     * ensuring that all appropriate client messages caused by the things
     * that happen in it are sent */
    (*j)->incrementState(timeNow);
  }

  /* process Ballistics */
  for (hash_list<Ballistic>::iterator k = ballistics.begin();
      k != ballistics.end(); ) {
    /* Check the Ballistic for collisions */
    if ((*k)->resolveIntersections()) {
      k = ballistics.erase(k);
    } else {
      ++k;
    }
  }

  /* process Beams */
  for (hash_list<Beam>::iterator beam = beams.begin(); beam != beams.end(); ) {
    /* Check for Beam for removal */
    if ((*beam)->onRemovalTest()) {
      beam = beams.erase(beam);
    } else {
      /* Check the Beam for intersections */
      (*beam)->resolveIntersections();
      ++beam;
    }
  }
  
  /* Next we deal with all effects that already exist (as opposed to newly
   * created ones, which are dealt with in the loop below) */
  for (list<Effect*>::iterator effect = effects.begin();
      effect != effects.end(); ) {
    effect = processEffect(effect);
  }
  
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
    
    while (!newEffects.empty()) {
      effectHappened = true;

      Effect* effect = newEffects.front();
      newEffects.pop();
      effects.push_back(effect);
      /* construct an iterator pointing at this new effect */
      list<Effect*>::iterator effectIt = effects.end();
      --effectIt;
      /* Units inside the effect get the 'entry' method called... */
      applyEffect(effect, &Effect::onUnitEnter);
      /* ...and then proceed as if it was a bog standard Effect */
      processEffect(effectIt);
    }
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
  for (hash_list<LayeredUnit>::iterator j=units.begin(); j!=units.end(); j++) {
    (*j)->clearDirty();
  }
}

void CompleteWorld::applyEntryExitEffects(
    const Ref<LayeredUnit>& unit,
    const Point<sint32>& oldPosition,
    const Point<sint32>& newPosition
  )
{
  /** \todo make this more efficient by storing the data differently */
  for (std::list<Effect*>::iterator i=effects.begin(); i!=effects.end(); i++) {
    if ((*i)->getRegion()->contains(oldPosition) &&
        !(*i)->getRegion()->contains(newPosition)) {
      (*i)->onUnitLeave(unit);
    }
    if (!(*i)->getRegion()->contains(oldPosition) &&
        (*i)->getRegion()->contains(newPosition)) {
      (*i)->onUnitEnter(unit);
    }
  }
}

void CompleteWorld::save(OArchive& /*archive*/) const
{
  /** \bug This is not implemented with good reason, because it is likely to
   * change dramatically as the game engine develops */
  Fatal("not implemented");
}

void CompleteWorld::loadNew(
    IArchive& /*archive*/,
    ResourceInterface::Ptr /*resourceInterface*/
  )
{
  /** \bug This is not implemented with good reason, because it is likely to
   * change dramatically as the game engine develops */
  Fatal("not implemented");
}

LIBSAKUSEN_SERVER_API CompleteWorld* world = NULL;

}} //End Namespaces

