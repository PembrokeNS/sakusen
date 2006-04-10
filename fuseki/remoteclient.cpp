#include "remoteclient.h"

#include "messagedata.h"
#include "unixdatagramlisteningsocket.h"
#include "server.h"

using namespace __gnu_cxx;

using namespace sakusen;
using namespace sakusen::comms;
using namespace fuseki;

RemoteClient::RemoteClient(
    ClientID i,
    Server* s,
    Socket* socket,
    bool createInSocket
#ifndef DISABLE_UNIX_SOCKETS
    ,
    bool abstract
#endif
  ) :
  Client(),
  SettingsUser(String("client")+clientID_toString(i)),
  id(i),
  server(s),
  inSocket(NULL),
  outSocket(socket),
  incomingMessageQueue(),
  outgoingUpdateQueue(),
  admin(false),
  neverAdmin(true),
  ready(false),
  autoUnready(false)
{
  if (createInSocket) {
#ifdef DISABLE_UNIX_SOCKETS
    Fatal("Seperate inSocket not supported on this platform");
#else
    inSocket = new UnixDatagramListeningSocket(abstract);
    outSocket->send(Message(AcceptMessageData(inSocket->getAddress(), id)));
#endif
  } else {
    inSocket = outSocket;
    outSocket->send(Message(AcceptMessageData("", id)));
  }
  inSocket->setNonBlocking(true);
}

/** \brief Destructor
 *
 * The destructor will detach this client from any player to whom it may be
 * attached, and destroy the associated sockets.  Further behavior is not
 * excluded */
RemoteClient::~RemoteClient()
{
  /* Must setPlayerId so as to detach from any player to whom we
   * might be attached, and thus avoid the Player having a dangling
   * pointer */
  setPlayerId(0, false);
  
  delete inSocket;
  if (outSocket != inSocket) {
    delete outSocket;
  }
  inSocket = NULL;
  outSocket = NULL;
}

void RemoteClient::setPlayerId(PlayerID id, bool removeGroup)
{
  if (playerId != 0) {
    server->getPlayerPtr(playerId)->detachClient(this);
    if (removeGroup) {
      /* This condition is necessary because when this method is called from
       * the destructor, this->groups may already have been cleared when the
       * game started */
      this->removeGroup(String("player")+playerID_toString(playerId));
    }
  }
  playerId = id;
  if (playerId != 0) {
    server->getPlayerPtr(playerId)->attachClient(this);
    addGroup(String("player")+playerID_toString(playerId));
    server->checkForGameStart();
  }
}

void RemoteClient::flushIncoming()
{
  uint8 buf[BUFFER_LEN];
  size_t messageLength;
  while (0 != (messageLength = inSocket->receive(buf, BUFFER_LEN))) {
    Message message(buf, messageLength);
    if (message.isRealMessage()) {
      incomingMessageQueue.push(message);
    } else {
      Debug("Unrecognized MessageType");
    }
  }
}

void RemoteClient::flushOutgoing(Time time)
{
  send(UpdateMessageData(time, outgoingUpdateQueue));
}

String RemoteClient::performBoolMagic(
    const std::list<String>& name,
    bool newValue
  )
{
  assert(!name.empty());
  if (name.front() == "observer") {
    if (newValue == observer) {
      Debug("unexpectedly asked to confirm a non-change");
      return "";
    }
    if (newValue) {
      if (!server->getAllowObservers()) {
        return "server does not allow observers";
      }
      if (playerId != 0) {
        return "cannot be an observer when assigned to a player";
      }
    }
    observer = newValue;
    if (observer) {
      /* A client becoming an observer can be enough to trigger a game start */
      server->checkForGameStart();
    }
    return "";
  } else if (name.front() == "application") {
    /* Do nothing */
    return "";
  } else if (name.front() == "neveradmin") {
    if (newValue == neverAdmin) {
      Debug("unexpectedly asked to confirm non-change");
      return "";
    }

    neverAdmin = newValue;
    if (!newValue) {
      server->ensureAdminExists();
    }
    return "";
  } else if (name.front() == "admin") {
    if (newValue == admin) {
      Debug("unexpectedly asked to confirm non-change");
      return "";
    }
    
    if (newValue) {
      if (neverAdmin) {
        return "cannot make admin when neveradmin is set";
      }
      admin = newValue;
      addGroup("admin");
    } else {
      admin = newValue;
      server->ensureAdminExists();
    }
    return "";
  } else if (name.front() == "ready") {
    if (newValue == ready) {
      Debug("Unexpectedly asked to confirm non-change");
      return "";
    }

    ready = newValue;

    if (newValue) {
      server->checkForGameStart();
    }
    return "";
  } else {
    Fatal("unexpected child of client branch: " << name.front());
  }
}

String RemoteClient::performStringMagic(
    const std::list<String>& name,
    const String& /*newValue*/
  )
{
  assert(!name.empty());
  if (name.front() == "observer") {
    Fatal("observer not a string setting");
  } else if (name.front() == "application") {
    /* Do nothing */
  } else {
    Fatal("unexpected child of client branch: " << name.front());
  }
  return "";
}

String RemoteClient::performStringSetMagic(
    /*const settingsTree::Leaf* altering,*/
    const std::list<String>& name,
    const __gnu_cxx::hash_set<String, sakusen::StringHash>& /*newValue*/
  )
{
  assert(!name.empty());
  if (name.front() == "observer") {
    Fatal("observer not a string set setting");
  } else if (name.front() == "application") {
    /* Do nothing */
  } else {
    Fatal("unexpected child of client branch: " << name.front());
  }
  return "";
}

