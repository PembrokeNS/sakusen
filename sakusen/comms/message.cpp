#include <sakusen/comms/message.h>

#include <sakusen/iarchive.h>
#include <sakusen/world.h>

namespace sakusen {
namespace comms{

Message::Message(const MessageData* d) :
  player(PlayerId::invalid()),
  data(d)
{
}

Message::Message(
    /*const uint8* buffer,
    size_t bufferLength,*/
    IArchive& in,
    const PlayerId p, /*= static_cast<PlayerId>(-1) (default in header)*/
    const ResourceInterface::Ptr& resourceInterface
  ) SAKUSEN_THROW_SPEC (ResourceDeserializationExn) :
  player(p),
  data()
{
  /* We try to initialize this message by reading in from the given buffer.
   * If anything goes wrong, then we throw an exception */

  /* Extract the protocol version and message type */
  uint8 version;
  MessageType type;
  DeserializationContext context(
      player, ( world ? world->getUniverse() : Universe::ConstPtr() ),
      resourceInterface
    );

  (in >> version).extractEnum(type);

  if (version != SAKUSEN_COMMS_NETWORK_PROTOCOL_VERSION) {
    throw WrongVersionDeserializationExn(
        SAKUSEN_COMMS_NETWORK_PROTOCOL_VERSION, version
      );
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
      data.reset(new GameStartMessageData(in, context));
      break;
    case messageType_order:
      if (!player.valid()) {
        /* This is what happens when we get an OrderMessage before the game
         * starts */
        throw NoWorldDeserializationExn();
      }
      data.reset(new OrderMessageData(in, context));
      break;
    case messageType_update:
      if (world == NULL || !player.valid()) {
        throw NoWorldDeserializationExn();
      }
      data.reset(new UpdateMessageData(in, context));
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
    archive << data->getBuffer();
  } else {
    OArchive subArchive;
    subArchive << uint8(SAKUSEN_COMMS_NETWORK_PROTOCOL_VERSION);
    subArchive.insertEnum(messageType_none);
    archive << subArchive.getBuffer();
  }
}

Message Message::load(IArchive& archive)
{
  PlayerId player;
  Buffer subBuffer;
  /* It's OK to extract the player id from the archive here, because this
   * method is used, e.g., for loading messages out of a replay.  It should
   * *not* be used for loading messages off the wire; that's what the
   * constructor is for. */
  archive >> player >> subBuffer;
  IArchive subArchive(subBuffer);
  return Message(subArchive, player);
}

}}//close namespaces

