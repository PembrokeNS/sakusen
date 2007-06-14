#ifndef SERVER_METHODS_H
#define SERVER_METHODS_H

#include "server.h"
#include "remoteclient-methods.h"

namespace fuseki {

template<typename T>
String Server::intSettingAlteringCallback(
    settingsTree::Leaf* altering,
    const T newValue
  )
{
  /* Perform magic associated with the setting */
  std::list<String> fullName = altering->getFullNameAsList();

  assert(!fullName.empty());

  if (fullName.front() == "clients") {
    /* If the change is inside a client branch, then forward the magic to the
     * client object */
    fullName.pop_front();
    assert(!fullName.empty());
    sakusen::ClientId id =
      sakusen::clientId_fromString(fullName.front());
    assert(clients.count(id));
    fullName.pop_front();
    return clients[id]->performIntMagic(/*altering,*/ fullName, newValue);
  } else if (fullName.front() == "players") {
    /* If the change is inside a player branch, then forward the magic to the
     * player object */
    fullName.pop_front();
    assert(!fullName.empty());
    sakusen::PlayerId id = sakusen::playerId_fromString(fullName.front());
    fullName.pop_front();
    sakusen::server::Player& player = players[id];
    assert(!fullName.empty());
    if (fullName.front() == "maxclients") {
      /* We need to ensure that the specified constraint on the number of
       * clients doesn't drop below the current number of clients */
      if (newValue < player.getNumClients()) {
        return "cannot set the maximum number of clients lower than the "
          "present number";
      }
      return "";
    } else if (fullName.front() == "application") {
      /* Do nothing */
      return "";
    } else {
      Fatal("unexpected child of player branch: " << fullName.front());
    }
  } else if (fullName.front() == "server") {
    /* It's in the server branch, so we need to deal with it */
    fullName.pop_front();
    assert(!fullName.empty());
    if (fullName.front() == "allowobservers") {
      setAllowObservers(newValue!=0);
      return "";
    } else {
      Fatal("unexpected child of server branch: " << fullName.front());
    }
  } else if (fullName.front() == "game") {
    fullName.pop_front();
    assert(!fullName.empty());
    if (fullName.front() == "speed") {
      gameSpeed = newValue;
      return "";
    } else {
      Fatal("unexpected child of game branch: " << fullName.front());
    }
  } else if (fullName.front() == "plugins") {
    fullName.pop_front();
    assert(!fullName.empty());
    sakusen::hash_map_string<Plugin::Ptr>::type::iterator plugin =
      plugins.find(fullName.front());
    assert(plugin != plugins.end());
    fullName.pop_front();
    assert(!fullName.empty());
    return plugin->second->settingAlterationCallback<T>(altering, newValue);
  } else {
    Fatal("unexpected child of root node: " << fullName.front());
  }
}

}

#endif // SERVER_METHODS_H

