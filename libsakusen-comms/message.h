#ifndef MESSAGE_H
#define MESSAGE_H

#include "libsakusen-global.h"
#include "messagedata.h"

class Message {
  public:
    Message();
    Message(const Message& copy);
    Message(const MessageData& data);
    Message(const uint8* buffer, size_t bufferLength);
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
    inline AdvertiseMessageData getAdvertiseData(void) const {
      return *dynamic_cast<const AdvertiseMessageData*>(data);
    }
    inline SolicitMessageData getSolicitData(void) const {
      return *dynamic_cast<const SolicitMessageData*>(data);
    }
    inline JoinMessageData getJoinData(void) const {
      return *dynamic_cast<const JoinMessageData*>(data);
    }
    inline AcceptMessageData getAcceptData(void) const {
      return *dynamic_cast<const AcceptMessageData*>(data);
    }
    inline RejectMessageData getRejectData(void) const {
      return *dynamic_cast<const RejectMessageData*>(data);
    }
    inline KickMessageData getKickData(void) const {
      return *dynamic_cast<const KickMessageData*>(data);
    }
    inline LeaveMessageData getLeaveData(void) const {
      return *dynamic_cast<const LeaveMessageData*>(data);
    }
    inline GetSettingMessageData getGetSettingData(void) const {
      return *dynamic_cast<const GetSettingMessageData*>(data);
    }
    inline ChangeSettingMessageData getChangeSettingData(void) const {
      return *dynamic_cast<const ChangeSettingMessageData*>(data);
    }
    inline NotifySettingMessageData getNotifySettingData(void) const {
      return *dynamic_cast<const NotifySettingMessageData*>(data);
    }
    inline UpdateMessageData getUpdateData(void) const {
      return *dynamic_cast<const UpdateMessageData*>(data);
    }
};

#endif // MESSAGE_H

