#ifndef MESSAGE_H
#define MESSAGE_H

#include "libsakusen-global.h"
#include "messagedata.h"

namespace sakusen {
namespace comms {

/** \brief Represents a message sent between sakusen programs.
 *
 * Any communication between client and server (be it over a network or by
 * whatever other means is appropriate) takes the form of a Message.
 *
 * Messages
 * can be serialized to an OArchive to facilitate transmission over channels
 * which carrybinary data, and then reconstructed from and IArchive at the
 * destination.
 *
 * In some cases the reconstruction needs to be told which player
 * the Message came from (this information cannot be stored within the Message
 * itself because that would allow one player to masquerade as another).
 */
class LIBSAKUSEN_COMMS_API Message {
  public:
    explicit Message(const MessageData* data);
    Message(
        IArchive& archive,
        PlayerId player = static_cast<PlayerId>(-1)
      );
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
    inline const uint8* getBytes(void) const {
      return data->getArchive().getBytes();
    }
    inline size_t getBytesLength(void) const {
      return data->getArchive().getLength();
    }
#define GETDATA(type) \
    inline type##MessageData get##type##Data(void) const { \
      return dynamic_cast<const type##MessageData&>(*data); \
    }
    GETDATA(Join)
    GETDATA(Accept)
    GETDATA(Reject)
    GETDATA(Kick)
    GETDATA(Leave)
    GETDATA(GetSetting)
    GETDATA(ChangeSetting)
    GETDATA(NotifySetting)
    GETDATA(GameStart)
    GETDATA(Order)
    GETDATA(Update)
    GETDATA(Extension)
#undef GETDATA

    void store(OArchive&) const;
    static Message load(IArchive&);
};

}}

#endif // MESSAGE_H

