#include "server.h"

#include "libsakusen-global.h"
#include "resourcesearchresult.h"
#include "libsakusen-comms-global.h"
#include "message.h"
#include "remoteclient.h"
#include "socketexn.h"
#include "resourceinterface-methods.h"
#include "completeworld.h"
#include "servedgame.h"
#include "serveraction.h"
#include "settingstree/stringsetleaf.h"
#include "settingstree/intleaf.h"
#include "settingstree/boolleaf.h"
#include "timeutils.h"

//In libsakusen/server.
#include "plugins/pluginexn.h"

#ifndef _MSC_VER
	#include "revision.h"
#else
  #define REVISION "0.1"
#endif

#ifndef DISABLE_AVAHI
	#include "mdns.h"
#endif

#include <time.h>
#include <signal.h>

#include <ostream>
#include <boost/bind.hpp>
#include <pcrecpp.h>

using namespace std;
using namespace __gnu_cxx;

using namespace sakusen;
using namespace sakusen::server;
using namespace sakusen::server::plugins;
using namespace sakusen::comms;
using namespace fuseki;
using namespace fuseki::settingsTree;

Server::Server(
    std::ostream& o,
    const sakusen::ResourceInterface::Ptr& r,
    const std::vector<boost::filesystem::path>& pP,
#ifndef DISABLE_UNIX_SOCKETS
    bool a,
    const sakusen::comms::Socket::Ptr& unS,
#endif
    const sakusen::comms::Socket::Ptr& tS,
    bool d
  ) :
  SettingsUser("server"),
  dots(d),
#ifndef DISABLE_UNIX_SOCKETS
  abstract(a),
  unixSocket(unS),
#endif
  tcpSocket(tS),
  out(o),
  resourceInterface(r),
  pluginInterface(pP, this),
  clients(10),
  universe(),
  map(),
  mapPlayMode(0),
  gameToAdvertiseChanged(false),
  startGame(false),
  players(),
  settings(new SettingsTree(this)),
  allowObservers(false),
  gameStarted(false),
  gameSpeed(DEFAULT_GAME_SPEED)
{
  String reason;
  reason = settings->changeRequest(
      "server:application:name",
      APPLICATION_NAME,
      this
    );
  assert(reason == "");
  reason = settings->changeRequest(
      "server:application:version",
      APPLICATION_VERSION,
      this
    );
  assert(reason == "");
  reason = settings->changeRequest(
      "server:application:revision",
      REVISION,
      this
    );
  assert(reason == "");
}

Server::~Server()
{
  /* Note that this loop is here mostly for paranoia purposes.  Really all
   * clients should have been removed before destruction, because they need to
   * be properly informed, etc. which this destructor does not do. */
  while (!clients.empty()) {
    delete clients.begin()->second;
    clients.erase(clients.begin());
  }

  /* Must clear plugins before end of destructor because they retain a pointer
   * to this, and use it during their destruction */
  plugins.clear();
}

ClientId Server::getFreeClientId()
{
  /* Just use a hopelessly naive algorithm for allocating Ids.  Note that this
   * assumes that ClientId is an integer type (or at least sufficiently similar
   * for this algorithm to work properly) */
  ClientId i;
  do {
    if (0 == clients.count(i)) {
      return i;
    }
    i++;
  } while (i.valid());

  /* Return of -1 indicates no free Ids */
  return ClientId::invalid();
}

