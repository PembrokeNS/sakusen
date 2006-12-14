#include "player.h"
#include "updatedata.h"
#include "layeredunit.h"
#include "completeworld.h"

#include <stack>

using namespace std;
using namespace __gnu_cxx;

namespace sakusen{
namespace server{

Player::Player(const PlayerTemplate& t) :
  noClients(t.isNoClients()),
  raceFixed(t.isRaceFixed()),
  playerId(0),
  name(""),
  clients(),
  units(),
  lastUnitId(static_cast<uint32>(-1)),
  sensorReturns(),
  sensorReturnsById(),
  lastSensorReturnsId(static_cast<SensorReturnsID>(-1)),
  visibleBallistics(),
  lastClientBallisticId(static_cast<uint32>(-1))
{
  assert(raceFixed || !noClients);
}

Player::Player(const Player& copy) :
  noClients(copy.noClients),
  raceFixed(copy.raceFixed),
  playerId(copy.playerId),
  name(copy.name),
  clients(copy.clients),
  units(copy.units),
  lastUnitId(copy.lastUnitId),
  sensorReturns(),
  sensorReturnsById(),
  lastSensorReturnsId(copy.lastSensorReturnsId),
  visibleBallistics(copy.visibleBallistics),
  lastClientBallisticId(copy.lastClientBallisticId)
{
  if (!copy.sensorReturns.empty()) {
    /* To perform this copy we'd need to set up sensorReturnsById, which would
     * be non-trivial */
    Fatal("Don't support copying a player with SensorReturns in place");
  }
}

Player& Player::operator=(const Player& copy)
{
  if (!copy.sensorReturns.empty()) {
    /* To perform this copy we'd need to set up sensorReturnsById, which would
     * be non-trivial */
    Fatal("Don't support copying a player with SensorReturns in place");
  }

  noClients = copy.noClients;
  raceFixed = copy.raceFixed;
  playerId = copy.playerId;
  name = copy.name;
  clients = copy.clients;
  units = copy.units;
  lastUnitId = copy.lastUnitId;
  sensorReturnsById.clear();
  sensorReturns.clear();
  lastSensorReturnsId = copy.lastSensorReturnsId;
  visibleBallistics = copy.visibleBallistics;
  lastClientBallisticId = copy.lastClientBallisticId;

  return *this;
}

Player::~Player()
{
  sensorReturnsById.clear();
}

void Player::setPlayerId(const PlayerID& id)
{
  playerId = id;
  for (std::list<Client*>::iterator client = clients.begin();
      client != clients.end(); client++) {
    (*client)->setPlayerId(id);
  }
}

void Player::attachClient(Client* client)
{
  assert(!noClients);
  for (std::list<Client*>::iterator c = clients.begin();
      c != clients.end(); c++) {
    if (*c == client) {
      Debug("Tried to attach an already attached client");
      return;
    }
  }
  clients.push_back(client);
}

void Player::detachClient(Client* client)
{
  assert(!noClients);
  for (std::list<Client*>::iterator c = clients.begin();
      c != clients.end(); c++) {
    if (*c == client) {
      clients.erase(c);
      return;
    }
  }
  
  Debug("Tried to detach an unattached client");
}

void Player::removeUnit(const uint32 id, enum changeOwnerReason why)
{
  __gnu_cxx::hash_map<uint32, Ref<LayeredUnit> >::iterator unit =
    units.find(id);
  if (unit == units.end()) {
    Fatal("tried to remove a unit which wasn't there");
  }
  /*Debug("removing unit " << id << " for player " << playerId);*/
  units.erase(id);
  informClients(Update(new UnitRemovedUpdateData(id, why)));
}

void Player::addUnit(const Ref<LayeredUnit>& unit, enum changeOwnerReason why)
{
  /*QDebug("[Player::addUnit]");*/
  ++lastUnitId;
  if (units.count(lastUnitId)) {
    Fatal("tried to insert a unit with an existing id");
  }
  units[lastUnitId] = unit;
  assert(units.find(lastUnitId) != units.end());
  unit->setId(lastUnitId);
  informClients(Update(new UnitAddedUpdateData(why, unit)));
}

void Player::checkSensorReturns()
{
  /* check over our sensor returns for ones where the units have beed destroyed
   * */
  stack<DynamicSensorReturnsRef> invalidatedSensorReturnsIds;
  for (DynamicSensorReturnsRef returns = sensorReturnsById.begin();
      returns != sensorReturnsById.end(); ++returns) {
    /* The fact that the SensorReturns has become empty without our calling
     * update should be enough to prove that the unit is gone */
    if (returns->second->empty()) {
      assert(returns->second->getPerception() == perception_none);
      informClients(Update(new SensorReturnsRemovedUpdateData(returns->first)));
      invalidatedSensorReturnsIds.push(returns);
    }
  }

  /* Now we actually erase the invalidated entries (we couldn't do this before
   * because erasure in a hash_map invalidates iterators) */
  while (!invalidatedSensorReturnsIds.empty()) {
    sensorReturns.erase(invalidatedSensorReturnsIds.top()->second);
    sensorReturnsById.erase(invalidatedSensorReturnsIds.top());
    invalidatedSensorReturnsIds.pop();
  }

  /* First we deal with other players' units */
  for (hash_list<LayeredUnit>::iterator unit = world->getUnits().begin();
      unit != world->getUnits().end(); ++unit) {
    /* check whether this unit already has a return to this player */
    hash_map<PlayerID, DynamicSensorReturnsRef>::iterator it =
      (*unit)->getSensorReturns().find(playerId);
    if (it != (*unit)->getSensorReturns().end()) {
      /* we have a sensor return, so we update it */
      Ref<DynamicSensorReturns> returns = it->second->second;
      returns->update();
      /* If it's gone, then remove it */
      if (returns->empty()) {
        informClients(
            Update(new SensorReturnsRemovedUpdateData(returns->getId()))
          );
        sensorReturns.erase(it->second->second);
        sensorReturnsById.erase(it->second);
        (*unit)->getSensorReturns().erase(it);
      } else if (returns->isDirty()) {
        /* It's changed, so inform clients */
        informClients(Update(new SensorReturnsAlteredUpdateData(returns)));
        returns->clearDirty();
      }
    } else if ((*unit)->getOwner() != playerId) {
      /* we have no sensor return, so we determine whether one should exist
       * */
      SensorReturnsID newId = lastSensorReturnsId + 1;
      DynamicSensorReturns newReturns(newId, *unit, this);
      if (!newReturns.empty()) {
        /* Clear the dirty flag that will certainly have been set */
        newReturns.clearDirty();
        sensorReturns.push_back(new DynamicSensorReturns(newReturns));
        pair<DynamicSensorReturnsRef, bool> inserted = sensorReturnsById.insert(
            pair<SensorReturnsID, Ref<DynamicSensorReturns> >(
              newId, sensorReturns.back()
            )
          );
        if (!inserted.second) {
          Fatal("SensorReturnsID wrapped around");
        }
        (*unit)->getSensorReturns().insert(
              pair<PlayerID, DynamicSensorReturnsRef>(playerId, inserted.first)
            );
        informClients(
            Update(new SensorReturnsAddedUpdateData(inserted.first->second))
          );
        lastSensorReturnsId = newId;
      }
    }
  }

  /* First we check over the existing visible Ballistics to remove the ones
   * that have invalidated */
  queue<MaskedPtr<Ballistic> > invalidatedBallisticIds;
  for (__gnu_cxx::hash_map<
        MaskedPtr<Ballistic>, pair<uint32, Ref<const Ballistic> >
      >::iterator
      vb = visibleBallistics.begin(); vb != visibleBallistics.end(); ++vb) {
    if (!vb->second.second.isValid()) {
      invalidatedBallisticIds.push(vb->first);
      informClients(Update(new BallisticRemovedUpdateData(vb->second.first)));
    }
  }

  /* Now we actually erase the invalidated entries (we couldn't do this before
   * because erasure in a hash_map invalidates iterators) */
  while (!invalidatedBallisticIds.empty()) {
    visibleBallistics.erase(invalidatedBallisticIds.front());
    invalidatedBallisticIds.pop();
  }

  /* Now we check for newly visible Ballistics (of all players) */
  for (hash_list<Ballistic>::const_iterator ballistic =
        world->getBallistics().begin();
      ballistic != world->getBallistics().end(); ++ballistic) {
    if (visibleBallistics.count(*ballistic)) {
      /* we have it already */
      /** \todo See whether it's still visible */
    } else {
      /* It's a new one */
      /** \todo Make an actual visibilty check */
      uint32 clientId = ++lastClientBallisticId;
      /** \bug There's no wraparound check for this ID */
      visibleBallistics[*ballistic] =
        pair<uint32, Ref<const Ballistic> >(
            clientId, *ballistic
          );
      informClients(Update(
            new BallisticAddedUpdateData(clientId, (*ballistic)->getPath())
          ));
    }
  }
}

void Player::applyIncomingOrders(void)
{
  for (std::list<Client*>::iterator client = clients.begin();
      client != clients.end(); client++) {
    while (!(*client)->orderMessageQueueEmpty()) {
      const OrderMessage message = (*client)->orderMessageQueuePopFront();
      __gnu_cxx::hash_map<uint32, Ref<LayeredUnit> >::iterator orderee =
        units.find(message.getOrderee());
      if (orderee == units.end()) {
        /* FIXME: This should not be a fatal error, but is for debugging */
        Fatal("Order for non-existent unit id " << message.getOrderee() <<
              ", player " << playerId);
      } else {
        /*Debug("order for unit id " << message.getOrderee());*/
        orderee->second->enqueueOrder(
            message.getCondition(),
            message.getOrder()
          );
      }
    }
  }
}

void Player::informClients(const Update& update)
{
  /*Debug("clients.size()=" << clients.size());*/
  for (std::list<Client*>::iterator client = clients.begin();
      client != clients.end(); client++) {
    /* Debug("informing client"); */
    (*client)->queueUpdate(update);
  }
}

}}

