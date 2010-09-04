#ifndef LIBSAKUSEN_CLIENT__PLAYER_H
#define LIBSAKUSEN_CLIENT__PLAYER_H

#include <sakusen/client/global.h>
#include <list>
#include <sakusen/gnu_extensions.h>
#include <sakusen/playerid.h>
#include <sakusen/client/client.h>
#include <sakusen/updatetype.h>
#include <sakusen/changeownerreason.h>
#include <sakusen/playertemplate.h>

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
    PlayerId playerId;
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

#endif // LIBSAKUSEN_CLIENT__PLAYER_H

