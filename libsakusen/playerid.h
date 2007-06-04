#ifndef PLAYERID_H
#define PLAYERID_H

#include "libsakusen-global.h"

#include "stringutils.h"

namespace sakusen {

/** \brief Typedef for unique client identifier, in case we ever want more than
 * 255 clients
 *
 * Code may assume this is an unsigned integer type */
typedef uint8 PlayerID;

/** \brief Converts PlayerID to string
 */
inline PlayerID playerID_fromString(String str)
{
  return numFromString<PlayerID>(str);
}

/** \brief Converts string to PlayerID
 *
 * Guaranteed that if \c id is some PlayerID then
 * \code
 * id == playerID_fromString(playerID_toString(id))
 * \endcode
 */
inline String playerID_toString(PlayerID id)
{
  return numToString(id);
}

/** \brief Exception to describe the case of an invalid player ID.
 *
 * \todo Make the version of the ctr that takes a player ID do something useful
 * with it.
 */
class InvalidPlayerID : public std::runtime_error {
  public:
    PlayerID id;
    InvalidPlayerID(): std::runtime_error("InvalidPlayerID"), id(0) {}
    InvalidPlayerID(PlayerID p): std::runtime_error("InvalidPlayerID"), id(p) {}
};
}

#endif // PLAYERID_H

