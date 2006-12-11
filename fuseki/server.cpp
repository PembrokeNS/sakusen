#include "server.h"

#include "libsakusen-global.h"
#include "resourcesearchresult.h"
#include "libsakusen-comms-global.h"
#include "message.h"
#include "remoteclient.h"
#include "socketexn.h"
#include "resourceinterface-methods.h"
#include "completeworld.h"
#include "fuseki-global.h"
#include "settingstree/stringlistleaf.h"
#include "settingstree/intleaf.h"
#include "settingstree/boolleaf.h"
#include "timeutils.h"
#include "revision.h"

#include <time.h>
#include <signal.h>

#include <ostream>
#include <pcrecpp.h>

using namespace std;
using namespace __gnu_cxx;

using namespace sakusen;
using namespace sakusen::server;
using namespace sakusen::comms;
using namespace fuseki;
using namespace fuseki::settingsTree;

Server::Server(
    std::ostream& o,
    const ResourceInterface::Ptr& r,
#ifndef DISABLE_UNIX_SOCKETS
    bool a,
    const Socket::Ptr& unS,
#endif
    const Socket::Ptr& udS,
    const Socket::Ptr& tS,
    bool d
  ) :
  SettingsUser("server"),
  dots(d),
#ifndef DISABLE_UNIX_SOCKETS
  abstract(a),
  unixSocket(unS),
