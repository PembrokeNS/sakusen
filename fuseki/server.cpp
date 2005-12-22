#include "server.h"

#include "libsakusen-global.h"
#include "resourcesearchresult.h"
#include "libsakusen-comms-global.h"
#include "message.h"
#include "unixdatagramconnectingsocket.h"
#include "remoteclient.h"
#include "socketexception.h"
#include "resourceinterface-methods.h"
#include "fuseki-global.h"
#include "settingstree/stringlistleaf.h"
#include "settingstree/intleaf.h"
#include "settingstree/boolleaf.h"

#include <time.h>
#include <signal.h>

#include <ostream>
#include <pcrecpp.h>

#define NANO 1000000000 /* Nanoseconds in a second */

using namespace std;
using namespace __gnu_cxx;

using namespace fuseki;
using namespace fuseki::settingsTree;

bool interrupted = false; /* Use of static data to monitor interrupt of
                             server instance (c.f. interruptHandler below) */

Server::Server(Socket* s, ostream& o, ResourceInterface* r, bool a, bool d) :
  SettingsUser("server"),
  abstract(a),
  dots(d),
  socket(s),
  out(o),
  resourceInterface(r),
  clients(),
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
      "server:application:id",
      APPLICATION_ID,
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

  delete universe;
  delete map;
  delete requestedUniverse;
  delete requestedMap;
  
  universe = NULL;
  map = NULL;
  requestedUniverse = NULL;
  requestedMap = NULL;
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

void Server::addClient(const String& address)
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
    if (pcrecpp::RE(*i).FullMatch(address)) {
      /* We matched a blocked address, so return without even bothering to
       * reject them. */
      out << "Ignoring join request because address is blocked.\n";
      return;
    }
  }
  Socket* socket = Socket::newConnectionToAddress(address);
  ClientID id = getFreeClientID();
  if (id == static_cast<ClientID>(-1)) {
    /* No free IDs */
    out << "Rejecting join request due to lack of space for more clients.\n";
    socket->send(RejectMessageData("No space for more clients"));
    delete socket;
    return;
  }
  /* Add the client's branch of the settings tree */
  settings.getClientsBranch()->addClient(id);
  clients[id] = new RemoteClient(id, this, socket, abstract);
}

void Server::removeClient(RemoteClient* client)
{
  settings.getClientsBranch()->removeClient(client->getID());
  clients.erase(clients.find(client->getID()));
  delete client;
  ensureAdminExists();
}

void Server::setAllowObservers(bool value)
{
  if (allowObservers && !value) {
    /* In this case, we may have to dump some clients who have themselves
     * registered as observers */
    for (hash_map<ClientID, RemoteClient*>::iterator client = clients.begin();
        client != clients.end(); client++) {
      if (client->second->isObserver()) {
        if ("" != 
            settings.changeRequest(
              String("clients:") + clientID_toString(client->second->getID()) +
                ":observer",
              "false",
              this
            )) {
          Fatal("something has gone wrong with the settings tree");
        }
      }
    }
  }

  allowObservers = value;
}

/* Handler for interrupt signals from the keyboard, so that the server can
 * shutdown gracefully.  Because signals are an inherently C thing, there seems
 * to be no nice way to handle them in an OO way, and thus I am reduced to
 * using static data here (c.f. definition of interrupted above).  This means
 * that if two servers are running on the same process, then interrupting
 * wither will stop both.  Of course, since the signal is sent to the process
 * as a whole, there's no other sane action anyway, and thus it's not the end
 * of the world. */
void interruptHandler(int /*signal*/)
{
  interrupted = true;
}