void Server::addClient(
    const String& requestedAddress,
    const String& fromAddress,
    const sakusen::comms::Socket::Ptr& existingSocket
  )
{
  /* Check whether this address is blocked */
  set<String> blockedAddresses =
    boost::dynamic_pointer_cast<StringSetLeaf>(
        settings->getNode("server:blockedaddresses")
      )->getValueAsSet();
  for (set<String>::iterator i=blockedAddresses.begin();
      i!=blockedAddresses.end(); i++) {
    /* This could be done faster, by saving one regex and not recompiling many
     * all the time, but that requires more magic, and speed is not of the
     * essence at this stage in the game */
    pcrecpp::RE re(*i);
    if ((requestedAddress != "" && re.FullMatch(requestedAddress)) ||
        (fromAddress != "" && re.FullMatch(fromAddress))) {
      /* We matched a blocked address, so return without even bothering to
       * reject them. */
      out << "Ignoring join request because address is blocked.\n";
      return;
    }
  }

  Socket::Ptr socket;
  
  if (requestedAddress != "") {
    socket = Socket::newConnectionToAddress(requestedAddress);
  } else if (NULL != existingSocket) {
    socket = existingSocket;
  } else {
    out << "Could not decide where to send response.\n";
    return;
  }
  ClientId id = getFreeClientId();
  if (!id.valid()) {
    /* No free ids */
    out << "Rejecting join request due to lack of space for more clients.\n";
    socket->send(Message(new RejectMessageData("No space for more clients")));
    return;
  }
  /* Add the client's branch of the settings tree */
  settings->getClientsBranch()->addClient(id);
  clients[id] =
    new RemoteClient(id, this, socket, requestedAddress!=""
#ifndef DISABLE_UNIX_SOCKETS
        , abstract
#endif
      );
}

void Server::removeClient(RemoteClient* client)
{
  settings->getClientsBranch()->removeClient(client->getClientId());
  clients.erase(clients.find(client->getClientId()));
  delete client;
  ensureAdminExists();
}

void Server::handleClientMessages()
{
  queue<ExtensionMessageData> extensionMessages;

  for (hash_map<ClientId, RemoteClient*>::iterator clientIt=clients.begin();
      clientIt!=clients.end(); ) {
    RemoteClient* client = clientIt->second;

    bool clientRemoved = false;

    if (client->isDead()) {
      out << "Removing dead client " <<
        client->getClientId().toString() << "\n";
      removeClient(client);
      clientRemoved = true;
    } else {
      try {
        client->flushIncoming();
        
        while (!client->messageQueueEmpty()) {
          const Message& message = client->messageQueuePopFront();

          /* Forward all the messages to all registered listeners
           * (this has to come before our own processing because our own
           * processing might result in the client being removed) */
          /** \todo Forward only to interested listeners */
          GetPtr<Listener> gp;
          for (hash_map<MaskedPtr<Listener>, Listener::VPtr>::iterator
              listener = listeners.begin();
              listener != listeners.end(); ++listener) {
            listener->second.apply_visitor(gp)->clientMessage(
                client, message
              );
          }

          /* Perform our own processing on the message */
          switch (message.getType()) {
            case messageType_leave:
              out << "Removing client (leaving)\n";
              removeClient(client);
              clientRemoved = true;
              break;
            case messageType_getSetting:
              {
                GetSettingMessageData data = message.getGetSettingData();
                String setting = data.getSetting();
                out << "Client requested setting " << setting << "\n";
                String reason;
                set<String> value;
                Node::ConstPtr node;
                boost::tie(reason, value, node) =
                  settings->getRequest(setting, client);
                if (reason != "") {
                  out << "Request rejected (" << reason << ")\n";
                  client->send(new RejectMessageData(reason));
                } else {
                  /* We send back node->getFullName() rather than just setting
                   * to ensure that it is in canonical form */
                  client->send(new NotifySettingMessageData(
                        node->getFullName(), node->isLeaf(), value
                      ));
                }
              }
              break;
            case messageType_changeSetting:
              {
                /* This has to be submitted asynchronously so that changes due
                 * to the each setting change have a chance to take effect
                 * before later settings changes are processed. */
                pendingActions.push_back(new ClientChangeSettingAction(
                      client->getClientId(), message.getChangeSettingData()
                    ));
              }
              break;
            case messageType_order:
              {
                out << "Received order from client\n";
                if (sakusen::world == NULL) {
                  out << "Ignoring order because game not started\n";
                } else {
                  client->enqueueOrder(message.getOrderData().getOrderMessage());
                }
              }
              break;
            case messageType_extension:
              {
                extensionMessages.push(message.getExtensionData());
              }
              break;
            default:
              out << "Unexpected MessageType " << message.getType() <<
                " from client\n";
              break;
          }

          if (clientRemoved) {
            break;
          }
        }
      } catch (SocketExn& e) {
        out << "Removing client " << client->getClientId().toString() <<
          " due to causing SocketExn: " << e.message << "\n";
        removeClient(client);
        clientRemoved = true;
      } catch (DeserializationExn& e) {
        out << "Removing client " << client->getClientId().toString() <<
          " due to causing DeserializationExn: " << e.message << "\n";
        removeClient(client);
        clientRemoved = true;
      }
    }
    
    if (clientRemoved) {
      /* erasing invalidates the iterator */
      clientIt = clients.begin();
    } else {
      ++clientIt;
    }
  }

  while (!extensionMessages.empty()) {
    ExtensionMessageData& data = extensionMessages.front();
    out << "Got extension message for extension " <<
      data.getExtension() << " version " << data.getVersion() << "\n";
    /* Loop over clients, forawarding the message to all of them */
    /** \todo Restrict to recipients that claim to support this
     * extension at this version, and also take into account any
     * restrictions attached to the message (e.g. chat message sent
     * only to allies) */
    /** \todo Send to other listeners who want it */
    stack<ClientId> clientsToRemove;
    for (hash_map<ClientId, RemoteClient*>::iterator dest =
        clients.begin(); dest!=clients.end(); ++dest) {
      try {
        dest->second->send(new ExtensionMessageData(data));
      } catch (SocketExn& e) {
        out << "Removing client " << dest->first.toString() <<
          " due to causing SocketExn: " << e.message << endl;
        clientsToRemove.push(dest->first);
      }
    }

    while (!clientsToRemove.empty()) {
      removeClient(clients[clientsToRemove.top()]);
      clientsToRemove.pop();
    }
    extensionMessages.pop();
  }
}