#endif
  udpSocket(udS),
  tcpSocket(tS),
  out(o),
  resourceInterface(r),
  clients(10),
  universe(NULL),
  map(NULL),
  mapPlayMode(0),
  players(),
  settings(this),
  allowObservers(false),
  checkForGameStartNextTime(false),
  ensureAdminExistsNextTime(false),
  requestedUniverse(NULL),
  requestedMap(NULL),
  mapPlayModeChanged(false),
  gameSpeed(DEFAULT_GAME_SPEED)
{
  String reason;
  reason = settings.changeRequest(
      "server:application:name",
      APPLICATION_NAME,
      this
    );
  assert(reason == "");
  reason = settings.changeRequest(
      "server:application:version",
      APPLICATION_VERSION,
      this
    );
  assert(reason == "");
  reason = settings.changeRequest(
      "server:application:revision",
#ifdef REVISION
      numToString(REVISION),
#else
      "unknown",
#endif
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

  /* The following, on the other hand, really need to be here */
  delete map;
  delete universe;
  delete requestedMap;
  delete requestedUniverse;
  
  universe = NULL;
  map = NULL;
  requestedUniverse = NULL;
  requestedMap = NULL;
}

void Server::advertise(
    const SolicitMessageData& data,
    const String& receivedFrom,
    const Socket::Ptr& receivedOn,
    const Message& advertisement
  )
{
  String address = data.getAddress();
  bool respondOnExisting = false;
  if (address.empty()) {
    /* If no address specified then we respond to the source of the
     * message and use the existing solicitation socket */
    address = receivedFrom;
    respondOnExisting = true;
  }
  Socket::Ptr responseSocket;
  if (respondOnExisting) {
    responseSocket = receivedOn;
  } else {
    responseSocket = Socket::newConnectionToAddress(address);
  }
  if (NULL != responseSocket) {
    out << "Advertising to " << address << ".\n";
    if (respondOnExisting) {
      responseSocket->sendTo(advertisement, address);
    } else {
      responseSocket->send(advertisement);
    }
  } else {
    out << "Unsupported socket type requested by client "
      "(address was '" << address << "').\n";
  }
}

ClientID Server::getFreeClientID()
{
  /* Just use a hopelessly naive algorithm for allocating IDs.  Note that this
   * assumes that ClientID is an integer type (or at least sufficiently similar
   * for this algorithm to work properly) */
  ClientID i = 0;
  do {
    if (0 == clients.count(i)) {
      return i;
    }
    i++;
  } while (i != static_cast<ClientID>(-1));

  /* Return of -1 indicates no free IDs */
  return static_cast<ClientID>(-1);
}

void Server::addClient(
    const String& requestedAddress,
    const String& fromAddress,
    const Socket::Ptr& existingSocket
  )
{
  /* Check whether this address is blocked */
  hash_set<String, StringHash> blockedAddresses =
    dynamic_cast<StringListLeaf*>(
        settings.getNode("server:blockedaddresses")
      )->getValueAsList();
  for (hash_set<String, StringHash>::iterator i=blockedAddresses.begin();
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
  ClientID id = getFreeClientID();
  if (id == static_cast<ClientID>(-1)) {
    /* No free IDs */
    out << "Rejecting join request due to lack of space for more clients.\n";
    socket->send(RejectMessageData("No space for more clients"));
    return;
  }
  /* Add the client's branch of the settings tree */
  settings.getClientsBranch()->addClient(id);
  clients[id] =
    new RemoteClient(id, this, socket, requestedAddress!=""
#ifndef DISABLE_UNIX_SOCKETS
        , abstract
#endif
      );
}

void Server::removeClient(RemoteClient* client)
{
  settings.getClientsBranch()->removeClient(client->getId());
  clients.erase(clients.find(client->getId()));
  delete client;
  ensureAdminExists();
}

void Server::handleClientMessages()
{
  for (hash_map<ClientID, RemoteClient*>::iterator clientIt=clients.begin();
      clientIt!=clients.end(); ) {
    RemoteClient* client = clientIt->second;
    bool clientRemoved = false;
    
    try {
      client->flushIncoming();
      
      while (!client->messageQueueEmpty()) {
        const Message& message = client->messageQueuePopFront();
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
              String value;
              const Node* node = NULL;
              reason = settings.getRequest(setting, value, node, client);
              if (reason != "") {
                out << "Request rejected (" << reason << ")\n";
                client->send(RejectMessageData(reason));
              } else {
                /* We send back node->getFullName() rather than just setting to
                 * ensure that it is in canonical form */
                client->send(
                    NotifySettingMessageData(node->getFullName(), value)
                  );
              }
            }
            break;
          case messageType_changeSetting:
            {
              ChangeSettingMessageData data = message.getChangeSettingData();
              out << "Client asked to change setting " << data.getSetting() <<
                "\n";
              String reason;
              if (!(reason = settings.changeRequest(
                    data.getSetting(), data.getValue(), client
                  )).empty()) {
                /* That a non-empty string was returned implies that a problem
                 * occured.  We tell the client as much */
                out << "Request rejected (" << reason << ")\n";
                client->send(RejectMessageData(reason));
              }
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
      out << "Removing client " << clientID_toString(client->getId()) <<
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
}

void Server::clearPlayers()
{
  for (hash_map<ClientID, RemoteClient*>::iterator clientIt = clients.begin();
      clientIt != clients.end(); ++clientIt) {
    RemoteClient* client = clientIt->second;
    if (client->getPlayerId() != 0) {
      changeInClientBranch(client, "player", "0");
    }
  }
  while (!players.empty()) {
    PlayerID id = static_cast<PlayerID>(players.size()-1);
    players.pop_back();
    settings.getPlayersBranch()->removePlayer(id);
  }
}

void Server::createPlayersFor(const MapPlayMode& mode)
{
  assert(players.empty());
  for (uint32 i=0; i<mode.getMaxPlayers(); i++) {
    const PlayerTemplate& player = mode.getPlayer(i);
    players.push_back(Player(player));
    settings.getPlayersBranch()->addPlayer(i, player);
  }
}

void Server::setAllowObservers(bool value)
{
  if (allowObservers && !value) {
    /* In this case, we may have to dump some clients who have themselves
     * registered as observers */
    for (hash_map<ClientID, RemoteClient*>::iterator client = clients.begin();
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
  if ("" != settings.changeRequest(
      String("clients:") + clientID_toString(client->getId()) +
        ":" + node, value, this
    )) {
      Fatal("something has gone wrong with the settings tree");
  }
}

/** \brief Converts a PlayerID into a pointer to a player
 *
 * \param id PlayerID to convert.
 * \return Pointer to Player with given id.
 *
 * Throws InvalidPlayerID if the ID is invalid.
 */
Player* Server::getPlayerPtr(PlayerID id)
{
  if (sakusen::server::world == NULL) {
    if (id >= players.size())
      throw InvalidPlayerID(id);
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
  /** \bug signal is deprecated.  Use sigaction(2) instead */
  signal(SIGINT, &interruptHandler);
  signal(SIGTERM, &interruptHandler);
  out << "Server started.  Hit Ctrl+C to interrupt and shut down server.\n";
  
#ifndef DISABLE_UNIX_SOCKETS
  unixSocket->setNonBlocking(true);
#endif
  if (udpSocket != NULL) {
    udpSocket->setNonBlocking(true);
  }
  if (tcpSocket != NULL) {
    tcpSocket->setNonBlocking(true);
  }
  Message advertisement = Message(AdvertiseMessageData("fuseki", "Game name"));

  /* A list of incoming (probably TCP) connections where we expect to get join
   * messages from prospective clients */
  list<pair<Socket::Ptr, timeval> > newConnections;
  
  /* A bool to get us out of the loop when it is time to start the game */
  bool startGame = false;
  
  /* To begin with we have the game init loop */
  do {
    size_t bytesReceived;
    String receivedFrom;
#ifndef DISABLE_UNIX_SOCKETS
    /* Look for messages on the unix socket, which can be solicit or join
     * messages */
    if ((bytesReceived =
          unixSocket->receiveFrom(buf, BUFFER_LEN, receivedFrom))) {
      Message message = Message(buf, bytesReceived);
      if (message.isRealMessage()) {
        switch (message.getType()) {
          case messageType_solicit:
            {
              SolicitMessageData data = message.getSolicitData();
                advertise(
                    data, receivedFrom, unixSocket,
                    advertisement
                  );
            }
            break;
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
    
    /* Look for solicit messages on the udp socket */
    if (udpSocket != NULL) {
      if ((bytesReceived =
            udpSocket->receiveFrom(buf, BUFFER_LEN, receivedFrom))) {
        Message message = Message(buf, bytesReceived);
        if (message.isRealMessage()) {
          switch (message.getType()) {
            case messageType_solicit:
              {
                SolicitMessageData data = message.getSolicitData();
                advertise(
                    data, receivedFrom, udpSocket,
                    advertisement
                  );
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
    }
    
    /* If we have a seperate tcp socket then check for messages there too */
    if (tcpSocket != NULL) {
      while (true) {
        Socket::Ptr newConnection = tcpSocket->accept();
        if (newConnection == NULL) {
          break;
        }
        
        newConnection->setNonBlocking(true);
        timeval timeout;
        timeUtils_getTime(&timeout);
        /** \todo We use a straight one second timeout.  It should be
         * user-specifiable */
        timeout += MICRO;
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
          Message message = Message(buf, bytesReceived);
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
          if (conn->second > timeNow) {
            conn = newConnections.erase(conn);
          } else {
            ++conn;
          }
        }
      }
    }
    
    /* Process client messages */
    handleClientMessages();

    if (ensureAdminExistsNextTime) {
      out << "Checking for existence of an admin\n";
      RemoteClient* adminCandidate = NULL;
      bool needAdmin = true;

      for (hash_map<ClientID, RemoteClient*>::iterator client = clients.begin();
          client != clients.end(); client++) {
        if (client->second->isAdmin()) {
          needAdmin = false;
          break;
        }
        if (adminCandidate == NULL && !client->second->isNeverAdmin()) {
          adminCandidate = client->second;
        }
      }

      if (needAdmin && adminCandidate != NULL) {
        out << "Promoting a client to admin status";
        changeInClientBranch(adminCandidate, "admin", "true");
      }
      
      ensureAdminExistsNextTime = false;
    }

    if (requestedUniverse != NULL) {
      delete universe;
      universe = requestedUniverse;
      requestedUniverse = NULL;
      String reason = settings.changeRequest(
          "game:universe:name", universe->getInternalName(), this
        );
      assert(reason == "");
      reason = settings.changeRequest(
          "game:universe:hash", universe->getHash(), this
        );
      assert(reason == "");
    }
    
    if (requestedMap != NULL) {
      delete map;
      map = requestedMap;
      requestedMap = NULL;
      String reason = settings.changeRequest(
          "game:map", map->getInternalName(), this
        );
      assert(reason == "");
      setMapPlayMode(0);
    }

    if (mapPlayModeChanged) {
      clearPlayers();
      createPlayersFor(map->getPlayModes()[mapPlayMode]);
      checkForGameStart();
      
      mapPlayModeChanged = false;
    }

    /* If we asked ourselves to, then check whether the game can start now */
    if (checkForGameStartNextTime) {
      out << "Checking for game start\n";
      /* Check that we have:
       * - A map and mapplaymode selected
       * - Races selected for all players
       * - A client for all players that need one
       * - All clients ready
       * - All clients either observers or else assigned to some player
       */
      if (map == NULL) {
        out << "Not ready because no map selected";
      } else {
        bool allPlayersReady = true;
        for (vector<Player>::iterator player = players.begin();
            player != players.end(); player++) {
          if (!player->isReadyForGameStart()) {
            allPlayersReady = false;
            out << "Not ready because player ";
            if (player->getName() == "") {
              out << (player - players.begin());
            } else {
              out << "'" << player->getName() << "'";
            }
            out << " not ready\n";
            break;
          }
        }
        if (allPlayersReady) {
          bool allClientsReady = true;
          for (hash_map<ClientID, RemoteClient*>::iterator client =
              clients.begin(); client != clients.end(); client++) {
            if (!client->second->isReadyForGameStart()) {
              allClientsReady = false;
              out << "Not ready because client '" <<
                clientID_toString(client->second->getId()) <<
                "' not ready\n";
              break;
            }
          }
          startGame = allClientsReady;
        }
      }
      checkForGameStartNextTime = false;
    }
    
    timeUtils_sleep(sleepTime);
    if (dots) {
      out << "." << flush;
    }
  } while (!(interrupted || startGame));

  if (!interrupted) {
    out << "Transitioning to gameplay state" << endl;

    /* Close the solicitation and join socket because we aren't going to
     * accept any more clients */
    /** \todo Maybe we can support dynamic join for people whose clients crash,
     * adding observers in mid-game, etc. */
#ifndef DISABLE_UNIX_SOCKETS
    unixSocket->close();
#endif
    if (udpSocket != NULL) {
      udpSocket->close();
    }
    if (tcpSocket != NULL) {
      tcpSocket->close();
    }

    /* Remove settings tree manipulation permissions from all clients, except
     * for some which remain in the 'playtime' group so that they can do such
     * things as pause the game */
    for (hash_map<ClientID, RemoteClient*>::iterator clientIt =
        clients.begin(); clientIt != clients.end(); ++clientIt) {
      RemoteClient* client = clientIt->second;
      client->send(GameStartMessageData(
            client->getPlayerId(), map->getTopology(),
            map->getTopRight(), map->getBottomLeft(),
            map->getGravity(), map->getHeightfield().getHorizontalResolution(),
            map->getHeightfield().getVerticalResolution()
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
    
    /* Now we have the real game loop */
    new CompleteWorld(*map, mapPlayMode, players);
      /* constructor assigns itself to global variable, so no need for
       * assignment here */
    
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
      
      /* Do the game's stuff */
      sakusen::server::world->incrementGameState();

      /* Send out updates */
      for (hash_map<ClientID, RemoteClient*>::iterator clientIt=clients.begin();
          clientIt!=clients.end(); ) {
        RemoteClient* client = clientIt->second;
        bool clientRemoved = false;
        
        try {
          client->flushOutgoing(sakusen::server::world->getTimeNow());
        } catch (SocketExn& e) {
          out << "Removing client " << clientID_toString(client->getId()) <<
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
        clients.begin()->second->send(KickMessageData("Server shutting down."));
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
  checkForGameStartNextTime = true;
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
  ensureAdminExistsNextTime = true;
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
    ClientID id = clientID_fromString(fullName.front());
    assert(clients.count(id));
    fullName.pop_front();
    return clients[id]->performBoolMagic(/*altering,*/ fullName, newValue);
  } else if (fullName.front() == "players") {
    /* If the change is inside a player branch, then forward the magic to the
     * player object */
    fullName.pop_front();
    assert(!fullName.empty());
    //PlayerID id = playerID_fromString(fullName.front());
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
    ClientID id = clientID_fromString(fullName.front());
    assert(clients.count(id));
    fullName.pop_front();
    return clients[id]->performStringMagic(/*altering,*/ fullName, newValue);
  } else if (fullName.front() == "players") {
    /* If the change is inside a player branch, then forward the magic to the
     * player object */
    fullName.pop_front();
    assert(!fullName.empty());
    //PlayerID id = playerID_fromString(fullName.front());
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
        if (universe != NULL && newValue == universe->getInternalName()) {
          return "";
        }
        ResourceSearchResult result;
        Universe* u = resourceInterface->search<Universe>(
            newValue, NULL, &result
          );
        switch(result) {
          case resourceSearchResult_success:
            assert(u != NULL);
            delete requestedUniverse;
            requestedUniverse = u; /* File an asynchronous request to use the
                                      universe */
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
      if (map != NULL && newValue == map->getInternalName()) {
        return "";
      }
      ResourceSearchResult result;
      MapTemplate* m = resourceInterface->search<MapTemplate>(
          newValue, universe, &result
        );
      switch(result) {
        case resourceSearchResult_success:
          assert(m != NULL);
          delete requestedMap;
          requestedMap = m; /* File an asynchronous request to use the map */
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
  } else {
    Fatal("unexpected child of root node: " << fullName.front());
  }
}

String Server::stringSetSettingAlteringCallback(
    fuseki::settingsTree::Leaf* altering,
    const __gnu_cxx::hash_set<String, sakusen::StringHash>& newValue
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
    ClientID id = clientID_fromString(fullName.front());
    assert(clients.count(id));
    fullName.pop_front();
    return clients[id]->performStringSetMagic(fullName, newValue);
  } else if (fullName.front() == "players") {
    /* If the change is inside a player branch, then forward the magic to the
     * player object */
    fullName.pop_front();
    assert(!fullName.empty());
    //PlayerID id = playerID_fromString(fullName.front());
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
      /* Do nothing */
      return "";
    } else if (fullName.front() == "blockedaddresses") {
      /* Do nothing */
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
  NotifySettingMessageData data(fullName, altered->getValue());
  list<RemoteClient*> deadClients;
  
  /* Inform everyone with read permission that the setting was altered */
  for (__gnu_cxx::hash_map<ClientID, RemoteClient*>::iterator
      clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
    RemoteClient* client = clientIt->second;
    if (client->hasReadPermissionFor(altered)) {
      try {
        client->send(data);
        
        /* clear readiness flag if desired */
        if (client->isAutoUnready() && !isReadinessChange) {
          changeInClientBranch(client, "ready", "false");
        }
      } catch (SocketExn& e) {
        deadClients.push_back(client);
      }
    }
  }

  while (!deadClients.empty()) {
    out << "Removing client " <<
      clientID_toString(deadClients.front()->getId()) <<
      "due to causing a SocketExn\n";
    removeClient(deadClients.front());
    deadClients.pop_front();
  }
}

