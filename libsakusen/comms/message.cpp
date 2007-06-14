#include "message.h"

#include "iarchive.h"
#include "libsakusen-comms-global.h"
#include "world.h"

namespace sakusen {
namespace comms{

Message::Message(const MessageData* d) :
  player(-1),
  data(d)
{
}

Message::Message(
    /*const uint8* buffer,
    size_t bufferLength,*/
    IArchive& in,
    PlayerID p /*= static_cast<PlayerID>(-1) (default in header)*/
  ) :
  player(p),
  data()
{
  /* We try to initialize this message by reading in from the given buffer.
   * If anything goes wrong, then we throw an exception */
  
  /* Extract the protocol version and message type */
  uint8 version;
  MessageType type;

  (in >> version).extractEnum(type);

  if (version != NETWORK_PROTOCOL_VERSION) {
    throw WrongVersionDeserializationExn(NETWORK_PROTOCOL_VERSION, version);
  }
  
  /** \todo More stuff in the message header such as endianness, a magic value
   * to show that it *really* is a sakusen message */

  switch (type) {
    case messageType_join:
      data.reset(new JoinMessageData(in));
      break;
    case messageType_accept:
      data.reset(new AcceptMessageData(in));
      break;
    case messageType_reject:
      data.reset(new RejectMessageData(in));
      break;
    case messageType_kick:
      data.reset(new KickMessageData(in));
      break;
    case messageType_leave:
      data.reset(new LeaveMessageData(in));
      break;
    case messageType_getSetting:
      data.reset(new GetSettingMessageData(in));
      break;
    case messageType_changeSetting:
      data.reset(new ChangeSettingMessageData(in));
      break;
    case messageType_notifySetting:
      data.reset(new NotifySettingMessageData(in));
      break;
    case messageType_gameStart:
      data.reset(new GameStartMessageData(in));
      break;
    case messageType_order:
      if (player == static_cast<PlayerID>(-1)) {
        /* This is what happens when we get an OrderMessage before the game
         * starts */
        throw NoWorldDeserializationExn();
      }
      data.reset(new OrderMessageData(
            in, DeserializationContext(player, world->getUniverse())
          ));
      break;
    case messageType_update:
      if (world == NULL || player == static_cast<PlayerID>(-1)) {
        throw NoWorldDeserializationExn();
      }
      data.reset(new UpdateMessageData(
            in, DeserializationContext(player, world->getUniverse())
          ));
      break;
    case messageType_extension:
      data.reset(new ExtensionMessageData(in));
      break;
    default:
      throw EnumDeserializationExn("type", type);
      return;
  }
  
  /* Check that we exactly reached the end of the archive */
  if (!in.isFinished()) {
    throw TooMuchArchiveDeserializationExn();
    data.reset();
  }
}

void Message::store(OArchive& archive) const
{
  archive << player;
  if (data) {
    archive << data->getArchive();
  } else {
    OArchive subArchive;
    subArchive << uint8(NETWORK_PROTOCOL_VERSION);
    subArchive.insertEnum(messageType_none);
    archive << subArchive;
  }
}

Message Message::load(IArchive& archive)
{
  PlayerID player;
  IArchive subArchive;
  archive >> player >> subArchive;
  return Message(subArchive, player);
}

}}//close namespaces

