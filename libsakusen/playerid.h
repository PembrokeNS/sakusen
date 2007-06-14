#ifndef PLAYERID_H
#define PLAYERID_H

#include "libsakusen-global.h"

#include "stringutils.h"

namespace sakusen {

/** \brief Typedef for unique client identifier, in case we ever want more than
 * 255 clients
 *
 * Code may assume this is an unsigned integer type */
typedef uint8 PlayerId;

/** \brief Converts PlayerId to string
 */
inline PlayerId playerId_fromString(String str)
{
  return numFromString<PlayerId>(str);
}

/** \brief Converts string to PlayerId
 *
 * Guaranteed that if \c id is some PlayerId then
 * \code
 * id == playerId_fromString(playerId_toString(id))
 * \endcode
 */
inline String playerId_toString(PlayerId id)
{
  return numToString(id);
}

/** \brief Exception to describe the case of an invalid player Id.
 *
 * \todo Make the version of the ctr that takes a player Id do something useful
 * with it.
 */
class InvalidPlayerId : public std::runtime_error {
  public:
    PlayerId id;
    InvalidPlayerId(): std::runtime_error("InvalidPlayerId"), id(0) {}
    InvalidPlayerId(PlayerId p): std::runtime_error("InvalidPlayerId"), id(p) {}
};
}

#endif // PLAYERID_H

