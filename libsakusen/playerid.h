#ifndef PLAYERID_H
#define PLAYERID_H

#include "libsakusen-global.h"

#include "stringutils.h"

namespace sakusen {

typedef uint8 PlayerID;

inline PlayerID playerID_fromString(String str)
{
  return numFromString<PlayerID>(str);
}

inline String playerID_toString(PlayerID id)
{
  return numToString(id);
}

/** Exception to describe the case of an invalid player ID.
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

