#include "stringutils.h"
#include "serverinterface.h"
#include "libsakusen-comms-global.h"
#include "unixdatagramlisteningsocket.h"
#include "socketexception.h"
#include "tedomari-global.h"
#include "revision.h"

#include <sys/time.h>
#include <time.h>
#include <ostream>
#include <sstream>

using namespace std;

using namespace sakusen;
using namespace sakusen::comms;
using namespace tedomari;
using namespace tedomari::game;

ServerInterface::ServerInterface(Socket* s, bool a, Game* g) :
  serverSocket(s),
  game(g),
  abstract(a),
  joined(false),
  localSocket(NULL),
  privateServerSocket(NULL)
{
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
    /* 1 second timeout used for all incoming server responses.
     * TODO: make timeout user-specifiable */
}

ServerInterface::~ServerInterface()
{
  if (joined) {
    leave(true);
  }
  joined = false; /* Excessive paranoia */
  delete localSocket;
  delete privateServerSocket;
  localSocket = NULL;
  privateServerSocket = NULL;
}

void ServerInterface::initialSettingsSetup()
{
  /* TODO: deal with return values */
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
  UnixDatagramListeningSocket tempSocket(abstract);
  String address = tempSocket.getAddress();
  tempSocket.setAsynchronous(true);
  serverSocket->send(SolicitMessageData(address));
  
  uint8 buffer[BUFFER_LEN];
  size_t messageLength;

  if (0==(messageLength=tempSocket.receive(buffer, BUFFER_LEN, timeout))) {
    return true;
  }

  Message message(buffer, messageLength);
  if (message.getType() != messageType_advertise) {
    Debug("Non-advertisement received (type was " << message.getType() << ")");
    return true;
  }
  *advertisement=message.getAdvertiseData();
  return false;
}

String ServerInterface::flushIncoming()
{
  if (!joined) {
    return "";
  }

  assert(localSocket != NULL);

  size_t messageLength;
  uint8 buf[BUFFER_LEN];
  ostringstream out;
  
  while (joined &&
      0 != (messageLength = localSocket->receive(buf, BUFFER_LEN))) {
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
            game->start(data);
          }
          break;
        case messageType_update:
          {
            UpdateMessageData data = message.getUpdateData();
            game->pushUpdate(data.getUpdate());
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
  assert(localSocket == NULL);
  assert(privateServerSocket == NULL);
  localSocket = new UnixDatagramListeningSocket(abstract);
  localSocket->setAsynchronous(true);
  try {
    serverSocket->send(JoinMessageData(localSocket->getAddress()));
  } catch (SocketExn* e) {
    delete localSocket;
    localSocket = NULL;
    String ret = String("Error while sending join message to server: '") +
      e->message + "'.\n";
    delete e;
    return ret;
  }
  
  uint8 buffer[BUFFER_LEN];
  size_t messageLength;

  if (0==(messageLength=localSocket->receive(buffer, BUFFER_LEN, timeout))) {
    delete localSocket;
    localSocket = NULL;
    return "Timed out while waiting for response to join request.\n";
  }

  Message message(buffer, messageLength);
  
  switch (message.getType()) {
    case messageType_accept:
      {
        AcceptMessageData data = message.getAcceptData();
        id = data.getID();
        privateServerSocket = Socket::newConnectionToAddress(data.getAddress());
        joined = true;
        initialSettingsSetup();
        return "";
      }
    case messageType_reject:
      {
        delete localSocket;
        localSocket = NULL;
        RejectMessageData data = message.getRejectData();
        return String("Join request rejected for reason: '") + data.getReason()
          + "'.";
      }
    default:
      delete localSocket;
      localSocket = NULL;
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
  assert(localSocket != NULL);
  assert(privateServerSocket != NULL);
  if (sendMessage) {
    try {
      privateServerSocket->send(LeaveMessageData());
    } catch (SocketExn* e) {
      Debug("Error sending leave message:" << e->message);
      delete e;
    }
  }
  game->stop();
  joined = false;
  delete localSocket;
  delete privateServerSocket;
  localSocket = NULL;
  privateServerSocket = NULL;
  return false;
}

bool ServerInterface::getSetting(const String& setting)
{
  privateServerSocket->send(GetSettingMessageData(setting));
  return false;
}

bool ServerInterface::setSetting(const String& setting, const String& value)
{
  privateServerSocket->send(ChangeSettingMessageData(setting, value));
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

