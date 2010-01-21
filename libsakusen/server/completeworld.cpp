#include "completeworld.h"

#include "effect.h"
#include "mapplaymode.h"
#include "region-methods.h"
#include "map-methods.h"
#include "naivespatial.h"

#include <list>
#include <vector>
#include <iterator>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>

using namespace std;

namespace sakusen{
namespace server{

CompleteWorld::CompleteWorld(
    const MapTemplate& m,
    uint32 mode,
    const std::vector<Player>& p,
    const ResourceInterface::Ptr& resourceInterface
  ) :
  World(m.getUniverse()),
  map(Map::newMap<CompleteMap>(m)),
  units(),
  lastFuseToken(static_cast<FuseToken>(-1)),
  fuseQueue(),
  players(p),
  random(),
  randomDisplacementField(
      random,
      m.getUniverse()->getLogMinSpatialNoiseScale(),
      m.getUniverse()->getLogMaxSpatialNoiseScale(),
      3 /** \bug Random constant (minTemporalNoiseScale) */,
      map->volume()
    )
{
  /** \todo We are default-constructing random for testing purposes.  At some
   * point it should be seeded from a high-quality random source (and probably
   * regularly reseeded). */

  /* yes, these first two variables hold the same number. But it makes
   * type-checking easier, and the optimizer will deal with them.
   */
  size_t vectorSize;
  uint32 numPlayers;

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
  
  /* check player ids */
  for (PlayerId i; i < numPlayers; ++i) {
    if (!i.valid()) {
      Fatal("too many players");
    }
    if (players[i].getId() != i) {
      Fatal("player id mismatch");
    }
  }

  /** \todo More sanity checks */

  /* register the spatial index so that it gets filled with units and effects
   * */
  spatialIndex.reset(new NaiveSpatial());/** \todo Support other choices */
  effects.registerIndex(IIndex<Bounded>::Ptr(spatialIndex));
  units.registerIndex(IIndex<Bounded>::Ptr(spatialIndex));

  /* Register the scipt of the Universe */
  Script* (*universeScriptFunction)();
  ResourceSearchResult result;
  boost::tie(universeScriptFunction, result) =
    resourceInterface->symbolSearch<Script* (*)()>(
        universe->getScriptModule(), universe->getScriptFunction()
      );
  switch (result) {
    case resourceSearchResult_success:
      {
        /* Everything is OK */
        Script::Ptr universeScript(universeScriptFunction());
        scripts.push_back(universeScript);
      }
      break;
    default:
      /* There is a problem */
      throw ResourceDeserializationExn(
          universe->getScriptModule(), result, resourceInterface->getError()
        );
  }

  /* Init player-global data for each player */
  PlayerData* (*playerDataFunction)();
  boost::tie(playerDataFunction, result) =
    resourceInterface->symbolSearch<PlayerData* (*)()>(
        universe->getScriptModule(), universe->getPlayerDataFunction()
      );
  switch (result) {
    case resourceSearchResult_success:
      {
        /* Everything is OK */
        for(BOOST_AUTO(i, players.begin()); i != players.end(); i++)
        {
          PlayerData::Ptr pD(playerDataFunction());
          i->playerData = pD;
        }
      }
      break;
    default:
      /* There is a problem */
      throw ResourceDeserializationExn(
          universe->getScriptModule(), result, resourceInterface->getError()
        );
  }
  
  /* put units on the map as specified in MapPlayMode */
  for (PlayerId i; i < numPlayers; ++i) {
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
  /* Clear fuses lest they by multiply-inheriting UnitMasks */
  fuseMap.clear();
  /* Clear units lest they outlive the sensor returns from them */
  units.clear();
  
  delete map;
  map = NULL;

  world = NULL;
}

void CompleteWorld::applyEffect(
    const Ref<Effect>& effect,
    void (Effect::*method)(const Ref<LayeredUnit>&)
  )
{
  /** \bug Using point-to-member-function here because it has the minimal code
   * duplication.  This is possibly too slow.  To speed it up this method could
   * be inlined, so that \p method becomes a constant */
  
  /* We can't just iterate over the units and mess with them directly, because
   * if they get destroyed then the iterator will be invalidated, and
   * segfaults ensue.  Instead, we first collect Refs to all the affected
   * units, and then process them all. */
  ISpatial::Result possiblyAffectedUnits =
    spatialIndex->findIntersecting(
        effect->getRegion()->getBoundingBox(), gameObject_unit
      );
  queue<Ref<LayeredUnit> > affectedUnits;
  for (ISpatial::Result::iterator bounded = possiblyAffectedUnits.begin();
      bounded != possiblyAffectedUnits.end(); ++bounded) {
    Ref<LayeredUnit> unit = bounded->dynamicCast<LayeredUnit>();
    if (effect->getRegion()->contains(unit->getStatus())) {
      affectedUnits.push(unit);
    }
  }
  
  while (!affectedUnits.empty()) {
    Ref<LayeredUnit> unit = affectedUnits.front();
    affectedUnits.pop();
    if (unit) {
      /* Somewhat hacky way to circumvent Ref's protection of the pointer */
      (effect.operator->().get()->*method)(unit);
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
hash_list<Effect, Bounded>::iterator CompleteWorld::processEffect(
    hash_list<Effect, Bounded>::iterator effect
  )
{
  /* Call the 'present' method on Units */
  applyEffect(*effect, &Effect::onUnitPresent);

  if ((*effect)->onRemovalTest()) {
    /* We need to remove the effect */
    applyEffect(*effect, &Effect::onUnitLeave);
    return effects.erase(effect);
  } else {
    return ++effect;
  }
}

/** \brief Add a unit to the world.
 *
 * This is not the function that should be called by module code which
 * wishes to create a unit (e.g. because a new one is being built).  For
 * that purpose, use LayeredUnit::spawn.
 */
Ref<LayeredUnit> CompleteWorld::addUnit(
    const LayeredUnit::Ptr& unit,
    PlayerId owner
  ) {
  units.push_back(unit);
  units.back()->changeOwner(owner, changeOwnerReason_created);
  BOOST_FOREACH(const Script::Ptr& script, scripts) {
    script->unitAdded(units.back());
  }
  return units.back();
}

/** Removes \p unit from the World.
 *
 * \warning This is for internal use only.  To kill a unit, use the
 * LayeredUnit::kill method */
void CompleteWorld::removeUnit(LayeredUnit* unit)
{
  unit->changeOwner(PlayerId(), changeOwnerReason_destroyed);
  hash_list<LayeredUnit, Bounded>::iterator it = units.find(unit);
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

/** \brief Advance the game state by one tick.
 *
 * This is the method most likely to be called by the server to actually
 * perform all of the game mecahnics.  Before calling this the server should
 * ensure that all Client's orders have reached their orderMessageQueues.
 */
void CompleteWorld::incrementGameState(void)
{
  bool effectHappened;
  
  /* sanity check */
  assert(world == this);
  
  /* apply incoming orders */
  for (std::vector<Player>::iterator player = players.begin();
      player != players.end(); player++) {
    player->applyIncomingOrders();
  }
  
  /* advance game time */
  timeNow++;
  
  /* process units */
  /* The two iterators are needed because units can die while being updated */
  /** \bug What if both *j and *k die while j is being updated? */
  for (hash_list<LayeredUnit, Bounded>::iterator
      j=units.begin(), k=boost::next(j);
      j!=units.end(); j=k, ++k) {
    /* this moves the unit according to its
     * current speed.  This method also responsible for
     * ensuring that all appropriate client messages caused by the things
     * that happen in it are sent */
    LayeredUnit::incrementState(*j, timeNow);
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
  for (hash_list<Effect, Bounded>::iterator effect = effects.begin();
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
        Debug(
            "Fuse occurance time missed (fuse time=" << fuseEntry.time <<
            ", timeNow = " << timeNow << ")"
          );
      }
      /* Check to see if this fuse has been removed since it was added */
      u_set<FuseToken>::type::iterator rFuseIt =
        removedFuses.find(fuseEntry.token);
      if (rFuseIt != removedFuses.end()) {
        removedFuses.erase(rFuseIt);
        continue;
      }
      /* Use token to look up the fuse in the map */
      u_map<FuseToken, Fuse::Ptr>::type::iterator fuseIt =
        fuseMap.find(fuseEntry.token);
      if (fuseIt == fuseMap.end()) {
        Fatal("Fuse missing from fuseMap");
      }
      /* perform action */
      fuseIt->second->expire(fuseEntry.token);
      /* remove from map */
      fuseMap.erase(fuseIt);
    }

    effectHappened = false;
    
    while (!newEffects.empty()) {
      effectHappened = true;

      Effect* effect = newEffects.front();
      newEffects.pop();
      /* insert, and get an iterator pointing at this new effect */
      hash_list<Effect, Bounded>::iterator effectIt =
        effects.push_back(effect);
      /* Units inside the effect get the 'entry' method called... */
      applyEffect(*effectIt, &Effect::onUnitEnter);
      /* ...and then proceed as if it was a bog standard Effect */
      processEffect(effectIt);
    }
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
  for (hash_list<LayeredUnit, Bounded>::iterator j=units.begin();
      j!=units.end(); ++j) {
    (*j)->clearDirty();
  }
}

/** \brief Performs all necessary changes to a unit caused by it entering or
 * leaving Effects.
 *
 * Whenever a unit's position changed, it should call this function to ensure
 * all appropriate Effects are processed.
 *
 * \param unit        Unit which is moving.
 * \param oldPosition Position \p unit is vacating.
 * \param newPosition Position \p unit is assuming.
 */
void CompleteWorld::applyEntryExitEffects(
    const Ref<LayeredUnit>& unit,
    const Point<sint32>& oldPosition,
    const Point<sint32>& newPosition
  )
{
  /** \todo make this more efficient by storing the data differently */
  {
    const ISpatial::Result possiblyLeftEffects =
      spatialIndex->findContaining(oldPosition, gameObject_effect);
    for (ISpatial::Result::const_iterator it=possiblyLeftEffects.begin();
        it!=possiblyLeftEffects.end(); ++it) {
      const Ref<Effect> i = it->dynamicCast<Effect>();
      if (!i->getRegion()->contains(newPosition)) {
        i->onUnitLeave(unit);
      }
    }
  }

  {
    const ISpatial::Result possiblyEnteredEffects =
      spatialIndex->findContaining(newPosition, gameObject_effect);
    for (ISpatial::Result::const_iterator it=possiblyEnteredEffects.begin();
        it!=possiblyEnteredEffects.end(); ++it) {
      const Ref<Effect> i = it->dynamicCast<Effect>();
      if (!i->getRegion()->contains(oldPosition)) {
        i->onUnitEnter(unit);
      }
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

