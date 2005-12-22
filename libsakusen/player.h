#ifndef PLAYER_H
#define PLAYER_H

#include "libsakusen-global.h"
#include <list>
#include "gnu_extensions.h"
#include "playerid.h"
#include "client.h"
#include "updatetype.h"
#include "changeownerreason.h"

class Unit;

class LIBSAKUSEN_API Player {
  private:
    PlayerID playerId;
    String name;
    std::list<Client*> clients;
    __gnu_cxx::hash_map<uint32, Unit*> units;
      /* This is a hashtable of the units belonging to the player, keyed by
         their id */
    uint32 lastUnitId; /* The id of the last unit that was added for this
                          player */
  public:
    Player();
    Player(const Player& copy);
    Player(String n);
    ~Player();

    /* accessors */
    void setPlayerId(const PlayerID& id);
    inline const String& getName(void) const { return name; }
    inline uint16 getNumClients(void) const { return clients.size(); }
    void attachClient(Client* client);

    inline bool isReadyForGameStart(void) const {
      /* FIXME: should check that a race is selected, and possibly that it has
       * a client, if needed */
      return true;
    }

    /* game mechanics */
    void removeUnit(const uint32 id, enum changeOwnerReason why);
      /* any call to removeUnit must be followed by a call to addUnit for
       * another player, otherwise the unit will end up in limbo */
    void addUnit(Unit* unit, enum changeOwnerReason why);
      /* returns the new unit id */
    
    void applyIncomingOrders(void);
      /* gets the orders received from each client and applies them to update
       * unit order queues appropriately */
    
    /* Methods to send a update message to all this player's clients */
    void informClients(const Update& update);
};

#endif

