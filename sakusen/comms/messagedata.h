#ifndef LIBSAKUSEN_COMMS__MESSAGEDATA_H
#define LIBSAKUSEN_COMMS__MESSAGEDATA_H

#include <sakusen/comms/global.h>
#include <sakusen/comms/messagetype.h>
#include <sakusen/oarchive.h>
#include <sakusen/iarchive.h>
#include <sakusen/update.h>
#include <sakusen/clientid.h>
#include <sakusen/topology.h>
#include <sakusen/ordermessage.h>

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
    mutable Buffer buffer;
    
    virtual void fillArchive(OArchive&) const = 0;
  public:
    virtual ~MessageData();
    const Buffer& getBuffer() const {
      if (buffer.getSize() == 0) {
        OArchive archive;
        archive << uint8(SAKUSEN_COMMS_NETWORK_PROTOCOL_VERSION);
        archive.insertEnum(getType());
        fillArchive(archive);
        buffer = archive.getBuffer();
      }
      return buffer;
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
    void fillArchive(OArchive& archive) const;
  public:
    MessageType getType() const;
    inline const String& getAddress() const { return address; }
};

class LIBSAKUSEN_COMMS_API AcceptMessageData : public MessageData {
  private:
    AcceptMessageData();
  public:
    AcceptMessageData(const String& address, const ClientId& id);
    AcceptMessageData(IArchive& in);
    ~AcceptMessageData() {}
  private:
    String address; /* The address where the server want all future
                       communication from this client to be sent */
    ClientId id; /* The id assigned to this client */
  protected:
    void fillArchive(OArchive& archive) const;
  public:
    MessageType getType() const;
    inline const String& getAddress() const { return address; }
    inline const ClientId& getId() const { return id; }
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
    void fillArchive(OArchive& archive) const;
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
    void fillArchive(OArchive& archive) const;
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
    void fillArchive(OArchive& archive) const;
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
    void fillArchive(OArchive& archive) const;
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
    void fillArchive(OArchive& archive) const;
  public:
    MessageType getType() const;
    inline const String& getSetting() const { return setting; }
    inline const String& getValue() const { return value; }
};

class LIBSAKUSEN_COMMS_API NotifySettingMessageData : public MessageData {
  private:
    NotifySettingMessageData();
  public:
    NotifySettingMessageData(
        const String& setting,
        bool leaf,
        const std::set<String>& value
      );
    NotifySettingMessageData(IArchive& in);
    ~NotifySettingMessageData() {}
  private:
    String setting; /* The full path of the setting being sent */
    bool leaf;
    std::set<String> value; /* The value of the setting */
  protected:
    void fillArchive(OArchive& archive) const;
  public:
    MessageType getType() const;
    inline const String& getSetting() const { return setting; }
    inline bool isLeaf() const { return leaf; }
    /** \brief The value of the setting.
     *
     * If the setting is a branch, a set of names of its children.
     * If the setting is a string set leaf, the set of its values.
     * Otherwise, a singleton set containing a string representation of the
     * value of the setting.
     * */
    inline const std::set<String>& getValue() const { return value; }
};

/** \brief Message providing data needed by client to initialize the game
 *
 * \todo We don't want to actually pass this much information (in particular,
 * we do not wish to reveal the map
 * bounds & heighfield in advance), but for the moment we
 * do so, because it makes writing the client much easier. */
class LIBSAKUSEN_COMMS_API GameStartMessageData : public MessageData {
  public:
    GameStartMessageData(
        PlayerId playerId,
        Topology topology,
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint32 dexPerPixel,
        uint16 gravity,
        const Heightfield& hf
      );
    GameStartMessageData(IArchive& in, const DeserializationContext&);
    ~GameStartMessageData() {}
  private:
    PlayerId playerId;
    Topology topology;
    Point<sint32> topRight;
    Point<sint32> bottomLeft;
    uint32 dexPerPixel;
    uint16 gravity;
    Heightfield heightfield;
  protected:
    void fillArchive(OArchive& archive) const;
  public:
    MessageType getType() const;
    inline PlayerId getPlayerId() const { return playerId; }
    inline Topology getTopology() const { return topology; }
    inline const Point<sint32>& getTopRight() const { return topRight; }
    inline const Point<sint32>& getBottomLeft() const { return bottomLeft; }
    inline uint32 getDexPerPixel() const { return dexPerPixel; }
    inline uint16 getGravity() const { return gravity; }
    inline const Heightfield& getHeightfield() const {
      return heightfield;
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
    void fillArchive(OArchive& archive) const;
  public:
    MessageType getType() const;
    inline const OrderMessage& getOrderMessage() const { return orderMessage; }
};

class LIBSAKUSEN_COMMS_API UpdateMessageData : public MessageData {
  private:
    UpdateMessageData();
  public:
    UpdateMessageData(
        Time time,
        uint64 energy,
        uint64 metal,
        std::list<Update>& update
      );
    UpdateMessageData(IArchive&, const DeserializationContext&);
    ~UpdateMessageData() {}
  private:
    Time time; /**< The game time at which these updates apply */
    uint64 energy; /**< Available energy */
    uint64 metal; /**< Available metal */
    std::list<Update> updates; /**< The updates */
  protected:
    void fillArchive(OArchive& archive) const;
  public:
    MessageType getType() const;
    uint64 getEnergy() const { return energy; }
    uint64 getMetal() const { return metal; }
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
        const Buffer& data
      );
    ExtensionMessageData(IArchive& in);
    ~ExtensionMessageData() {}
  private:
    String extension; /**< The name of the extension */
    uint16 version; /**< The version number of the extension */
    Buffer data; /**< The associated data */
  protected:
    void fillArchive(OArchive& archive) const;
  public:
    MessageType getType() const;
    inline const String& getExtension() const { return extension; }
    inline uint16 getVersion() const { return version; }
    inline const Buffer& getData() const { return data; }
};

}}

#endif // LIBSAKUSEN_COMMS__MESSAGEDATA_H

