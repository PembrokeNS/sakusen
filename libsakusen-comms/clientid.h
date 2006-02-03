#ifndef CLIENTID_H
#define CLIENTID_H

#include "stringutils.h"

namespace sakusen {
namespace comms {

typedef uint8 ClientID;

inline String clientID_toString(ClientID id)
{
  return numToString(id);
}

inline ClientID clientID_fromString(String str)
{
  return numFromString<ClientID>(str);
}

}}

#endif // CLIENTID_H

