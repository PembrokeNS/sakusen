#include "completeworld.h"
#include "effect.h"
#include "mapplaymode.h"
#include "region-methods.h"
#include "map-methods.h"
#include "layeredunit-methods.h"

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
  assert(ballisticRefs.empty());
  
  /* Free memory for beams */
  while (!beams.empty()) {
    invalidateRefs(beams.front());
    delete beams.front();
    beams.pop_front();
  }
  assert(beamRefs.empty());
  
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

/** \todo Use cunningness to reduce the code duplication in the following
 * methods */
void CompleteWorld::invalidateRefs(const MaskedPtr<LayeredUnit>& id)
{
  pair<
      __gnu_cxx::hash_multimap<
          MaskedPtr<LayeredUnit>, Ref<LayeredUnit>*, MaskedPtrHash<LayeredUnit>
        >::iterator,
      __gnu_cxx::hash_multimap<
          MaskedPtr<LayeredUnit>, Ref<LayeredUnit>*, MaskedPtrHash<LayeredUnit>
        >::iterator
    > refRange = unitRefs.equal_range(id);
  
  for (__gnu_cxx::hash_multimap<
          MaskedPtr<LayeredUnit>, Ref<LayeredUnit>*, MaskedPtrHash<LayeredUnit>
        >::iterator refIt = refRange.first; refIt != refRange.second;
      ++refIt) {
    refIt->second->invalidate();
  }
  unitRefs.erase(refRange.first, refRange.second);
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

void CompleteWorld::invalidateRefs(const MaskedPtr<Beam>& id)
{
  pair<
      __gnu_cxx::hash_multimap<
          MaskedPtr<Beam>, Ref<Beam>*, MaskedPtrHash<Beam>
        >::iterator,
      __gnu_cxx::hash_multimap<
          MaskedPtr<Beam>, Ref<Beam>*, MaskedPtrHash<Beam>
        >::iterator
    > refRange = beamRefs.equal_range(id);
  
  for (__gnu_cxx::hash_multimap<
          MaskedPtr<Beam>, Ref<Beam>*, MaskedPtrHash<Beam>
        >::iterator refIt = refRange.first; refIt != refRange.second;
      ++refIt) {
    refIt->second->invalidate();
  }
  beamRefs.erase(refRange.first, refRange.second);
}

void CompleteWorld::applyEffect(
    Effect* effect,
    void (Effect::*method)(Ref<LayeredUnit>&)
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
  for (list<LayeredUnit>::iterator unit = units.begin();
      unit != units.end(); ++unit) {
    if (effect->getRegion().contains(unit->getStatus())) {
      affectedUnits.push(&*unit);
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

void CompleteWorld::addUnit(const LayeredUnit& unit, PlayerID owner) {
  units.push_back(unit);
  std::list<LayeredUnit>::iterator newIt = units.end();
  --newIt;
  assert(0 == unitIts.count(&*newIt));
  unitIts[&*newIt] = newIt;
  units.back().changeOwner(owner, changeOwnerReason_created);
}

/** Removes \p unit from the World.
 *
 * \warning This is for internal use only.  To kill a unit, use the
 * LayeredUnit::kill method */
void CompleteWorld::removeUnit(LayeredUnit* unit)
{
  unit->changeOwner(0, changeOwnerReason_destroyed);
  __gnu_cxx::hash_map<
        MaskedPtr<LayeredUnit>,
        std::list<LayeredUnit>::iterator,
        MaskedPtrHash<LayeredUnit>
      >::iterator unitItIt = unitIts.find(unit);
  if (unitItIt == unitIts.end()) {
    Fatal("removing unit not present");
  }
  invalidateRefs(unit);
  units.erase(unitItIt->second);
  unitIts.erase(unitItIt);
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

  /* process Ballistics */
  for (std::list<Ballistic*>::iterator k = ballistics.begin();
      k != ballistics.end(); ) {
    /* Check the Ballistic for collisions */
    if ((*k)->resolveIntersections()) {
      invalidateRefs(*k);
      delete *k;
      k = ballistics.erase(k);
    } else {
      ++k;
    }
  }

  /* process Beams */
  for (std::list<Beam*>::iterator beam = beams.begin();
      beam != beams.end(); ) {
    /* Check for Beam for removal */
    if ((*beam)->onRemovalTest()) {
      invalidateRefs(*beam);
      delete *beam;
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
  for (std::list<LayeredUnit>::iterator j=units.begin(); j!=units.end(); j++) {
    j->clearDirty();
  }
}

void CompleteWorld::applyEntryExitEffects(
    LayeredUnit* unit,
    const Point<sint32>& oldPosition,
    const Point<sint32>& newPosition
  )
{
  Ref<LayeredUnit> unitRef(unit);

  /** \todo make this more efficient by storing the data differently */
  for (std::list<Effect*>::iterator i=effects.begin(); i!=effects.end(); i++) {
    if ((*i)->getRegion().contains(oldPosition) &&
        !(*i)->getRegion().contains(newPosition)) {
      (*i)->onUnitLeave(unitRef);
    }
    if (!(*i)->getRegion().contains(oldPosition) &&
        (*i)->getRegion().contains(newPosition)) {
      (*i)->onUnitEnter(unitRef);
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

void CompleteWorld::registerRef(Ref<LayeredUnit>* ref)
{
  MaskedPtr<LayeredUnit> id(**ref);
  pair<MaskedPtr<LayeredUnit>, Ref<LayeredUnit>*> item(id, ref);
  unitRefs.insert(item);
}

void CompleteWorld::unregisterRef(Ref<LayeredUnit>* ref)
{
  pair<
      __gnu_cxx::hash_multimap<
          MaskedPtr<LayeredUnit>, Ref<LayeredUnit>*, MaskedPtrHash<LayeredUnit>
        >::iterator,
      __gnu_cxx::hash_multimap<
          MaskedPtr<LayeredUnit>, Ref<LayeredUnit>*, MaskedPtrHash<LayeredUnit>
        >::iterator
    > refRange = unitRefs.equal_range(**ref);
  
  /* This becomes slow if there are many Refs to the same LayeredUnit object.
   * If that is a problem we could ID the refs too and have O(1) lookups for
   * them, but it's unlikely to be of help overall */
  for (__gnu_cxx::hash_multimap<
          MaskedPtr<LayeredUnit>, Ref<LayeredUnit>*, MaskedPtrHash<LayeredUnit>
        >::iterator refIt = refRange.first; refIt != refRange.second;
      ++refIt) {
    if (refIt->second == ref) {
      unitRefs.erase(refIt);
      return;
    }
  }
  Fatal("tried to unregister a not-registered Ref");
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

}} //End Namespaces

