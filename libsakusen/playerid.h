#ifndef LIBSAKUSEN__PLAYERID_H
#define LIBSAKUSEN__PLAYERID_H

#include "libsakusen-global.h"

#include "stringutils.h"
#include "idbase.h"


namespace sakusen {

/** \brief Unique player identifier
 *
 * At present this is essentially a wrapper for uint8.  It can be altered if
 * we ever want more than 255 players
 */
class PlayerId : public IdBase<uint8, PlayerId> {
};

}

namespace sakusen {

/** \brief Exception to describe the case of an invalid player Id.
 *
 * \todo Make the version of the ctr that takes a player Id do something useful
 * with it.
 */
class InvalidPlayerId : public std::runtime_error {
  public:
    PlayerId id;
    InvalidPlayerId(): std::runtime_error("InvalidPlayerId"), id() {}
    InvalidPlayerId(PlayerId p): std::runtime_error("InvalidPlayerId"), id(p) {}
};

}

#endif // LIBSAKUSEN__PLAYERID_H

