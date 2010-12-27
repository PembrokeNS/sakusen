#include "serverinterface.h"

#include <sakusen/stringutils.h>
#include <sakusen/comms/timeutils.h>
#include <sakusen/comms/udplisteningsocket.h>
#include <sakusen/comms/unixdatagramlisteningsocket.h>
#include <sakusen/comms/socketexn.h>
#include <sakusen/revision.h>
#include <sakusen/heightfield-methods.h>

#include <ostream>
#include <sstream>

using namespace std;
using namespace sakusen;
using namespace comms;
using namespace tedomari;
using namespace tedomari::game;

ServerInterface::ServerInterface(
    const String& ja,
#ifndef DISABLE_UNIX_SOCKETS
    bool us,
    bool a,
#endif
    Game* g
  ) :
  joinAddress(ja),
  game(g),
#ifndef DISABLE_UNIX_SOCKETS
  unixSockets(us),
  abstract(a),
#endif
  joined(false),
  id(),
  incomingSocket(),
  outgoingSocket(),
  universeName()
{
  /*SAKUSEN_DEBUG("unixSockets = " << unixSockets);*/
  /** \todo Make timeout user-specifiable (currently 5 seconds) */
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;
}

/** \brief Destructor
 *
 * The destructor sends a leave message to the server if appropriate, and
 * closes associated sockets.  Further behaviour is not excluded. */
ServerInterface::~ServerInterface()
{
  if (joined) {
    leave(true);
  }
  joined = false; /* Excessive paranoia */
}

/** Do initial setup of settings for us on the server (e.g. tell our
 * client application, etc.) */
void ServerInterface::initialSettingsSetup()
{
  /** \todo deal with return values */
  setClientSetting("application:name", TEDOMARI_APPLICATION_NAME);
  setClientSetting("application:version", TEDOMARI_APPLICATION_VERSION);
  setClientSetting("application:revision", REVISION);

  /* we request the universe name and hash so that our game can be set up
   * correctly if they were set before we joined */
  getSetting("game:universe:name");
  getSetting("game:universe:hash");
}

void ServerInterface::settingAlteration(
    const String& setting,
    const set<String>& value
  )
{
  if (setting == ":game:universe:name") {
    /* Store the universe name for use when the hash arrives */
    /*SAKUSEN_QDEBUG("storing universe name");*/
    assert(value.size() == 1);
    universeName = *value.begin();
  }
  else if (setting == ":game:universe:hash") {
    /* If the universe name is not set, then we must have caught the tail end
     * of a report that was in progress when we joined.  The hash *should*
     * arrive again after the name, so for now we wait */
    if (universeName.empty()) {
      SAKUSEN_DEBUG("got universe hash without name");
      return;
    }
    /* When the universe is set we need to let game know */
    assert(value.size() == 1);
    game->setUniverse(universeName, *value.begin());
  }
}

/** \brief Deal with all pending messages from server.
 *
 * \return Output that should be given to the user. */
String ServerInterface::flushIncoming(
    const sakusen::ResourceInterface::Ptr& resourceInterface
  )
{
  if (!joined) {
    return "";
  }

  assert(incomingSocket != NULL);

  size_t messageLength;
  uint8 buf[SAKUSEN_COMMS_BUFFER_LEN];
  ostringstream out;
  
  while (joined && 0 != (
        messageLength = incomingSocket->receive(buf, SAKUSEN_COMMS_BUFFER_LEN)
      )) {
    try {
      PlayerId playerId =
        ( NULL == client::world ?
          PlayerId::invalid() : client::world->getPlayerId() );
      IArchive messageArchive(buf, messageLength);
      Message message(messageArchive, playerId, resourceInterface);
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
            if (data.isLeaf()) {
              out << "Server reported that value of '" << data.getSetting() <<
                "' was:\n" << stringUtils_join(data.getValue(), "\n") << "\n";
            } else {
              out << "Server reported that '" << data.getSetting() <<
                "' contained the following nodes:\n" <<
                stringUtils_join(data.getValue(), "\n") << "\n";
            }
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
        case messageType_extension:
          {
            /** \todo Provision for other extensions, possibly through plugins
             * */
            ExtensionMessageData data = message.getExtensionData();
            if (data.getExtension() == "talk" && data.getVersion() == 0) {
              IArchive subData(data.getData());
              String talkMessage;
              try {
                subData >> talkMessage;
                out << "-- " << talkMessage << "\n";
              } catch (DeserializationExn& e) {
                out << "Warning: Malformed talk message (" << e.message <<
                  ")\n";
              }
            }
          }
          break;
        default:
          out << "Unexpected MessageType " << message.getType() << "\n";
          break;
      }
    } catch (DeserializationExn& e) {
      out << "Deserialization exception: " << e.message << "\n";
    }
  }

  return out.str();
}

/** \brief Try to join server.
 *
 * \return Empty string if there is no problem, otherwise an error message
 * */
