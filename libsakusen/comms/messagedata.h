#ifndef MESSAGEDATA_H
#define MESSAGEDATA_H

#include "messagetype.h"
#include "oarchive.h"
#include "iarchive.h"
#include "update.h"
#include "libsakusen-comms-global.h"
#include "clientid.h"

namespace sakusen {
namespace comms {

class MessageData {
  /* Abstract class - all constructors must be protected */
  protected:
    MessageData();
    MessageData(const MessageData& copy);
    OArchive archive;
    
    virtual void fillArchive() = 0;
  public:
    virtual ~MessageData();
    const OArchive& getArchive() const {
      if (archive.getLength() == 0) {
        /* Much as it pains me to use it, the const_cast here is warranted,
         * because what we're really doing here is lazy evaluation, rather than
         * *real* alteration.  At least, that's what I tell myself.  It makes me
         * feel better. - JJB */
        const_cast<OArchive&>(archive) << uint8(NETWORK_PROTOCOL_VERSION) <<
          uint8(getType());
        const_cast<MessageData*>(this)->fillArchive();
      }
      return archive;
    }
    virtual MessageType getType() const = 0;
    virtual MessageData* newCopy() const = 0;
};

class SolicitMessageData : public MessageData {
  private:
    SolicitMessageData();
  public:
    SolicitMessageData(const String& address);
    SolicitMessageData(IArchive& in);
    ~SolicitMessageData() {}
  private:
    String address; /* The address where the client expects the advertisement
                       to be sent */
  protected:
    void fillArchive();
  public:
    MessageType getType() const;
    MessageData* newCopy() const;
    inline const String& getAddress() const { return address; }
};

class AdvertiseMessageData : public MessageData {
  private:
  public:
    AdvertiseMessageData();
    AdvertiseMessageData(const String& serverName, const String& gameName);
    AdvertiseMessageData(IArchive& in);
    ~AdvertiseMessageData() {}
  private:
    String serverName;
    String gameName;
  protected:
    void fillArchive();
  public:
    MessageType getType() const;
    MessageData* newCopy() const;
    String getServerName() const;
    String getGameName() const;
};

class JoinMessageData : public MessageData {
  private:
    JoinMessageData();
  public:
    JoinMessageData(const String& address);
    JoinMessageData(IArchive& in);
    ~JoinMessageData() {}
  private:
    String address; /* The address where the client want all future
                       communication to be sent */
  protected:
    void fillArchive();
  public:
    MessageType getType() const;
    MessageData* newCopy() const;
    inline const String& getAddress() const { return address; }
};

class AcceptMessageData : public MessageData {
  private:
    AcceptMessageData();
  public:
    AcceptMessageData(const String& address, const ClientID& id);
    AcceptMessageData(IArchive& in);
    ~AcceptMessageData() {}
  private:
    String address; /* The address where the server want all future
                       communication from this client to be sent */
    ClientID id; /* The id assigned to this client */
  protected:
    void fillArchive();
  public:
    MessageType getType() const;
    MessageData* newCopy() const;
    inline const String& getAddress() const { return address; }
    inline const ClientID& getID() const { return id; }
};

class RejectMessageData : public MessageData {
  private:
    RejectMessageData();
  public:
    RejectMessageData(const String& reason);
    RejectMessageData(IArchive& in);
    ~RejectMessageData() {}
  private:
    String reason; /* The reason for rejection */
  protected:
    void fillArchive();
  public:
    MessageType getType() const;
    MessageData* newCopy() const;
    inline const String& getReason() const { return reason; }
};

class KickMessageData : public MessageData {
  private:
    KickMessageData();
  public:
    KickMessageData(const String& reason);
    KickMessageData(IArchive& in);
    ~KickMessageData() {}
  private:
    String reason; /* The reason for kicking */
  protected:
    void fillArchive();
  public:
    MessageType getType() const;
    MessageData* newCopy() const;
    inline const String& getReason() const { return reason; }
};

class LeaveMessageData : public MessageData {
  private:
  public:
    LeaveMessageData();
    LeaveMessageData(IArchive& in);
    ~LeaveMessageData() {}
  private:
  protected:
    void fillArchive();
  public:
    MessageType getType() const;
    MessageData* newCopy() const;
};

class GetSettingMessageData : public MessageData {
  private:
    GetSettingMessageData();
  public:
    GetSettingMessageData(const String& setting);
    GetSettingMessageData(IArchive& in);
    ~GetSettingMessageData() {}
  private:
    String setting; /* The full path of the setting to get */
  protected:
    void fillArchive();
  public:
    MessageType getType() const;
    MessageData* newCopy() const;
    inline const String& getSetting() const { return setting; }
};

class ChangeSettingMessageData : public MessageData {
  private:
    ChangeSettingMessageData();
  public:
    ChangeSettingMessageData(const String& setting, const String& value);
    ChangeSettingMessageData(IArchive& in);
    ~ChangeSettingMessageData() {}
  private:
    String setting; /* The full path of the setting to change */
    String value; /* The value to give to the setting */
  protected:
    void fillArchive();
  public:
    MessageType getType() const;
    MessageData* newCopy() const;
    inline const String& getSetting() const { return setting; }
    inline const String& getValue() const { return value; }
};

class NotifySettingMessageData : public MessageData {
  private:
    NotifySettingMessageData();
  public:
    NotifySettingMessageData(const String& setting, const String& value);
    NotifySettingMessageData(IArchive& in);
    ~NotifySettingMessageData() {}
  private:
    String setting; /* The full path of the setting being sent */
    String value; /* The value of the setting */
  protected:
    void fillArchive();
  public:
    MessageType getType() const;
    MessageData* newCopy() const;
    inline const String& getSetting() const { return setting; }
    inline const String& getValue() const { return value; }
};

class GameStartMessageData : public MessageData {
  public:
    GameStartMessageData();
    GameStartMessageData(IArchive& in);
    ~GameStartMessageData() {}
  private:
  protected:
    void fillArchive();
  public:
    MessageType getType() const;
    MessageData* newCopy() const;
};

class UpdateMessageData : public MessageData {
  private:
    UpdateMessageData();
  public:
    UpdateMessageData(const Update& update);
    UpdateMessageData(IArchive& in);
    ~UpdateMessageData() {}
  private:
    Update update; /* The address where the client want all future
                       communication to be sent */
  protected:
    void fillArchive();
  public:
    MessageType getType() const;
    MessageData* newCopy() const;
    inline const Update& getUpdate() const { return update; }
};

}}

#endif // MESSAGEDATA_H

