#include "player.h"
#include "updatedata.h"
#include "layeredunit.h"
#include "completeworld.h"

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
  lastSensorReturnsId(static_cast<SensorReturnsID>(-1))
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
  sensorReturns(copy.sensorReturns),
  lastSensorReturnsId(copy.lastSensorReturnsId)
{
}

Player::~Player()
{
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
  __gnu_cxx::hash_map<uint32, LayeredUnit*>::iterator unit = units.find(id);
  if (unit == units.end()) {
    Fatal("tried to remove a unit which wasn't there");
  }
  /*Debug("removing unit " << id << " for player " << playerId);*/
  units.erase(id);
  informClients(Update(UnitRemovedUpdateData(id, why)));
}

void Player::addUnit(LayeredUnit* unit, enum changeOwnerReason why)
{
  ++lastUnitId;
  if (units.count(lastUnitId)) {
    Fatal("tried to insert a unit with an existing id");
  }
  units[lastUnitId] = unit;
  assert(units.find(lastUnitId) != units.end());
  unit->setId(lastUnitId);
  informClients(Update(UnitAddedUpdateData(why, unit)));
}

void Player::checkSensorReturns()
{
  for (list<LayeredUnit>::iterator unit = world->getUnits().begin();
      unit != world->getUnits().end(); ++unit) {
    /* check whether this unit already has a return to this player */
    hash_map<PlayerID, DynamicSensorReturnsRef>::iterator it =
      unit->getSensorReturns().find(playerId);
    if (it != unit->getSensorReturns().end()) {
      /* we have a sensor return, so we update it */
      DynamicSensorReturns& returns = it->second->second;
      returns.update();
      /* If it's gone, then remove it */
      if (returns.empty()) {
        informClients(Update(SensorReturnsRemovedUpdateData(returns.getId())));
        sensorReturns.erase(it->second);
        unit->getSensorReturns().erase(it);
      } else if (returns.isDirty()) {
        /* It's changed, so inform clients */
        informClients(Update(SensorReturnsAlteredUpdateData(&returns)));
        returns.clearDirty();
      }
    } else if (unit->getOwner() != playerId) {
      /* we have no sensor return, so we determine whether one should exist
       * */
      SensorReturnsID newId = lastSensorReturnsId + 1;
      DynamicSensorReturns newReturns(newId, &*unit, this);
      if (!newReturns.empty()) {
        /* Clear the dirty flag that will certainly have been set */
        newReturns.clearDirty();
        pair<DynamicSensorReturnsRef, bool> inserted = sensorReturns.insert(
            pair<SensorReturnsID, DynamicSensorReturns>(newId, newReturns)
          );
        if (!inserted.second) {
          Fatal("SensorReturnsID wrapped around");
        }
        unit->getSensorReturns().insert(
              pair<PlayerID, DynamicSensorReturnsRef>(playerId, inserted.first)
            );
        informClients(
            Update(SensorReturnsAddedUpdateData(&inserted.first->second))
          );
        lastSensorReturnsId = newId;
      }
    }
  }
}

void Player::applyIncomingOrders(void)
{
  for (std::list<Client*>::iterator client = clients.begin();
      client != clients.end(); client++) {
    while (!(*client)->orderMessageQueueEmpty()) {
      const OrderMessage message = (*client)->orderMessageQueuePopFront();
      __gnu_cxx::hash_map<uint32, LayeredUnit*>::iterator orderee =
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

}
}