void Server::handlePendingActions()
{
  while (!pendingActions.empty()) {
    boost::ptr_list<ServerAction>::iterator begin = pendingActions.begin();
    begin->act(*this);
    /* Can't use pop_front because more things might have been prepended
     * during the call to act */
    pendingActions.erase(begin);
  }
}

void Server::clearPlayers()
{
  for (hash_map<ClientId, RemoteClient*>::iterator clientIt = clients.begin();
      clientIt != clients.end(); ++clientIt) {
    RemoteClient* client = clientIt->second;
    if (client->getPlayerId() != 0) {
      changeInClientBranch(client, "player", "0");
    }
  }
  while (!players.empty()) {
    PlayerId id = players.back().getId();
    assert(id == players.size()-1);
    players.pop_back();
    settings->getPlayersBranch()->removePlayer(id);
  }
}

void Server::createPlayersFor(const MapPlayMode& mode)
{
  assert(players.empty());
  for (PlayerId i; i<mode.getMaxPlayers(); ++i) {
    const PlayerTemplate& player = mode.getPlayer(i);
    players.push_back(Player(player, i));
    settings->getPlayersBranch()->addPlayer(i, player);
  }
}

void Server::setAllowObservers(bool value)
{
  if (allowObservers && !value) {
    /* In this case, we may have to dump some clients who have themselves
     * registered as observers */
    for (hash_map<ClientId, RemoteClient*>::iterator client = clients.begin();
        client != clients.end(); ++client) {
      if (client->second->isObserver()) {
        changeInClientBranch(client->second, "observer", "false");
      }
    }
  }

  allowObservers = value;
}

void Server::changeInClientBranch(
    RemoteClient* client,
    const String& node,
    const String& value
  )
{
  if ("" != settings->changeRequest(
      String("clients:") + client->getClientId().toString() +
        ":" + node, value, this
    )) {
      Fatal("something has gone wrong with the settings tree");
  }
}

void Server::checkPlugins(const std::set<String>& newList)
{
  /* Check for plugins to load */
  for (set<String>::const_iterator newPlugin = newList.begin();
      newPlugin != newList.end(); ++newPlugin) {
    if (!plugins.count(*newPlugin)) {
      pendingActions.push_front(new AddPluginAction(*newPlugin));
    }
  }

  /* Check for plugins to unload */
  for (hash_map_string<Plugin::Ptr>::type::iterator oldPlugin =
      plugins.begin(); oldPlugin != plugins.end(); ++oldPlugin) {
    if (!newList.count(oldPlugin->first)) {
      pendingActions.push_front(new RemovePluginAction(oldPlugin->first));
    }
  }
}

