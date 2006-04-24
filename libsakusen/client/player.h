#ifndef PLAYER_H
#define PLAYER_H

#include "libsakusen-global.h"
#include <list>
#include "gnu_extensions.h"
#include "playerid.h"
#include "client.h"
#include "updatetype.h"
#include "changeownerreason.h"
#include "playertemplate.h"

namespace sakusen {
namespace client {

class Player {
  private:
    Player();
  public:
    Player(const Player& copy);
    ~Player();
  private:
    PlayerID playerId;
    String name;
    std::list<Client*> clients; /* owned by this */
  public:
    /* accessors */
    inline const String& getName(void) const { return name; }
    inline uint16 getNumClients(void) const { return (uint16)clients.size(); }
};

}}

#endif

