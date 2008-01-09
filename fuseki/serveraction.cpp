#include "serveraction.h"

#include "plugins/pluginexn.h"

using namespace std;
using namespace __gnu_cxx;

using namespace sakusen;
using namespace sakusen::comms;
using namespace sakusen::server;
using namespace sakusen::server::plugins;

namespace fuseki {

ServerAction::~ServerAction()
{
}

void ClientChangeSettingAction::act(Server& server) const
{
  hash_map<ClientId, RemoteClient*>::iterator clientIt =
    server.clients.find(clientId);
  if (clientIt == server.clients.end()) {
    server.out << "Ignoring client change setting request because client has "
      "vanished\n";
    return;
  }
  RemoteClient* client = clientIt->second;
  server.out << "Client asked to change setting " << data.getSetting() << '\n';
  String reason;
  if (!(reason = server.settings->changeRequest(
        data.getSetting(), data.getValue(), client
      )).empty()) {
    /* That a non-empty string was returned implies that a problem
     * occured.  We tell the client as much */
    server.out << "Request rejected (" << reason << ")\n";
    client->send(new RejectMessageData(reason));
  }
}

void EnsureAdminExistsAction::act(Server& server) const
{
  if (server.isGameStarted()) {
    return;
  }
  server.out << "Checking for existence of an admin\n";
  RemoteClient* adminCandidate = NULL;
  bool needAdmin = true;

  for (hash_map<ClientId, RemoteClient*>::iterator client =
      server.clients.begin(); client != server.clients.end(); client++) {
    if (client->second->isAdmin()) {
      needAdmin = false;
      break;
    }
    if (adminCandidate == NULL && !client->second->isNeverAdmin()) {
      adminCandidate = client->second;
    }
  }

  if (needAdmin && adminCandidate != NULL) {
    server.out << "Promoting a client to admin status\n";
    server.changeInClientBranch(adminCandidate, "admin", "true");
  }
}

void RequestUniverseAction::act(Server& server) const
{
  assert(!server.isGameStarted());
  assert(universe);
  assert(!path.empty());
  server.universe = universe;
  server.universePath = path;
  String reason = server.settings->changeRequest(
      "game:universe:name", path, &server
    );
  assert(reason == "");
  reason = server.settings->changeRequest(
      "game:universe:hash", universe->getHash(), &server
    );
  assert(reason == "");
  server.gameToAdvertise->setUniverse(path);
  server.gameToAdvertiseChanged = true;
}

void RequestMapAction::act(Server& server) const
{
  assert(!server.isGameStarted());
  assert(map);
  assert(!path.empty());
  server.map = map;
  server.mapPath = path;
  String reason = server.settings->changeRequest(
      "game:map", path, &server
    );
  assert(reason == "");
  server.pendingActions.push_front(new SetMapPlayModeAction(0));

  server.gameToAdvertise->setMap(path);
  server.gameToAdvertiseChanged = true;
}

void SetMapPlayModeAction::act(Server& server) const
{
  assert(!server.isGameStarted());
  server.mapPlayMode = mode;
  server.clearPlayers();
  server.createPlayersFor(server.map->getPlayModes()[mode]);
  server.checkForGameStart();
}

void AddPluginAction::act(Server& server) const
{
  if (server.plugins.count(name)) {
    return;
  }
  try {
    Plugin::Ptr plugin(server.pluginInterface.load(name));
    server.plugins.insert(make_pair(plugin->getName(), plugin));
    if (plugin->getName() != name) {
      /* Correct the name as listed in the settings tree. */
      String reason = server.settings->changeRequest(
          "server:plugins", "-"+name, &server
        );
      assert(reason == "");
      /* At this point, the plugin we've just loaded will have been
       * flagged for removal, so we must remove it, lest this recently
       * loaded plugin be immediately unloaded again. */
      assert(plugin->getName() ==
          dynamic_cast<RemovePluginAction&>(server.pendingActions.front()).name
        );
      server.pendingActions.pop_front();
      /* And then add it in so the same thing doesn't happen again */
      reason = server.settings->changeRequest(
          "server:plugins", "+"+plugin->getName(), &server
        );
      assert(reason == "");
    }
  } catch (PluginExn& e) {
    Debug("plugin load failed: " + e.message);
    String reason = server.settings->changeRequest(
        "server:plugins", "-"+name, &server
      );
    assert(reason == "");
  }
}

void RemovePluginAction::act(Server& server) const
{
  hash_map_string<Plugin::Ptr>::type::iterator p = server.plugins.find(name);
  if (p == server.plugins.end()) {
    return;
  }
  server.plugins.erase(p);
}

void CheckForGameStartAction::act(Server& server) const
{
  if (server.isGameStarted()) {
    return;
  }
  /* If there's another CheckForGameStartAction right after this one, then
   * skip this one. */
  boost::ptr_list<ServerAction>::iterator i = server.pendingActions.begin();
  assert(i != server.pendingActions.end());
  assert(&*i == this);
  ++i;
  if(i != server.pendingActions.end()) {
    if (dynamic_cast<CheckForGameStartAction*>(&*i)) {
      return;
    }
    /* If there's another action after this one, we skip this attempt and
     * resubmit the request to be done later. */
    server.pendingActions.push_back(new CheckForGameStartAction());
    return;
  }

  server.out << "Checking for game start\n";
  /* Check that we have:
   * - A map and mapplaymode selected
   * - Races selected for all players
   * - A client for all players that need one
   * - All clients ready
   * - All clients either observers or else assigned to some player
   */
  if (server.map == NULL) {
    server.out << "Not ready because no map selected\n";
  } else {
    /** \bug Can't cope with using fewer players than the maximum number
     * */
    bool allPlayersReady = true;
    for (vector<Player>::iterator player = server.players.begin();
        player != server.players.end(); player++) {
      if (!player->isReadyForGameStart()) {
        allPlayersReady = false;
        server.out << "Not ready because player ";
        if (player->getName() == "") {
          server.out << (player - server.players.begin());
        } else {
          server.out << "'" << player->getName() << "'";
        }
        server.out << " not ready\n";
        break;
      }
    }
    if (allPlayersReady) {
      bool allClientsReady = true;
      for (hash_map<ClientId, RemoteClient*>::iterator client =
          server.clients.begin(); client != server.clients.end(); client++) {
        if (!client->second->isReadyForGameStart()) {
          allClientsReady = false;
          server.out << "Not ready because client '" <<
            client->second->getClientId().toString() <<
            "' not ready\n";
          break;
        }
      }
      server.startGame = allClientsReady;
    }
  }
}

}

