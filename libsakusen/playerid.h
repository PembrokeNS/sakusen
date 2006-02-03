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

}

#endif // PLAYERID_H

