#ifndef LIBSAKUSEN_SERVER__PLAYER_H
#define LIBSAKUSEN_SERVER__PLAYER_H

#include "libsakusen-global.h"
#include <list>
#include "gnu_extensions.h"
#include "playerid.h"
#include "client.h"
#include "updatetype.h"
#include "changeownerreason.h"
#include "playertemplate.h"

namespace sakusen {
namespace server {

class LayeredUnit;

class LIBSAKUSEN_API Player {
  private:
    Player();
  public:
    Player(const PlayerTemplate& t);
    Player(const Player& copy);
    ~Player();
  private:
    bool noClients;
    bool raceFixed;
    PlayerID playerId; /**< ID of player is assigned by the World constructor */
    String name;
    std::list<Client*> clients;
    __gnu_cxx::hash_map<uint32, LayeredUnit*> units;
      /* This is a hashtable of the units belonging to the player, keyed by
         their id */
    uint32 lastUnitId; /* The id of the last unit that was added for this
                          player */
  public:
    /* accessors */
    void setPlayerId(const PlayerID& id);
    inline const String& getName(void) const { return name; }
    inline void setName(const String& n) { name = n; }
    inline uint16 getNumClients(void) const { return clients.size(); }
    void attachClient(Client* client);
    void detachClient(Client* client);

    inline bool isReadyForGameStart(void) const {
      /* FIXME: should check that a race is selected */
      return (clients.size() > 0 || noClients);
    }

    /* game mechanics */
    void removeUnit(const uint32 id, enum changeOwnerReason why);
      /* any call to removeUnit must be followed by a call to addUnit for
       * another player, otherwise the unit will end up in limbo */
    void addUnit(LayeredUnit* unit, enum changeOwnerReason why);
      /* returns the new unit id */
    
    void applyIncomingOrders(void);
      /* gets the orders received from each client and applies them to update
       * unit order queues appropriately */
    
    /* Methods to send a update message to all this player's clients */
    void informClients(const Update& update);
};

}}

#endif // LIBSAKUSEN_SERVER__PLAYER_H