/** \brief Converts a PlayerId into a pointer to a player
 *
 * \param id PlayerId to convert.
 * \return Pointer to Player with given id.
 *
 * Throws InvalidPlayerId if the id is invalid.
 */
Player* Server::getPlayerPtr(PlayerId id)
{
  if (sakusen::server::world == NULL) {
    if (id >= players.size())
      throw InvalidPlayerId(id);
    return &players[id];
  } else {
    return sakusen::server::world->getPlayerPtr(id);
  }
}

bool interrupted = false; /* Use of static data to monitor interrupt of
                             server instance (c.f. interruptHandler below) */

/** Handler for interrupt signals from the keyboard, so that the server can
 * shutdown gracefully.  Because signals are an inherently C thing, there seems
 * to be no nice way to handle them in an OO way, and thus I am reduced to
 * using static data here (c.f. definition of interrupted above).  This means
 * that if two servers are running on the same process, then interrupting
 * either will stop both.  Of course, since the signal is sent to the process
 * as a whole, there's no other sane action anyway, and thus it's not the end
 * of the world. */
void interruptHandler(int /*signal*/)
{
  interrupted = true;
}

/** \brief Runs the server
 *
 * This method will not return until either an entire game has taken place or
 * the server is interrupted */
void Server::serve()
{
  uint8 buf[BUFFER_LEN];
  struct timeval sleepTime = {0, MICRO/10};
  /** \todo Get the TCP port number and game name to the publisher. */
  gameToAdvertise.reset(new ServedGame("Game name", DEFAULT_PORT));
  /** \bug signal is deprecated.  Use sigaction(2) instead */
  signal(SIGINT, &interruptHandler);
  signal(SIGTERM, &interruptHandler);
  out << "Server started.  Hit Ctrl+C to interrupt and shut down server.\n";
  
#ifndef DISABLE_UNIX_SOCKETS
  unixSocket->setNonBlocking(true);
#endif
  if (tcpSocket != NULL) {
    tcpSocket->setNonBlocking(true);
  }

  /* A list of incoming (probably TCP) connections where we expect to get join
   * messages from prospective clients */
  list<pair<Socket::Ptr, timeval> > newConnections;
  
#ifndef DISABLE_AVAHI
  /* Start up the mDNS publisher. */
  MdnsPublisher publisher(gameToAdvertise);
#endif

  /* To begin with we have the game init loop */
  do {
    size_t bytesReceived;
    String receivedFrom;
#ifndef DISABLE_UNIX_SOCKETS
    /* Look for join messages on the unix socket. */
    if ((bytesReceived =
          unixSocket->receiveFrom(buf, BUFFER_LEN, receivedFrom))) {
      IArchive messageArchive(buf, bytesReceived);
      Message message(messageArchive);
      if (message.isRealMessage()) {
        switch (message.getType()) {
          case messageType_join:
            {
              JoinMessageData data = message.getJoinData();
              out << "Considering adding new client at " <<
                data.getAddress() << " based on message through unix "
                "socket\n";
              addClient(data.getAddress(), receivedFrom, Socket::Ptr());
            }
            break;
          default:
            out << "Unexpected MessageType: " << message.getType() << "\n";
            break;
        }
      } else {
        out << "Unrecognized message received\n";
      }
    }
#endif // DISABLE_UNIX_SOCKETS
    
    /* If we have a separate tcp socket then check for messages there too */
    if (tcpSocket != NULL) {
      while (true) {
        Socket::Ptr newConnection = tcpSocket->accept();
        if (newConnection == NULL) {
          break;
        }
        
        newConnection->setNonBlocking(true);
        timeval timeout;
        timeUtils_getTime(&timeout);
        /** \todo We use a straight five second timeout.  It should be
         * user-specifiable */
        timeout += 5*MICRO;
        newConnections.push_back(
            pair<Socket::Ptr, timeval>(newConnection, timeout)
          );
      }
    }

    /* Now check the incoming TCP connections to see if any have sent messages
     * yet */
    if (!newConnections.empty()) {
      timeval timeNow;
      timeUtils_getTime(&timeNow);
      for (list<pair<Socket::Ptr, timeval> >::iterator conn =
          newConnections.begin(); conn != newConnections.end(); ) {
        if ((bytesReceived =
            conn->first->receive(buf, BUFFER_LEN))) {
          IArchive messageArchive(buf, bytesReceived);
          Message message(messageArchive);
          if (message.isRealMessage()) {
            switch (message.getType()) {
              case messageType_join:
                {
                  JoinMessageData data = message.getJoinData();
                  out << "Considering adding new client on existing connection"
                    "\n";
                  addClient(data.getAddress(), "", conn->first);
                  conn = newConnections.erase(conn);
                }
                break;
              default:
                out << "Unexpected MessageType: " << message.getType() << "\n";
                ++conn;
                break;
            }
          } else {
            out << "Unrecognized message received\n";
            ++conn;
          }
        } else {
          /* If we've reached the timeout for this connection, then delete it
           * */
          if (conn->second < timeNow) {
            out << "Connection timed out, dropping\n";
            conn = newConnections.erase(conn);
          } else {
            ++conn;
          }
        }
      }
    }
    
    /* Process client messages */
    handleClientMessages();

    /* Process server actions */
    handlePendingActions();

    if (gameToAdvertiseChanged) {
#ifndef DISABLE_AVAHI
      publisher.game_changed();
#endif
      gameToAdvertiseChanged = false;
    }
    
    timeUtils_sleep(sleepTime);
    if (dots) {
      out << "." << flush;
    }
  } while (!(interrupted || startGame));

  if (!interrupted) {
    out << "Transitioning to gameplay state" << endl;

    /* Close the join sockets because we aren't going to
     * accept any more clients */
    /** \todo Maybe we can support dynamic join for people whose clients crash,
     * adding observers in mid-game, etc. */
#ifndef DISABLE_UNIX_SOCKETS
    unixSocket->close();
#endif
    if (tcpSocket != NULL) {
      tcpSocket->close();
    }

    /* Remove settings tree manipulation permissions from all clients, except
     * for some which remain in the 'playtime' group so that they can do such
     * things as pause the game */
    for (hash_map<ClientId, RemoteClient*>::iterator clientIt =
        clients.begin(); clientIt != clients.end(); ++clientIt) {
      RemoteClient* client = clientIt->second;
      client->send(new GameStartMessageData(
            client->getPlayerId(), map->getTopology(),
            map->getTopRight(), map->getBottomLeft(),
            map->getGravity(), map->getHeightfield()
          ));
      bool wasAdmin = client->hasGroup("admin");
      client->clearGroups();
      client->addGroup("world");
      if (wasAdmin) {
        client->addGroup("playtime");
      }
    }

    /** \todo The part where we wait for the clients to initialize themselves
     * */
    
    gameStarted = true;

    /* Now we have the real game loop */
    new CompleteWorld(*map, mapPlayMode, players);
      /* constructor assigns itself to global variable, so no need for
       * assignment here */

    /* Indicate game start to listeners (Don't need to pass any arguments
     * because they can just use sakusen::server::world) */
    GetPtr<Listener> gp;
    for (hash_map<MaskedPtr<Listener>, Listener::VPtr>::iterator listener =
        listeners.begin(); listener != listeners.end(); ++listener) {
      listener->second.apply_visitor(gp)->gameStart();
    }
    
    /* Note: henceforth, do not use the local field 'players', because world has
     * a copy of it - that's the one that must be worked with */
    struct timeval timeNow;
    struct timeval timeForNextTick;
    bool warnedLastTime = false;
    
    while (!interrupted) {
      /* find out when we aim to start the next tick */
      timeUtils_getTime(&timeNow);
      timeForNextTick = timeNow + gameSpeed;

      /* Handle client messages (including accepting incoming orders into
       * players' order queues) */
      handleClientMessages();

      handlePendingActions();

      /* Tell listeners that this is all the messages we'll be receiving for
       * this tick */
      for (hash_map<MaskedPtr<Listener>, Listener::VPtr>::iterator listener =
          listeners.begin(); listener != listeners.end(); ++listener) {
        listener->second.apply_visitor(gp)->ticksMessagesDone();
      }
      
      /* Do the game's stuff */
      sakusen::server::world->incrementGameState();

      /* Send out updates */
      for (hash_map<ClientId, RemoteClient*>::iterator clientIt=clients.begin();
          clientIt!=clients.end(); ) {
        RemoteClient* client = clientIt->second;
        bool clientRemoved = false;
        
        try {
          client->flushOutgoing(sakusen::server::world->getTimeNow());
        } catch (SocketExn& e) {
          out << "Removing client " << client->getClientId().toString() <<
            " due to causing SocketExn: " << e.message;
          removeClient(client);
          clientRemoved = true;
        }
        
        if (clientRemoved) {
          /* erasing invalidates the iterator */
          clientIt = clients.begin();
        } else {
          ++clientIt;
        }
      }

      /* Tell listeners that this is the end of this tick */
      for (hash_map<MaskedPtr<Listener>, Listener::VPtr>::iterator listener =
          listeners.begin(); listener != listeners.end(); ++listener) {
        listener->second.apply_visitor(gp)->tickDone();
      }
      
      if (dots && sakusen::server::world->getTimeNow() % 16 == 0) {
        out << "." << flush;
      }

      /* Sleep until it's time for the next tick */
      timeUtils_getTime(&timeNow);
      if (timeNow < timeForNextTick || gameSpeed == 0) {
        timeUtils_sleep(timeForNextTick-timeNow);
        warnedLastTime = false;
      } else {
        if (!warnedLastTime) {
          out << "Warning: server is lagging\n";
          warnedLastTime = true;
        }
      }
    }

    delete sakusen::server::world;
  }
  
  if (interrupted) {
    out << "Server interrupted.  Shutting down." << endl;
    while (!clients.empty()) {
      try {
        clients.begin()->second->send(
            new KickMessageData("Server shutting down.")
          );
      } catch (SocketExn& e) {
        out << "Socket exception while kicking client: " << e.message << "\n";
      }
      removeClient(clients.begin()->second);
    }
    return;
  }
}

