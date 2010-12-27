#ifndef FUSEKI__SERVER_METHODS_H
#define FUSEKI__SERVER_METHODS_H

#include "server.h"

#include <sakusen/resourceinterface-methods.h>
#include "serveraction.h"
#include "remoteclient-methods.h"
#include "null_cast.h"

namespace fuseki {

/** \brief Settings tree callback
 *
 * Called by leaves of the settings tree to advise the
 * server that the setting on the given
 * leaf wants to be altered.  Does not themselves perform any
 * alteration of the leaf (at least not directly),
 * but they do execute the associated
 * magic.
 *
 * \param altering The leaf being altered.
 * \param newValue The new value of the leaf.
 * \return Empty string if the alteration may be performed, or a reason why
 * it cannot be performed otherwise. */
template<typename T>
String Server::settingAlteringCallback(
    settingsTree::Leaf* altering,
    const T newValue
  )
{
  /* Construct a type which is equal to T when T is an integer type but is
   * equal to 'int' otherwise */
  typedef typename boost::mpl::if_<
      typename boost::is_integral<T>::type,
      T,
      int
    >::type TInt;

  /* Perform magic associated with the setting */
  std::list<String> fullName = altering->getFullNameAsList();

  assert(!fullName.empty());

  if (fullName.front() == "clients") {
    /* If the change is inside a client branch, then forward the magic to the
     * client object */
    fullName.pop_front();
    assert(!fullName.empty());
    sakusen::ClientId id = sakusen::ClientId::fromString(fullName.front());
    assert(clients.count(id));
    fullName.pop_front();
    return clients[id]->performMagic<T>(/*altering,*/ fullName, newValue);
  } else if (fullName.front() == "players") {
    /* If the change is inside a player branch, then forward the magic to the
     * player object */
    fullName.pop_front();
    assert(!fullName.empty());
    sakusen::PlayerId id = sakusen::PlayerId::fromString(fullName.front());
    fullName.pop_front();
    sakusen::server::Player& player = players[id];
    assert(!fullName.empty());
    if (fullName.front() == "maxclients") {
      /* We need to ensure that the specified constraint on the number of
       * clients doesn't drop below the current number of clients */
      if (null_cast<TInt>(newValue) < player.getNumClients()) {
        return "cannot set the maximum number of clients lower than the "
          "present number";
      }
      return "";
    } else if (fullName.front() == "application") {
      /* Do nothing */
      return "";
    } else {
      SAKUSEN_FATAL("unexpected child of player branch: " << fullName.front());
    }
  } else if (fullName.front() == "server") {
    /* It's in the server branch, so we need to deal with it */
    fullName.pop_front();
    assert(!fullName.empty());
    if (fullName.front() == "allowobservers") {
      setAllowObservers(null_cast<TInt>(newValue) != 0);
      return "";
    } else if (fullName.front() == "application") {
      /* Do nothing */
      return "";
    } else if (fullName.front() == "blockedaddresses") {
      /* Do nothing */
      return "";
    } else if (fullName.front() == "plugins") {
      checkPlugins(null_cast<std::set<String> >(newValue));
      return "";
    } else {
      SAKUSEN_FATAL("unexpected child of server branch: " << fullName.front());
    }
  } else if (fullName.front() == "game") {
    fullName.pop_front();
    assert(!fullName.empty());
    if (fullName.front() == "speed") {
      gameSpeed = null_cast<TInt>(newValue);
      return "";
    } else if (fullName.front() == "universe") {
      fullName.pop_front();
      assert(!fullName.empty());
      if (fullName.front() == "name") {
        const String& newValueString = null_cast<String>(newValue);
        if (universe != NULL && newValueString == universePath) {
          return "";
        }
        sakusen::ResourceSearchResult result;
        sakusen::Universe::Ptr u;
        String uPath;
        tie(u, result, uPath) =
          resourceInterface->search<sakusen::Universe>(newValueString);
        switch(result) {
          case sakusen::resourceSearchResult_success:
            assert(u);
            /* File an asynchronous request to use the universe */
            pendingActions.push_front(new RequestUniverseAction(u, uPath));
            return "";
          case sakusen::resourceSearchResult_ambiguous:
            return "specified universe name '"+newValueString+"' ambiguous";
          case sakusen::resourceSearchResult_notFound:
            return "specified universe name not found";
          case sakusen::resourceSearchResult_error:
            return String("error finding resource: ") +
              resourceInterface->getError();
          default:
            SAKUSEN_FATAL("unexpected ResourceSearchResult: " << result);
        }
      } else if (fullName.front() == "hash") {
        return "";
      } else {
        SAKUSEN_FATAL("unexpected child of universe branch: " << fullName.front());
      }
    } else if (fullName.front() == "map") {
      if (universe == NULL) {
        return "must specify a universe before map";
      }
      const String& newValueString = null_cast<String>(newValue);
      if (map != NULL && newValueString == mapPath) {
        return "";
      }
      sakusen::ResourceSearchResult result;
      sakusen::MapTemplate::Ptr m;
      String mPath;
      boost::tie(m, result, mPath) =
        resourceInterface->
        search<sakusen::MapTemplate>(newValueString, universe);
      switch(result) {
        case sakusen::resourceSearchResult_success:
          assert(m);
          /* File an asynchronous request to use the map */
          pendingActions.push_front(new RequestMapAction(m, mPath));
          return "";
        case sakusen::resourceSearchResult_ambiguous:
          return "specified map template name ambiguous";
        case sakusen::resourceSearchResult_notFound:
          return "specified map template name not found";
        case sakusen::resourceSearchResult_error:
          return String("error finding resource: ") +
            resourceInterface->getError();
        default:
          SAKUSEN_FATAL("unexpected ResourceSearchResult: " << result);
      }
    } else {
      SAKUSEN_FATAL("unexpected child of game branch: " << fullName.front());
    }
  } else if (fullName.front() == "plugins") {
    fullName.pop_front();
    assert(!fullName.empty());
    u_map<String, Plugin::Ptr>::type::iterator plugin =
      plugins.find(fullName.front());
    assert(plugin != plugins.end());
    fullName.pop_front();
    assert(!fullName.empty());
    return plugin->second->settingAlterationCallback<T>(altering, newValue);
  } else {
    SAKUSEN_FATAL("unexpected child of root node: " << fullName.front());
  }
}

}

#endif // FUSEKI__SERVER_METHODS_H

