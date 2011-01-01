#ifndef FUSEKI__REMOTECLIENT_METHODS_H
#define FUSEKI__REMOTECLIENT_METHODS_H

#include "remoteclient.h"
#include "server.h"
#include "null_cast.h"
#ifdef _MSC_VER
#pragma warning (disable:4101)
#endif //This is about unreferenced local variables.
//Called on e, below. Spurious, because the constructor to e does all the work.

namespace fuseki {

template<typename T>
String RemoteClient::performMagic(
    const std::list<String>& name,
    T newValue
  )
{
  assert(!name.empty());
  if (name.front() == "observer") {
    bool newValueBool = null_cast<bool>(newValue);
    if (newValueBool == observer) {
      SAKUSEN_DEBUG("unexpectedly asked to confirm a non-change");
      return "";
    }
    if (newValueBool) {
      if (!server->getAllowObservers()) {
        return "server does not allow observers";
      }
      if (playerId != 0) {
        return "cannot be an observer when assigned to a player";
      }
    }
    observer = newValueBool;
    if (observer) {
      /* A client becoming an observer can be enough to trigger a game start */
      server->checkForGameStart();
    }
    return "";
  } else if (name.front() == "application") {
    /* Do nothing */
    return "";
  } else if (name.front() == "neveradmin") {
    bool newValueBool = null_cast<bool>(newValue);
    if (newValueBool == neverAdmin) {
      SAKUSEN_DEBUG("unexpectedly asked to confirm non-change");
      return "";
    }

    neverAdmin = newValueBool;
    if (!newValueBool) {
      server->ensureAdminExists();
    }
    return "";
  } else if (name.front() == "admin") {
    bool newValueBool = null_cast<bool>(newValue);
    if (newValueBool == admin) {
      SAKUSEN_DEBUG("unexpectedly asked to confirm non-change");
      return "";
    }

    if (newValueBool) {
      if (neverAdmin) {
        return "cannot make admin when neveradmin is set";
      }
      admin = newValueBool;
      addGroup("admin");
    } else {
      admin = newValueBool;
      server->ensureAdminExists();
    }
    return "";
  } else if (name.front() == "player") {
    sakusen::PlayerId::internal_type newValueInt =
      null_cast<sakusen::PlayerId::internal_type>(newValue);
    if (newValueInt == playerId) {
      SAKUSEN_DEBUG("unexpectedly asked to confirm a non-change");
      return "";
    }

    if (observer) {
      return "cannot assign to a player whilst an observer";
    }

    try {
      setPlayerId(sakusen::PlayerId::fromInteger(newValueInt));
    } catch (sakusen::InvalidPlayerId&) {
      return "the given player id does not exist in this map";
    }
    return "";
  } else if (name.front() == "ready") {
    bool newValueBool = null_cast<bool>(newValue);
    if (newValueBool == ready) {
      SAKUSEN_DEBUG("Unexpectedly asked to confirm non-change");
      return "";
    }

    ready = newValueBool;

    if (newValueBool) {
      server->checkForGameStart();
    }
    return "";
  } else {
    SAKUSEN_FATAL("unexpected child of client branch: " << name.front());
  }
}

}

#ifdef _MSC_VER
#pragma warning (default:4101)
#endif // Reset warning disabled above

#endif // FUSEKI__REMOTECLIENT_METHODS_H

