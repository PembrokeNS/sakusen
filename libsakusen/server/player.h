#ifndef LIBSAKUSEN_SERVER__PLAYER_H
#define LIBSAKUSEN_SERVER__PLAYER_H

#include "libsakusen-global.h"
#include "hash_list.h"
#include "gnu_extensions.h"
#include "playerid.h"
#include "client.h"
#include "updatetype.h"
#include "changeownerreason.h"
#include "playertemplate.h"
#include "dynamicsensorreturns.h"
#include "ballistic.h"
#include "maskedptr.h"

namespace sakusen {
namespace server {

class LayeredUnit;

class LIBSAKUSEN_SERVER_API Player {
  private:
    Player();
  public:
    Player(const PlayerTemplate& t);
    Player(const Player& copy);
    Player& operator=(const Player&);
    ~Player();
  private:
    bool noClients;
    bool raceFixed;
    PlayerID playerId; /**< ID of player is assigned by the World constructor */
    String name;
    std::list<Client*> clients;
    
    /* This is a hashtable of the units belonging to the player, keyed by
       their id */
    __gnu_cxx::hash_map<uint32, Ref<LayeredUnit> > units;
    /* The id of the last unit that was added for this player */
    uint32 lastUnitId;
    
    hash_list<DynamicSensorReturns> sensorReturns;
    
    __gnu_cxx::hash_map<SensorReturnsID, Ref<DynamicSensorReturns> >
      sensorReturnsById;
    SensorReturnsID lastSensorReturnsId;

    /* A container of Refs to the Ballistics visible to this player.  The key
     * is a pointer which we use as a UID for the Ballistic, but it shouldn't
     * be dereferenced - use the Ref instead to avoid danglingness.  The first
     * pair entry is the client-side ID */
    __gnu_cxx::hash_map<
        MaskedPtr<Ballistic>,
        std::pair<uint32, Ref<const Ballistic> >
      > visibleBallistics;
    uint32 lastClientBallisticId;
  public:
    /* accessors */
    PlayerID getId() const { return playerId; }
    void setPlayerId(const PlayerID& id);
    inline const String& getName(void) const { return name; }
    inline void setName(const String& n) { name = n; }
    inline uint16 getNumClients(void) const {
      return static_cast<uint16>(clients.size());
    }
    void attachClient(Client* client);
    void detachClient(Client* client);
    const __gnu_cxx::hash_map<uint32, Ref<LayeredUnit> >& getUnits(void) const {
      return units;
    }
    Ref<DynamicSensorReturns> getSensorReturns(SensorReturnsID id) {
      DynamicSensorReturnsRef it = sensorReturnsById.find(id);
      if (it == sensorReturnsById.end()) {
        return Ref<DynamicSensorReturns>();
      }
      return it->second;
    }

    inline bool isReadyForGameStart(void) const {
      /* FIXME: should check that a race is selected */
      return (clients.size() > 0 || noClients);
    }

    /* game mechanics */
    void removeUnit(const uint32 id, enum changeOwnerReason why);
      /* any call to removeUnit must be followed by a call to addUnit for
       * another player, otherwise the unit will end up in limbo */
    void addUnit(const Ref<LayeredUnit>& unit, enum changeOwnerReason why);
    void checkSensorReturns();
    
    void applyIncomingOrders(void);
      /* gets the orders received from each client and applies them to update
       * unit order queues appropriately */
    
    /* Methods to send a update message to all this player's clients */
    void informClients(const Update& update);
};

}}

#include "layeredunit.h"

#endif // LIBSAKUSEN_SERVER__PLAYER_H

