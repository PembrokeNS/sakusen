#include "serverinterface.h"

#include "stringutils.h"
#include "libsakusen-comms-global.h"
#include "timeutils.h"
#include "udplisteningsocket.h"
#include "unixdatagramlisteningsocket.h"
#include "socketexn.h"
#include "tedomari-global.h"
#include "revision.h"

#include <ostream>
#include <sstream>

using namespace std;

using namespace sakusen;
using namespace sakusen::comms;
using namespace tedomari;
using namespace tedomari::game;

ServerInterface::ServerInterface(
    Socket* s,
    const String& ja,
#ifndef DISABLE_UNIX_SOCKETS
    bool us,
    bool a,
#endif
    Game* g
  ) :
  solicitationSocket(s),
  joinAddress(ja),
  game(g),
#ifndef DISABLE_UNIX_SOCKETS
  unixSockets(us),
  abstract(a),
#endif
  joined(false),
  incomingSocket(NULL),
  outgoingSocket(NULL)
{
  /*Debug("unixSockets = " << unixSockets);*/
  /** \todo Make timeout user-specifiable (currently 5 seconds) */
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;
}

ServerInterface::~ServerInterface()
{
  if (joined) {
    leave(true);
  }
  joined = false; /* Excessive paranoia */
  delete incomingSocket;
  if (outgoingSocket != incomingSocket) {
    delete outgoingSocket;
  }
  incomingSocket = NULL;
  outgoingSocket = NULL;
}

void ServerInterface::initialSettingsSetup()
{
  /** \todo deal with return values */
  setClientSetting("application:name", APPLICATION_NAME);
  setClientSetting("application:version", APPLICATION_VERSION);
#ifdef REVISION
  setClientSetting("application:revision", numToString(REVISION));
#else
  setClientSetting("application:revision", "unknown");
#endif
}

void ServerInterface::settingAlteration(
    const String& setting,
    const String& value
  )
{
  if (setting == ":game:universe:name") {
    universeName = value;
  }
  else if (setting == ":game:universe:hash") {
    /* When the universe is set we need to let game know */
    game->setUniverse(universeName, value);
  }
}

bool ServerInterface::getAdvertisement(AdvertiseMessageData* advertisement)
{
  Socket* tempSocket;

#ifndef DISABLE_UNIX_SOCKETS
  if (unixSockets) {
    tempSocket = new UnixDatagramListeningSocket(abstract);
    String address = tempSocket->getAddress();
    solicitationSocket->send(SolicitMessageData(address));
  } else {
#endif
    tempSocket = solicitationSocket;
    solicitationSocket->send(SolicitMessageData(""));
#ifndef DISABLE_UNIX_SOCKETS
  }
#endif
  
  tempSocket->setNonBlocking(true);
  
  uint8 buffer[BUFFER_LEN];
  size_t messageLength;

  if (0 == (messageLength =
        tempSocket->receiveTimeout(buffer, BUFFER_LEN, timeout))) {
    return true;
  }

  Message message(buffer, messageLength);
  if (message.getType() != messageType_advertise) {
    Debug("Non-advertisement received (type was " << message.getType() << ")");
    return true;
  }
  *advertisement=message.getAdvertiseData();

#ifndef DISABLE_UNIX_SOCKETS
  if (unixSockets) {
    delete tempSocket;
  }
#endif
  
  return false;
}

String ServerInterface::flushIncoming()
{
  if (!joined) {
    return "";
  }

  assert(incomingSocket != NULL);

  size_t messageLength;
  uint8 buf[BUFFER_LEN];
  ostringstream out;
  
  while (joined &&
      0 != (messageLength = incomingSocket->receive(buf, BUFFER_LEN))) {
    try {
      Message message(buf, messageLength);
      switch (message.getType()) {
        case messageType_kick:
          {
            KickMessageData data = message.getKickData();
            out << "Kicked by server (" << data.getReason() << ")\n";
            leave(false);
          }
          break;
        case messageType_reject:
          {
            RejectMessageData data = message.getRejectData();
            out << "Request rejected by server (" << data.getReason() << ")\n";
          }
          break;
        case messageType_notifySetting:
          {
            NotifySettingMessageData data = message.getNotifySettingData();
            out << "Server reported that value of '" << data.getSetting() <<
              "' was:\n" << data.getValue() << "\n";
            settingAlteration(data.getSetting(), data.getValue());
          }
          break;
        case messageType_gameStart:
          {
            GameStartMessageData data = message.getGameStartData();
            out << "Game is starting!\n";
            game->start(data, this);
          }
          break;
        case messageType_update:
          {
            game->pushUpdates(message.getUpdateData());
          }
          break;
        default:
          out << "Unexpected MessageType " << message.getType() << "\n";
          break;
      }
    } catch (DeserializationExn* e) {
      out << "Deserialization exception: " << e->message << "\n";
      delete e;
    }
  }

  return out.str();
}

