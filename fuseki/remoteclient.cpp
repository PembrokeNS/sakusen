#include "remoteclient.h"

#include "messagedata.h"
#include "unixdatagramlisteningsocket.h"
#include "server.h"

using namespace __gnu_cxx;

using namespace sakusen;
using namespace sakusen::comms;
using namespace fuseki;

RemoteClient::RemoteClient(
    ClientId i,
    Server* s,
    const sakusen::comms::Socket::Ptr& socket,
    bool createInSocket
#ifndef DISABLE_UNIX_SOCKETS
    ,
    bool abstract
#endif
  ) :
  Client(i),
  SettingsUser(String("client")+clientId_toString(i)),
  server(s),
  inSocket(),
  outSocket(socket),
  incomingMessageQueue(),
  outgoingUpdateQueue(),
  admin(false),
  neverAdmin(true),
  ready(false),
  autoUnready(false),
  dead(false)
{
  outSocket->setNonBlocking(true);
  if (createInSocket) {
#ifdef DISABLE_UNIX_SOCKETS
    Fatal("Separate inSocket not supported on this platform");
#else
    inSocket = Socket::Ptr(new UnixDatagramListeningSocket(abstract));
    inSocket->setNonBlocking(true);
    /** \bug The following line should not be here, because it can make the
     * server hang.  However, without it, we can suffer from the bug caused by
     * our poor handling of EAGAIN in *Socket::send.  Once that is somehow
     * fixed, this can also be fixed */
    outSocket->setNonBlocking(false);
    outSocket->send(
        Message(new AcceptMessageData(inSocket->getAddress(), clientId))
      );
#endif
  } else {
    inSocket = outSocket;
    outSocket->send(Message(new AcceptMessageData("", clientId)));
  }
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
}

void RemoteClient::setPlayerId(PlayerId id, bool removeGroup)
{
  if (playerId != 0) {
    try {
      server->getPlayerPtr(playerId)->detachClient(this);
      if (removeGroup) {
        /* This condition is necessary because when this method is called from
         * the destructor, this->groups may already have been cleared when the
         * game started */
        this->removeGroup(String("player")+playerId_toString(playerId));
      }
    } catch (InvalidPlayerId& e) {
      /* The last player id was invalid. Continue anyway, o/w we can never
       * recover.
       */
      Debug("RemoteClient::setPlayerId found the old id was invalid");
    }
  }
  if (id != 0) {
    try {
      server->getPlayerPtr(id)->attachClient(this);
      addGroup(String("player")+playerId_toString(id));
    } catch (InvalidPlayerId& e) {
      Debug("RemoteClient::setPlayerId was given an invalid id");
      throw;
    }
  }
  playerId = id; /* do this last in case we exn'd out */
  server->checkForGameStart();
}

void RemoteClient::flushIncoming()
{
  uint8 buf[BUFFER_LEN];
  size_t messageLength;
  while (0 != (messageLength = inSocket->receive(buf, BUFFER_LEN))) {
    IArchive archive(buf, messageLength);
    Message message(archive, getPlayerId());
    if (message.isRealMessage()) {
      incomingMessageQueue.push(message);
    } else {
      Debug("Unrecognized MessageType");
    }
  }
}

void RemoteClient::flushOutgoing(Time time)
{
  send(new UpdateMessageData(time, outgoingUpdateQueue));
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
    const std::set<String>& /*newValue*/
  )
{
  assert(!name.empty());
  if (name.front() == "observer") {
    Fatal("observer not a string set setting");
  } else if (name.front() == "application") {
    Fatal("application not a string set setting");
  } else {
    Fatal("unexpected child of client branch: " << name.front());
  }
  return "";
}