String ServerInterface::join()
{
  if (joined) {
    SAKUSEN_FATAL("attempted to join when already joined");
  }
  assert(!incomingSocket);
  assert(!outgoingSocket);
  try {
#ifndef DISABLE_UNIX_SOCKETS
    if (unixSockets) {
      /* For one-way Unix sockets we need to setup two sockets: one connected
       * to the server's socket in the given place, and one we listen on.
       */
      incomingSocket.reset(new UnixDatagramListeningSocket(abstract));
      outgoingSocket = Socket::newConnectionToAddress(joinAddress);
      outgoingSocket->send(
          Message(new JoinMessageData(incomingSocket->getAddress()))
        );
    } else {
#endif
      /* For two-way sockets, the server will talk to us on the socket we join
       * from, and we need to be prepared to open a second socket only if the
       * server tells us to.
       */
      /*SAKUSEN_DEBUG(
          "creating new connection to " << joinAddress << " for join message"
        );*/
      incomingSocket = Socket::newConnectionToAddress(joinAddress);
      if (incomingSocket == NULL) {
        return "Could not connect to address '" + joinAddress +
          "'.  Perhaps you need to specify a join address,  or you have "
          "specified an invalid one?";
      }
      incomingSocket->send(Message(new JoinMessageData("")));
      outgoingSocket = incomingSocket;
#ifndef DISABLE_UNIX_SOCKETS
    }
#endif
    incomingSocket->setNonBlocking(true);
  } catch (SocketExn& e) {
    String ret = String("Error while sending join message to server: '") +
      e.message + "'.\n";
    incomingSocket.reset();
    outgoingSocket.reset();
    return ret;
  }
  
  uint8 buffer[SAKUSEN_COMMS_BUFFER_LEN];
  size_t messageLength;

  if (0 == (messageLength = incomingSocket->receiveTimeout(
          buffer, SAKUSEN_COMMS_BUFFER_LEN, timeout
        ))) {
    incomingSocket.reset();
    outgoingSocket.reset();
    return "Timed out while waiting for response to join request.\n";
  }

  IArchive messageArchive(buffer, messageLength);
  Message message(messageArchive);
  
  switch (message.getType()) {
    case messageType_accept:
      {
        AcceptMessageData data = message.getAcceptData();
        id = data.getId();
        if (data.getAddress() != "") {
          outgoingSocket = Socket::newConnectionToAddress(data.getAddress());
        }
        joined = true;
        initialSettingsSetup();
        return "";
      }
    case messageType_reject:
      {
        RejectMessageData data = message.getRejectData();
        incomingSocket.reset();
        outgoingSocket.reset();
        return String("Join request rejected for reason: '") + data.getReason()
          + "'.";
      }
    default:
      {
        ostringstream ret;
        ret << "Unexpected response from server (message type was" <<
          message.getType() << ").";
        incomingSocket.reset();
        outgoingSocket.reset();
        return ret.str();
      }
  }
}

/** \brief Try to leave server.
 *
 * \param sendMessage Whether to send a message to the server indicating
 * that we are leaving.
 * \return true iff an error occurs */
bool ServerInterface::leave(bool sendMessage)
{
  if (!joined) {
    SAKUSEN_FATAL("attempted to leave when not joined");
  }
  assert(incomingSocket != NULL);
  assert(outgoingSocket != NULL);
  if (sendMessage) {
    try {
      outgoingSocket->send(Message(new LeaveMessageData()));
    } catch (SocketExn& e) {
      SAKUSEN_DEBUG("Error sending leave message: " << e.message);
    }
  }
  game->stop();
  joined = false;
  return false;
}

/** \brief Send the given message to the server at once
 *
 * \param message Message to send (Ownership tranferred to this)
 * \return true iff an error occurs */
bool ServerInterface::send(const MessageData* message)
{
  outgoingSocket->send(Message(message));
  return false;
}

/** \brief Request a setting from the server (asynchronously).
 *
 * \param setting Address of the setting to request
 * \return true iff an error occurs */
bool ServerInterface::getSetting(const String& setting)
{
  outgoingSocket->send(Message(new GetSettingMessageData(setting)));
  return false;
}

/** \brief Requests a setting change on the server (asynchronously).
 *
 * \param setting Address of the setting to change
 * \param value Value to assign to the setting
 * \return true iff an error occurs */
bool ServerInterface::setSetting(const String& setting, const String& value)
{
  outgoingSocket->send(Message(new ChangeSettingMessageData(setting, value)));
  return false;
}

/** \brief Requests a setting value from client settings tree.
 * 
 * \param setting Address of the setting to get relative to the client
 * branch
 * \return true iff an error occurs */
bool ServerInterface::getClientSetting(
    const String& setting
  )
{
  String delim;
  if (setting.length() != 0 && setting[0] != ':') {
    delim = SAKUSEN_COMMS_SETTINGS_DELIMITER;
  }
  
  return getSetting(
      "clients:" + id.toString() + delim + setting
    );
}

/** \brief Requests a setting change  our client settings tree.
 * 
 * \param setting Address of the setting to change relative to the client
 * branch
 * \param value Value to assign to the setting
 * \return true iff an error occurs */
bool ServerInterface::setClientSetting(
    const String& setting,
    const String& value
  )
{
  String delim;
  if (setting.length() != 0 && setting[0] != ':') {
    delim = SAKUSEN_COMMS_SETTINGS_DELIMITER;
  }
  
  return setSetting(
      "clients:" + id.toString() + delim + setting, value
    );
}
