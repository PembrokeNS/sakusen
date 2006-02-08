#include "player.h"
#include "unit.h"
#include "updatedata.h"

using namespace sakusen;

Player::Player(const PlayerTemplate& t) :
  noClients(t.isNoClients()),
  raceFixed(t.isRaceFixed()),
  playerId(0),
  name(""),
  clients(),
  units(),
  lastUnitId(static_cast<uint32>(-1))
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
  lastUnitId(copy.lastUnitId)
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
  __gnu_cxx::hash_map<uint32, Unit*>::iterator unit = units.find(id);
  if (unit == units.end()) {
    Fatal("tried to remove a unit which wasn't there");
  }
  Debug("removing unit " << id << " for player " << playerId);
  units.erase(id);
  informClients(Update(UnitRemovedUpdateData(id, why)));
}

void Player::addUnit(Unit* unit, enum changeOwnerReason why)
{
  ++lastUnitId;
  if (units.count(lastUnitId)) {
    Fatal("tried to insert a unit with an existing id");
  }
  units[lastUnitId] = unit;
  assert(units.find(lastUnitId) != units.end());
  /* Debugf(("added unit %d for player %d (%x) (who now has %d units)",
        lastUnitId, playerId, (unsigned int) this, units.size())); */
  unit->setId(lastUnitId);
  informClients(Update(UnitAddedUpdateData(unit, why)));
}

void Player::applyIncomingOrders(void)
{
  for (std::list<Client*>::iterator client = clients.begin();
      client != clients.end(); client++) {
    while (!(*client)->orderMessageQueueEmpty()) {
      const OrderMessage message = (*client)->orderMessageQueuePopFront();
      __gnu_cxx::hash_map<uint32, Unit*>::iterator orderee =
        units.find(message.getOrderee());
      if (orderee == units.end()) {
        /* FIXME: This should not be a fatal error, but is for debugging */
        Fatal("Order for non-existent unit id " << message.getOrderee() <<
              ", player " << playerId);
      } else {
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
  for (std::list<Client*>::iterator client = clients.begin();
      client != clients.end(); client++) {
    /* Debug("informing client"); */
    (*client)->sendUpdate(update);
  }
}

