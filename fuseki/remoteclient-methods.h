#ifndef REMOTECLIENT_METHODS_H
#define REMOTECLIENT_METHODS_H

#include "remoteclient.h"
#include "server.h"

namespace fuseki {

template<typename T>

String RemoteClient::performIntMagic(
    const std::list<String>& name,
    T newValue
  )
{
  assert(!name.empty());
  if (name.front() == "observer") {
    Fatal("observer not an int setting");
  } else if (name.front() == "application") {
    /* Do nothing */
    return "";
  } else if (name.front() == "player") {
    if (newValue == playerId) {
      Debug("unexpectedly asked to confirm a non-change");
      return "";
    }

    if (observer) {
      return "cannot assign to a player whilst an observer";
    }

    try {
      setPlayerId(newValue);
    } catch (sakusen::InvalidPlayerID& e) {
      return "the given player ID does not exist in this map";
    }
    return "";
  } else {
    Fatal("unexpected child of client branch: " << name.front());
  }
}

}

#endif // REMOTECLIENT_METHODS_H

