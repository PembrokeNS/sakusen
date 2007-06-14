#ifndef MESSAGEDATA_H
#define MESSAGEDATA_H

#include "messagetype.h"
#include "oarchive.h"
#include "iarchive.h"
#include "update.h"
#include "libsakusen-comms-global.h"
#include "clientid.h"
#include "topology.h"
#include "ordermessage.h"

namespace sakusen {
namespace comms {

class LIBSAKUSEN_COMMS_API MessageData {
  public:
    typedef boost::shared_ptr<MessageData> Ptr;
    typedef boost::shared_ptr<const MessageData> ConstPtr;
  protected:
    /* Abstract class - all constructors must be protected */
    MessageData();
    MessageData(const MessageData& copy);
    mutable OArchive archive;
    
    virtual void fillArchive() const = 0;
  public:
    virtual ~MessageData();
    const OArchive& getArchive() const {
      if (archive.getLength() == 0) {
        (archive << uint8(NETWORK_PROTOCOL_VERSION)).insertEnum(getType());
        fillArchive();
      }
      return archive;
    }
    virtual MessageType getType() const = 0;
};

class LIBSAKUSEN_COMMS_API JoinMessageData : public MessageData {
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
    void fillArchive() const;
  public:
    MessageType getType() const;
    inline const String& getAddress() const { return address; }
};

class LIBSAKUSEN_COMMS_API AcceptMessageData : public MessageData {
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
    void fillArchive() const;
  public:
    MessageType getType() const;
    inline const String& getAddress() const { return address; }
    inline const ClientID& getID() const { return id; }
};

class LIBSAKUSEN_COMMS_API RejectMessageData : public MessageData {
  private:
    RejectMessageData();
  public:
    RejectMessageData(const String& reason);
    RejectMessageData(IArchive& in);
    ~RejectMessageData() {}
  private:
    String reason; /* The reason for rejection */
  protected:
    void fillArchive() const;
  public:
    MessageType getType() const;
    inline const String& getReason() const { return reason; }
};

class LIBSAKUSEN_COMMS_API KickMessageData : public MessageData {
  private:
    KickMessageData();
  public:
    KickMessageData(const String& reason);
    KickMessageData(IArchive& in);
    ~KickMessageData() {}
  private:
    String reason; /* The reason for kicking */
  protected:
    void fillArchive() const;
  public:
    MessageType getType() const;
    inline const String& getReason() const { return reason; }
};

class LIBSAKUSEN_COMMS_API LeaveMessageData : public MessageData {
  private:
  public:
    LeaveMessageData();
    LeaveMessageData(IArchive& in);
    ~LeaveMessageData() {}
  private:
  protected:
    void fillArchive() const;
  public:
    MessageType getType() const;
};

class LIBSAKUSEN_COMMS_API GetSettingMessageData : public MessageData {
  private:
    GetSettingMessageData();
  public:
    GetSettingMessageData(const String& setting);
    GetSettingMessageData(IArchive& in);
    ~GetSettingMessageData() {}
  private:
    String setting; /* The full path of the setting to get */
  protected:
    void fillArchive() const;
  public:
    MessageType getType() const;
    inline const String& getSetting() const { return setting; }
};

class LIBSAKUSEN_COMMS_API ChangeSettingMessageData : public MessageData {
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
    void fillArchive() const;
  public:
    MessageType getType() const;
    inline const String& getSetting() const { return setting; }
    inline const String& getValue() const { return value; }
};

class LIBSAKUSEN_COMMS_API NotifySettingMessageData : public MessageData {
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
    void fillArchive() const;
  public:
    MessageType getType() const;
    inline const String& getSetting() const { return setting; }
    inline const String& getValue() const { return value; }
};

/** \brief Message providing data needed by client to initialize the game
 *
 * \todo We don't want to actually pass this much information (in particular,
 * we do not wish to reveal the map bounds in advance), but for the moment we
 * do so, because it makes writing the client much easier. */
class LIBSAKUSEN_COMMS_API GameStartMessageData : public MessageData {
  private:
    GameStartMessageData();
  public:
    GameStartMessageData(
        PlayerID playerId,
        Topology topology,
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        uint32 horizontalHeightfieldRes,
        uint32 verticalHeightfieldRes
      );
    GameStartMessageData(IArchive& in);
    ~GameStartMessageData() {}
  private:
    PlayerID playerId;
    Topology topology;
    Point<sint32> topRight;
    Point<sint32> bottomLeft;
    uint16 gravity;
    uint32 horizontalHeightfieldRes;
    uint32 verticalHeightfieldRes;
  protected:
    void fillArchive() const;
  public:
    MessageType getType() const;
    inline PlayerID getPlayerId() const { return playerId; }
    inline Topology getTopology() const { return topology; }
    inline const Point<sint32>& getTopRight() const { return topRight; }
    inline const Point<sint32>& getBottomLeft() const { return bottomLeft; }
    inline uint16 getGravity() const { return gravity; }
    inline uint32 getHorizontalHeightfieldResolution() const {
      return horizontalHeightfieldRes;
    }
    inline uint32 getVerticalHeightfieldResolution() const {
      return verticalHeightfieldRes;
    }
};

class LIBSAKUSEN_COMMS_API OrderMessageData : public MessageData {
  private:
    OrderMessageData();
  public:
    OrderMessageData(const OrderMessage&);
    OrderMessageData(IArchive&, const DeserializationContext&);
    ~OrderMessageData() {}
  private:
    OrderMessage orderMessage; /**< The order message */
  protected:
    void fillArchive() const;
  public:
    MessageType getType() const;
    inline const OrderMessage& getOrderMessage() const { return orderMessage; }
};

class LIBSAKUSEN_COMMS_API UpdateMessageData : public MessageData {
  private:
    UpdateMessageData();
  public:
    UpdateMessageData(Time time, std::list<Update>& update);
    UpdateMessageData(IArchive&, const DeserializationContext&);
    ~UpdateMessageData() {}
  private:
    Time time; /**< The game time at which these updates apply */
    std::list<Update> updates; /**< The updates */
  protected:
    void fillArchive() const;
  public:
    MessageType getType() const;
    inline Time getTime() const { return time; }
    inline const std::list<Update>& getUpdates() const { return updates; }
};

/** \brief Data for a message for some network protocol extension
 *
 * This is a generic MessageData for use by extensions to the network protocol.
 * The server will forward it to those clients who claim to support the
 * extension.
 *
 * \todo Allow the mesasge to specify a subset of the clients for forwarding
 * to, and whether it should be stored in the replay
 */
class LIBSAKUSEN_COMMS_API ExtensionMessageData : public MessageData {
  public:
    ExtensionMessageData(
        const String& extension,
        uint16 version,
        const OArchive& data
      );
    ExtensionMessageData(IArchive& in);
    ~ExtensionMessageData() {}
  private:
    String extension; /**< The name of the extension */
    uint16 version; /**< The version number of the extension */
    OArchive oData; /**< The associated data */
    IArchive iData; /**< The associated data */
  protected:
    void fillArchive() const;
  public:
    MessageType getType() const;
    inline const String& getExtension() const { return extension; }
    inline uint16 getVersion() const { return version; }
    inline const IArchive& getData() const { return iData; }
};

}}

#endif // MESSAGEDATA_H