/** \brief Request an (asynchronous) check for game start
 *
 * This causes the server (when it next has an opportunity) to check whther the
 * game can be started, and, if it can, then start the game.  The check is
 * asynchronous, in the sense that this method returns before it has been
 * performed.
 *
 * This is called when something has reason to believe that it might now be
 * possible for the game to start (e.g. a client calls because they just set
 * their ready flag). */
void Server::checkForGameStart()
{
  pendingActions.push_back(new CheckForGameStartAction());
}

/** \brief Requests an (asynchronous) check to see whether an admin exists
 *
 * This causes the server (when it next has an opportunity) to check whether
 * any client has admin status.  If there is no such client, and one is willing
 * to assume admin status, then that client is automatically promoted to admin
 * status.
 * 
 * This is called if any client loses admin status or leaves or permits admin
 * status. */
void Server::ensureAdminExists()
{
  pendingActions.push_front(new EnsureAdminExistsAction());
}

/** \brief Add a listener that intercepts some fraction of server-client
 * communications */
void Server::registerListener(
    const sakusen::server::plugins::Listener::VPtr& listener
  )
{
  listeners.insert(make_pair(MaskedPtr<Listener>(listener), listener));
}

/** \brief Remove a previously registered listener */
void Server::unregisterListener(
    const MaskedPtr<sakusen::server::plugins::Listener>& listener
  )
{
  listeners.erase(listener);
}