String ServerInterface::join()
{
  if (joined) {
    Fatal("attempted to join when already joined");
  }
  assert(incomingSocket == NULL);
  assert(outgoingSocket == NULL);
  try {
#ifndef DISABLE_UNIX_SOCKETS
    if (unixSockets) {
      incomingSocket = new UnixDatagramListeningSocket(abstract);
      /** \bug We're assuming equal solicitation and join addresses */
      solicitationSocket->send(JoinMessageData(incomingSocket->getAddress()));
    } else {
#endif
      /*Debug(
          "creating new connection to " << joinAddress << " for join message"
        );*/
      incomingSocket = Socket::newConnectionToAddress(joinAddress);
      if (incomingSocket == NULL) {
        return "Could not connect to address '" + joinAddress +
          "'.  Perhaps you need to specify a join address,  or you have "
          "specified an invalid one?";
      }
      incomingSocket->send(JoinMessageData(""));
#ifndef DISABLE_UNIX_SOCKETS
    }
#endif
    incomingSocket->setNonBlocking(true);
  } catch (SocketExn* e) {
    delete incomingSocket;
    incomingSocket = NULL;
    String ret = String("Error while sending join message to server: '") +
      e->message + "'.\n";
    delete e;
    return ret;
  }
  
  uint8 buffer[BUFFER_LEN];
  size_t messageLength;

  if (0 == (messageLength =
        incomingSocket->receiveTimeout(buffer, BUFFER_LEN, timeout))) {
    delete incomingSocket;
    incomingSocket = NULL;
    return "Timed out while waiting for response to join request.\n";
  }

  Message message(buffer, messageLength);
  
  switch (message.getType()) {
    case messageType_accept:
      {
        AcceptMessageData data = message.getAcceptData();
        id = data.getID();
        if (data.getAddress() == "") {
          outgoingSocket = incomingSocket;
        } else {
          outgoingSocket = Socket::newConnectionToAddress(data.getAddress());
        }
        joined = true;
        initialSettingsSetup();
        return "";
      }
    case messageType_reject:
      {
        delete incomingSocket;
        incomingSocket = NULL;
        RejectMessageData data = message.getRejectData();
        return String("Join request rejected for reason: '") + data.getReason()
          + "'.";
      }
    default:
      delete incomingSocket;
      incomingSocket = NULL;
      ostringstream ret;
      ret << "Unexpected response from server (message type was" <<
        message.getType() << ").";
      return ret.str();
  }
}

bool ServerInterface::leave(bool sendMessage)
{
  if (!joined) {
    Fatal("attempted to leave when not joined");
  }
  assert(incomingSocket != NULL);
  assert(outgoingSocket != NULL);
  if (sendMessage) {
    try {
      outgoingSocket->send(LeaveMessageData());
    } catch (SocketExn* e) {
      Debug("Error sending leave message:" << e->message);
      delete e;
    }
  }
  game->stop();
  joined = false;
  delete incomingSocket;
  if (outgoingSocket != incomingSocket) {
    delete outgoingSocket;
  }
  incomingSocket = NULL;
  outgoingSocket = NULL;
  return false;
}

bool ServerInterface::send(const MessageData& message)
{
  outgoingSocket->send(message);
  return false;
}

bool ServerInterface::getSetting(const String& setting)
{
  outgoingSocket->send(GetSettingMessageData(setting));
  return false;
}

bool ServerInterface::setSetting(const String& setting, const String& value)
{
  outgoingSocket->send(ChangeSettingMessageData(setting, value));
  return false;
}

bool ServerInterface::setClientSetting(
    const String& localSetting,
    const String& value
  )
{
  ostringstream settingStream;
  settingStream << "clients:" << clientID_toString(id);
  
  if (localSetting.length() != 0 && localSetting[0] != ':') {
    settingStream << SETTINGS_DELIMITER;
  }

  settingStream << localSetting;

  return setSetting(settingStream.str(), value);
}

