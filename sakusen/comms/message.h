#ifndef LIBSAKUSEN_COMMS__MESSAGE_H
#define LIBSAKUSEN_COMMS__MESSAGE_H

#include <sakusen/comms/global.h>
#include <sakusen/comms/messagedata.h>

namespace sakusen {
namespace comms {

/** \brief Represents a message sent between sakusen programs.
 *
 * Any communication between client and server (be it over a network or by
 * whatever other means is appropriate) takes the form of a Message.
 *
 * Messages
 * can be serialized to an OArchive to facilitate transmission over channels
 * which carry binary data, and then reconstructed from and IArchive at the
 * destination.
 *
 * In some cases the reconstruction needs to be told which player
 * the Message came from (this information cannot be stored within the Message
 * itself because that would allow one player to masquerade as another).
 */
class LIBSAKUSEN_COMMS_API Message {
  public:
    explicit Message(const MessageData* data);
    explicit Message(
        IArchive& archive,
        const PlayerId player = PlayerId::invalid(),
        const ResourceInterface::Ptr& = ResourceInterface::Ptr()
      ) SAKUSEN_THROW_SPEC (ResourceDeserializationExn);
  private:
    /** \brief Source of this message.
     *
     * Player will be meaningful only if it was passed to the constructor,
     * otherwise it will be -1.  Such assignment only occurs for messages from
     * clients to servers at game time. */
    PlayerId player;
    MessageData::ConstPtr data;
  public:
    inline MessageType getType(void) const {
      return ( data ? data->getType() : messageType_none );
    }
    inline bool isRealMessage(void) const {
      return data;
    }
    inline const Buffer& getBuffer(void) const {
      return data->getBuffer();
    }
#define SAKUSEN_COMMS_MESSAGE_GETDATA(type) \
    inline type##MessageData get##type##Data(void) const { \
      return dynamic_cast<const type##MessageData&>(*data); \
    }
    SAKUSEN_COMMS_MESSAGE_GETDATA(Join)
    SAKUSEN_COMMS_MESSAGE_GETDATA(Accept)
    SAKUSEN_COMMS_MESSAGE_GETDATA(Reject)
    SAKUSEN_COMMS_MESSAGE_GETDATA(Kick)
    SAKUSEN_COMMS_MESSAGE_GETDATA(Leave)
    SAKUSEN_COMMS_MESSAGE_GETDATA(GetSetting)
    SAKUSEN_COMMS_MESSAGE_GETDATA(ChangeSetting)
    SAKUSEN_COMMS_MESSAGE_GETDATA(NotifySetting)
    SAKUSEN_COMMS_MESSAGE_GETDATA(GameStart)
    SAKUSEN_COMMS_MESSAGE_GETDATA(Order)
    SAKUSEN_COMMS_MESSAGE_GETDATA(Update)
    SAKUSEN_COMMS_MESSAGE_GETDATA(Extension)
#undef SAKUSEN_COMMS_MESSAGE_GETDATA

    void store(OArchive&) const;
    static Message load(IArchive&);
};

}}

#endif // LIBSAKUSEN_COMMS__MESSAGE_H

