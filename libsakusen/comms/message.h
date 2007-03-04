#ifndef MESSAGE_H
#define MESSAGE_H

#include "libsakusen-global.h"
#include "messagedata.h"

namespace sakusen {
namespace comms {

class LIBSAKUSEN_COMMS_API Message {
  public:
    explicit Message(const MessageData* data);
    /*Message(
        const uint8* buffer,
        size_t bufferLength,
        PlayerID player = static_cast<PlayerID>(-1)
      );*/
    Message(
        IArchive& archive,
        PlayerID player = static_cast<PlayerID>(-1)
      );
  private:
    /* player will be meaningful only if it was passed to the constructor,
     * otherwise it will be -1 */
    PlayerID player;
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
    GETDATA(Advertise)
    GETDATA(Solicit)
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

