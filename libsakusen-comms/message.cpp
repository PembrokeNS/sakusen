#include "message.h"

#include "iarchive.h"
#include "libsakusen-comms-global.h"

Message::Message(const Message& copy) :
  data( copy.isRealMessage() ? copy.data->newCopy() : NULL )
{
}

Message::Message(const MessageData& data) :
  data(data.newCopy())
{
}

Message::Message(const uint8* buffer, size_t bufferLength) :
  data(NULL)
{
  /* We try to initialize this message by reading in from the given buffer.
   * If anything goes wrong, then we let data remain NULL and leave this as a
   * non-message */
  if (bufferLength < 2) {
    return;
  }
  
  /* Extract the first byte, which gives the protocol version */
  uint8 version = buffer[0];

  if (version != NETWORK_PROTOCOL_VERSION) {
    Debug("unexpected protocol version " << version << " on message");
    return;
  }
  
  /* Extract the second byte, which gives the message type */
  MessageType type = MessageType(buffer[1]);
  
  /* TODO: More stuff in the message header such as endianness, a magic value
   * to show that it *really* is a sakusen message */

  /* Initialize an archive using the remainder of the buffer */
  IArchive in(buffer+2, bufferLength-2);
  
  switch (type) {
    case messageType_solicit:
      data = new SolicitMessageData(in);
      break;
    case messageType_advertise:
      data = new AdvertiseMessageData(in);
      break;
    case messageType_join:
      data = new JoinMessageData(in);
      break;
    case messageType_accept:
      data = new AcceptMessageData(in);
      break;
    case messageType_reject:
      data = new RejectMessageData(in);
      break;
    case messageType_kick:
      data = new KickMessageData(in);
      break;
    case messageType_leave:
      data = new LeaveMessageData(in);
      break;
    case messageType_getSetting:
      data = new GetSettingMessageData(in);
      break;
    case messageType_changeSetting:
      data = new ChangeSettingMessageData(in);
      break;
    case messageType_notifySetting:
      data = new NotifySettingMessageData(in);
      break;
    case messageType_update:
      data = new UpdateMessageData(in);
      break;
    default:
      Debug("Unknown message type " << type);
      return;
  }
  
  /* Check that we exactly reached the end of the archive */
  if (!in.isFinished()) {
    Debug("Archive didn't exactly match expected length");
    delete data;
    data = NULL;
  }
}

Message& Message::operator=(const Message& copy)
{
  data = copy.data->newCopy();
  return *this;
}

Message::~Message()
{
  if (data != NULL) {
    delete data;
    data = NULL;
  }
}

