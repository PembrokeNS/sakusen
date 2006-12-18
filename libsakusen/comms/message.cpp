#include "message.h"

#include "iarchive.h"
#include "libsakusen-comms-global.h"
#include "world.h"

namespace sakusen {
namespace comms{

Message::Message(const MessageData* d) :
  data(d)
{
}

Message::Message(
    const uint8* buffer,
    size_t bufferLength,
    PlayerID player /*= static_cast<PlayerID>(-1) (default in header)*/
  ) :
  data()
{
  /* We try to initialize this message by reading in from the given buffer.
   * If anything goes wrong, then we throw an exception */
  if (bufferLength < 2) {
    throw EndOfArchiveDeserializationExn();
  }
  
  /* Extract the first byte, which gives the protocol version */
  uint8 version = buffer[0];

  if (version != NETWORK_PROTOCOL_VERSION) {
    throw WrongVersionDeserializationExn(NETWORK_PROTOCOL_VERSION, version);
  }
  
  /* Extract the second byte, which gives the message type */
  MessageType type = MessageType(buffer[1]);
  
  /** \todo More stuff in the message header such as endianness, a magic value
   * to show that it *really* is a sakusen message */

  /* Initialize an archive using the remainder of the buffer */
  IArchive in(buffer+2, bufferLength-2);
  
  switch (type) {
    case messageType_solicit:
      data.reset(new SolicitMessageData(in));
      break;
    case messageType_advertise:
      data.reset(new AdvertiseMessageData(in));
      break;
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
        /* NoWorld is perhaps not entirely descriptive, but it should be
         * accurate */
        throw NoWorldDeserializationExn();
      }
      data.reset(new OrderMessageData(in, &player));
      break;
    case messageType_update:
      if (world == NULL || player == static_cast<PlayerID>(-1)) {
        throw NoWorldDeserializationExn();
      }
      data.reset(new UpdateMessageData(in, &player));
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

}}//close namespaces