/* The following functions suffer greatly from code duplication.
 * Unfortunately, the only alternative I could see involved simultaneously
 * abusing the template system and the preprocessor.  This seemed unwise. */

String Server::boolSettingAlteringCallback(
    settingsTree::Leaf* altering,
    bool newValue
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
    ClientId id = ClientId::fromString(fullName.front());
    assert(clients.count(id));
    fullName.pop_front();
    return clients[id]->performBoolMagic(/*altering,*/ fullName, newValue);
  } else if (fullName.front() == "players") {
    /* If the change is inside a player branch, then forward the magic to the
     * player object */
    fullName.pop_front();
    assert(!fullName.empty());
    //PlayerId id = playerId_fromString(fullName.front());
    fullName.pop_front();
    //Player& player = players[id];
    assert(!fullName.empty());
    if (fullName.front() == "maxclients") {
      Fatal("maxclients not bool setting");
    } else {
      Fatal("unexpected child of player branch: " << fullName.front());
    }
  } else if (fullName.front() == "server") {
    /* It's in the server branch, so we need to deal with it */
    fullName.pop_front();
    assert(!fullName.empty());
    if (fullName.front() == "allowobservers") {
      setAllowObservers(newValue);
      return "";
    } else if (fullName.front() == "application") {
      /* Do nothing */
      return "";
    } else {
      Fatal("unexpected child of server branch: " << fullName.front());
    }
  } else if (fullName.front() == "game") {
    fullName.pop_front();
    assert(!fullName.empty());
    if (fullName.front() == "speed") {
      Fatal("speed not a bool setting");
    } else {
      Fatal("unexpected child of game branch: " << fullName.front());
    }
  } else if (fullName.front() == "plugins") {
    fullName.pop_front();
    assert(!fullName.empty());
    hash_map_string<Plugin::Ptr>::type::iterator plugin =
      plugins.find(fullName.front());
    assert(plugin != plugins.end());
    fullName.pop_front();
    assert(!fullName.empty());
    return plugin->second->settingAlterationCallback<bool>(altering, newValue);
  } else {
    Fatal("unexpected child of root node: " << fullName.front());
  }
}

