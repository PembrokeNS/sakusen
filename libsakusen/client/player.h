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

/** \brief Encapsulates a player, as seen by a client.
 *
 * This class is not used at time of writing.
 */
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
    /** \name Accessors. */
    //@{
    inline const String& getName(void) const { return name; }
    inline uint16 getNumClients(void) const { return static_cast<uint16>(clients.size()); }
    //@}
};

}}

#endif

