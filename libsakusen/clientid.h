#ifndef CLIENTID_H
#define CLIENTID_H

#include "stringutils.h"

namespace sakusen {

/** \brief Typedef for unique client identifier, in case we ever want more than
 * 255 clients
 *
 * Code may assume this is an unsigned integer type */
typedef uint8 ClientID;

/** \brief Converts ClientID to string
 */
inline String clientID_toString(ClientID id)
{
  return numToString(id);
}

/** \brief Converts string to ClientID
 *
 * Guaranteed that if \c id is some ClientID then
 * \code
 * id == clientID_fromString(clientID_toString(id))
 * \endcode
 */
inline ClientID clientID_fromString(String str)
{
  return numFromString<ClientID>(str);
}

}

#endif // CLIENTID_H