String Server::stringSettingAlteringCallback(
    settingsTree::Leaf* altering,
    const String& newValue
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
    ClientId id = ClientId::fromString(fullName.front());
    assert(clients.count(id));
    fullName.pop_front();
    return clients[id]->performStringMagic(/*altering,*/ fullName, newValue);
  } else if (fullName.front() == "players") {
    /* If the change is inside a player branch, then forward the magic to the
     * player object */
    fullName.pop_front();
    assert(!fullName.empty());
    //PlayerId id = playerId_fromString(fullName.front());
    fullName.pop_front();
    //Player& player = players[id];
    assert(!fullName.empty());
    if (fullName.front() == "maxclients") {
      Fatal("maxclients not string setting");
    } else {
      Fatal("unexpected child of player branch: " << fullName.front());
    }
  } else if (fullName.front() == "server") {
    /* It's in the server branch, so we need to deal with it */
    fullName.pop_front();
    assert(!fullName.empty());
    if (fullName.front() == "allowobservers") {
      Fatal("allowobservers not string setting");
    } else if (fullName.front() == "application") {
      /* Do nothing */
      return "";
    } else {
      Fatal("unexpected child of server branch: " << fullName.front());
    }
  } else if (fullName.front() == "game") {
    fullName.pop_front();
    assert(!fullName.empty());
    if (fullName.front() == "speed") {
      Fatal("speed not a string setting");
    } else if (fullName.front() == "universe") {
      fullName.pop_front();
      assert(!fullName.empty());
      if (fullName.front() == "name") {
        if (universe != NULL && newValue == universePath) {
          return "";
        }
        ResourceSearchResult result;
        Universe::Ptr u;
        String uPath;
        tie(u, result, uPath) =
          resourceInterface->search<Universe>(newValue);
        switch(result) {
          case resourceSearchResult_success:
            assert(u);
            /* File an asynchronous request to use the universe */
            pendingActions.push_front(new RequestUniverseAction(u, uPath));
            return "";
          case resourceSearchResult_ambiguous:
            return "specified universe name ambiguous";
          case resourceSearchResult_notFound:
            return "specified universe name not found";
          case resourceSearchResult_error:
            return String("error finding resource: ") +
              resourceInterface->getError();
          default:
            Fatal("unexpected ResourceSearchResult: " << result);
        }
      } else if (fullName.front() == "hash") {
        return "";
      } else {
        Fatal("unexpected child of universe branch: " << fullName.front());
      }
    } else if (fullName.front() == "map") {
      if (universe == NULL) {
        return "must specify a universe before map";
      }
      if (map != NULL && newValue == mapPath) {
        return "";
      }
      ResourceSearchResult result;
      MapTemplate::Ptr m;
      String mPath;
      boost::tie(m, result, mPath) =
        resourceInterface->search<MapTemplate>(newValue, universe);
      switch(result) {
        case resourceSearchResult_success:
          assert(m);
          /* File an asynchronous request to use the map */
          pendingActions.push_front(new RequestMapAction(m, mPath));
          return "";
        case resourceSearchResult_ambiguous:
          return "specified map template name ambiguous";
        case resourceSearchResult_notFound:
          return "specified map template name not found";
        case resourceSearchResult_error:
          return String("error finding resource: ") +
            resourceInterface->getError();
        default:
          Fatal("unexpected ResourceSearchResult: " << result);
      }
    } else {
      Fatal("unexpected child of game branch: " << fullName.front());
    }
  } else if (fullName.front() == "plugins") {
    fullName.pop_front();
    assert(!fullName.empty());
    hash_map_string<Plugin::Ptr>::type::iterator plugin =
      plugins.find(fullName.front());
    assert(plugin != plugins.end());
    fullName.pop_front();
    assert(!fullName.empty());
    return plugin->second->
      settingAlterationCallback<String>(altering, newValue);
  } else {
    Fatal("unexpected child of root node: " << fullName.front());
  }
}