void Server::serve()
{
  uint8 buf[BUFFER_LEN];
  struct timespec sleepTime = {0, NANO/10};
  /* FIXME: signal is deprecated.  Use sigaction(2) instead */
  signal(SIGINT, &interruptHandler);
  signal(SIGTERM, &interruptHandler);
  out << "Server started.  Hit Ctrl+C to interrupt and shut down server.\n";
  
  socket->setAsynchronous(true);
  Message advertisement = Message(AdvertiseMessageData("fuseki", "Game name"));
  
  /* A bool to get us out of the loop when it is time to start the game */
  bool startGame = false;
  
  /* To begin with we have the game init loop */
  do {
    size_t bytesReceived;
    /* Look for messages on the solicitation socket */
    if ((bytesReceived = socket->receive(buf, BUFFER_LEN))) {
      Message message = Message(buf, bytesReceived);
      if (message.isRealMessage()) {
        switch (message.getType()) {
          case messageType_solicit:
            {
              SolicitMessageData data = message.getSolicitData();
              const String& address = data.getAddress();
              Socket* clientSocket = Socket::newConnectionToAddress(address);
              if (NULL != clientSocket) {
                out << "Advertising to " << clientSocket->getAddress() << ".\n";
                clientSocket->send(advertisement);
                delete clientSocket;
              } else {
                out << "Unsupported socket type requested by client.\n";
              }
            }
            break;
          case messageType_join:
            {
              JoinMessageData data = message.getJoinData();
              out << "Considering adding new client at " <<
                data.getAddress() << "\n";
              addClient(data.getAddress());
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
    
    /* Process client messages */
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
                reason = settings.getRequest(setting, value, client);
                if (reason != "") {
                  out << "Request rejected (" << reason << ")\n";
                  client->send(RejectMessageData(reason));
                } else {
                  client->send(NotifySettingMessageData(setting, value));
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
            default:
              out << "Unexpected MessageType " << message.getType() <<
                " from client\n";
              break;
          }
          if (clientRemoved) {
            break;
          }
        }
      } catch (SocketException* e) {
        out << "Removing client " << clientID_toString(client->getID()) <<
          " due to causing Exception: " << e->what();
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
      if (map != NULL) {
        bool allPlayersReady = true;
        for (vector<Player>::iterator player = players.begin();
            player != players.end(); player++) {
          if (!player->isReadyForGameStart()) {
            allPlayersReady = false;
            break;
          }
        }
        if (allPlayersReady) {
          bool allClientsReady = true;
          for (hash_map<ClientID, RemoteClient*>::iterator client =
              clients.begin(); client != clients.end(); client++) {
            if (!client->second->isReadyForGameStart()) {
              allClientsReady = false;
              break;
            }
          }
          startGame = allClientsReady;
        }
      }
      checkForGameStartNextTime = false;
    }

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
        if ("" != settings.changeRequest(
              String("clients:") + clientID_toString(adminCandidate->getID()) +
                ":admin",
              "true",
              this
            )) {
          Fatal("something has gone wrong with the settings tree");
        }
      }
      
      ensureAdminExistsNextTime = false;
    }

    /* TODO: Promote requestedUniverse and requestedMap */
    
    nanosleep(&sleepTime, NULL);
    if (dots) {
      out << "." << flush;
    }
  } while (!(interrupted || startGame));

  if (interrupted) {
    out << "Server interrupted.  Shutting down." << endl;
    while (!clients.empty()) {
      try {
        clients.begin()->second->send(KickMessageData("Server shutting down."));
      } catch (SocketException* e) {
        out << "Socket exception while kicking client: " << e->what() << "\n";
        delete e;
      }
      removeClient(clients.begin()->second);
    }
    return;
  }

  /* TODO: Now we have the real game loop */
  out << "At this point a game would start" << endl;
}

void Server::checkForGameStart()
{
  /* Request an (asynchronous) check for game start the next time the main loop
   * gets around to that part */
  checkForGameStartNextTime = true;
}

void Server::ensureAdminExists()
{
  /* Request a check next time around */
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
    } else if (fullName.front() == "map") {
      if (universe == NULL) {
        return "must specify a universe before map";
      }
      ResourceSearchResult result;
      Map* m = resourceInterface->search<Map>(
          newValue, universe, &result
        );
      switch(result) {
        case resourceSearchResult_success:
          assert(m != NULL);
          delete requestedMap;
          requestedMap = m; /* File an asynchronous request to use the map */
          return "";
        case resourceSearchResult_ambiguous:
          return "specified map name ambiguous";
        case resourceSearchResult_notFound:
          return "specified map name not found";
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
    settingsTree::Leaf* altering,
    const hash_set<String, StringHash>& newValue
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

void Server::settingAlteredCallback(Leaf* altered)
{
  NotifySettingMessageData data(altered->getFullName(), altered->getValue());
  list<RemoteClient*> deadClients;
  
  /* Inform everyone with read permission that the setting was altered */
  for (__gnu_cxx::hash_map<ClientID, RemoteClient*>::iterator
      client = clients.begin(); client != clients.end(); client++) {
    if (client->second->hasReadPermissionFor(altered)) {
      try {
        client->second->send(data);
        /* TODO: clear readiness flag if desired */
      } catch (SocketException* e) {
        deadClients.push_back(client->second);
      }
    }
  }

  while (!deadClients.empty()) {
    out << "Removing client " <<
      clientID_toString(deadClients.front()->getID()) <<
      "due to causing a SocketException\n";
    removeClient(deadClients.front());
    deadClients.pop_front();
  }
}

