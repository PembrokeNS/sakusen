#ifndef MESSAGE_H
#define MESSAGE_H

#include "libsakusen-global.h"
#include "messagedata.h"

namespace sakusen {
namespace comms {

class LIBSAKUSEN_COMMS_API Message {
  public:
    Message();
    Message(const Message& copy);
    Message(const MessageData& data);
    Message(
        const uint8* buffer,
        size_t bufferLength,
        PlayerID player = static_cast<PlayerID>(-1)
      );
    ~Message();
    Message& operator=(const Message& copy);
  private:
    const MessageData* data;
  public:
    inline MessageType getType(void) const {
      return ( data == NULL ? messageType_none : data->getType() );
    }
    inline bool isRealMessage(void) const {
      return data != NULL;
    }
    inline const uint8* getBytes(void) const {
      return data->getArchive().getBytes();
    }
    inline size_t getBytesLength(void) const {
      return data->getArchive().getLength();
    }
#define GETDATA(type) \
    inline type##MessageData get##type##Data(void) const { \
      return *dynamic_cast<const type##MessageData*>(data); \
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
#undef GETDATA
};

}}

#endif // MESSAGE_H

