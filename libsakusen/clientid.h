#ifndef CLIENTID_H
#define CLIENTID_H

#include "stringutils.h"

namespace sakusen {

/** \brief Typedef for unique client identifier, in case we ever want more than
 * 255 clients
 *
 * Code may assume this is an unsigned integer type */
typedef uint8 ClientId;

/** \brief Converts ClientId to string
 */
inline String clientId_toString(ClientId id)
{
  return numToString(id);
}

/** \brief Converts string to ClientId
 *
 * Guaranteed that if \c id is some ClientId then
 * \code
 * id == clientId_fromString(clientId_toString(id))
 * \endcode
 */
inline ClientId clientId_fromString(String str)
{
  return numFromString<ClientId>(str);
}

}

#endif // CLIENTID_H