String Server::stringSetSettingAlteringCallback(
    fuseki::settingsTree::Leaf* altering,
    const std::set<String>& newValue
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
    ClientId id = ClientId::fromString(fullName.front());
    assert(clients.count(id));
    fullName.pop_front();
    return clients[id]->performStringSetMagic(fullName, newValue);
  } else if (fullName.front() == "players") {
    /* If the change is inside a player branch, then forward the magic to the
     * player object */
    fullName.pop_front();
    assert(!fullName.empty());
    //PlayerId id = playerId_fromString(fullName.front());
    fullName.pop_front();
    //Player& player = players[id];
    assert(!fullName.empty());
    if (fullName.front() == "maxclients") {
      Fatal("maxclients not string list setting");
    } else {
      Fatal("unexpected child of player branch: " << fullName.front());
    }
  } else if (fullName.front() == "server") {
    /* It's in the server branch, so we need to deal with it */
    fullName.pop_front();
    assert(!fullName.empty());
    if (fullName.front() == "allowobservers") {
      Fatal("allowobserers not string set setting");
    } else if (fullName.front() == "application") {
      Fatal("application not string set setting");
    } else if (fullName.front() == "blockedaddresses") {
      /* Do nothing */
      return "";
    } else if (fullName.front() == "plugins") {
      checkPlugins(newValue);
      return "";
    } else {
      Fatal("unexpected child of server branch: " << fullName.front());
    }
  } else if (fullName.front() == "game") {
    fullName.pop_front();
    assert(!fullName.empty());
    if (fullName.front() == "speed") {
      Fatal("speed not a string set setting");
    } else {
      Fatal("unexpected child of game branch: " << fullName.front());
    }
  } else if (fullName.front() == "plugins") {
    fullName.pop_front();
    assert(!fullName.empty());
    hash_map_string<Plugin::Ptr>::type::iterator plugin =
      plugins.find(fullName.front());
    assert(plugin != plugins.end());
    fullName.pop_front();
    assert(!fullName.empty());
    return plugin->second->
      settingAlterationCallback<set<String> >(altering, newValue);
  } else {
    Fatal("unexpected child of root node: " << fullName.front());
  }
}

/** \brief Called after a leaf of the settings tree had changed its value
 *
 * This method informs all clients of the change to the settings tree and also
 * clears the readiness flag on any clients who had requested that it be
 * automatically cleared when a setting was changed. */
void Server::settingAlteredCallback(Leaf* altered)
{
  String fullName = altered->getFullName();
  bool isReadinessChange =
    pcrecpp::RE(":clients:[0-9]+:ready").FullMatch(fullName);
  NotifySettingMessageData data(fullName, true, altered->getValue());
  
  /* Inform everyone with read permission that the setting was altered */
  for (__gnu_cxx::hash_map<ClientId, RemoteClient*>::iterator
      clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
    RemoteClient* client = clientIt->second;
    if (client->hasReadPermissionFor(altered)) {
      try {
        client->send(new NotifySettingMessageData(data));
        
        /* clear readiness flag if desired */
        if (client->isAutoUnready() && !isReadinessChange) {
          changeInClientBranch(client, "ready", "false");
        }
      } catch (SocketExn&) {
        out << "Marking client " << client->getClientId().toString() <<
          " as dead due to causing SocketExn\n";
        client->setDead();
      }
    }
  }
}

