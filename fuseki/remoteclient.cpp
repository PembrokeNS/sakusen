#include "remoteclient.h"

#include "messagedata.h"
#include "unixdatagramlisteningsocket.h"
#include "server.h"

using namespace __gnu_cxx;

using namespace fuseki;

RemoteClient::RemoteClient(
    ClientID i,
    Server* s,
    Socket* socket,
    bool abstract
  ) :
  SettingsUser(String("client")+clientID_toString(i)),
  id(i),
  server(s),
  inSocket(NULL),
  outSocket(socket),
  messageQueue(),
  admin(false),
  neverAdmin(true)
{
  inSocket = new UnixDatagramListeningSocket(abstract);
  inSocket->setAsynchronous(true);
  outSocket->send(Message(AcceptMessageData(inSocket->getAddress(), id)));
}

RemoteClient::~RemoteClient()
{
  delete inSocket;
  delete outSocket;
  inSocket = NULL;
  outSocket = NULL;
}

void RemoteClient::flushIncoming()
{
  uint8 buf[BUFFER_LEN];
  size_t messageLength;
  while (0 != (messageLength = inSocket->receive(buf, BUFFER_LEN))) {
    Message message(buf, messageLength);
    if (message.isRealMessage()) {
      messageQueue.push(message);
    } else {
      Debug("Unrecognized MessageType");
    }
  }
}

void RemoteClient::sendUpdate(const Update& update)
{
  send(UpdateMessageData(update));
}

String RemoteClient::performBoolMagic(
    /*const settingsTree::Leaf* altering,*/
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
  } else if (name.front() == "application") {
    /* Do nothing */
  } else if (name.front() == "neveradmin") {
    if (newValue == neverAdmin) {
      Debug("unexpectedly asked to confirm non-change");
      return "";
    }

    if (newValue) {
      neverAdmin = newValue;
    } else {
      neverAdmin = newValue;
      server->ensureAdminExists();
    }
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
  } else {
    Fatal("unexpected child of client branch: " << name.front());
  }
  return "";
}

String RemoteClient::performStringMagic(
    /*const settingsTree::Leaf* altering,*/
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
    const hash_set<String, StringHash>& /*newValue*/
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

