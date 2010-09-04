#include <sakusen/comms/messagedata.h>

#include <sakusen/libsakusen-global.h>
#include <sakusen/oarchive-methods.h>
#include <sakusen/world.h>

using namespace std;

namespace sakusen{
namespace comms{

MessageData::MessageData() :
  buffer()
{
}

MessageData::MessageData(const MessageData& copy) :
  buffer(copy.buffer)
{
}

MessageData::~MessageData()
{
}

JoinMessageData::JoinMessageData(const String& a) :
  MessageData(),
  address(a)
{
}

JoinMessageData::JoinMessageData(IArchive& in) :
  MessageData()
{
  in >> address;
}

void JoinMessageData::fillArchive(OArchive& archive) const
{
  archive << address;
}

MessageType JoinMessageData::getType() const
{
  return messageType_join;
}

AcceptMessageData::AcceptMessageData(const String& a, const ClientId& i) :
  MessageData(),
  address(a),
  id(i)
{
}

AcceptMessageData::AcceptMessageData(IArchive& in) :
  MessageData()
{
  in >> address;
  in >> id;
}

void AcceptMessageData::fillArchive(OArchive& archive) const
{
  archive << address;
  archive << id;
}

MessageType AcceptMessageData::getType() const
{
  return messageType_accept;
}

RejectMessageData::RejectMessageData(const String& r) :
  MessageData(),
  reason(r)
{
}

RejectMessageData::RejectMessageData(IArchive& in) :
  MessageData()
{
  in >> reason;
}

void RejectMessageData::fillArchive(OArchive& archive) const
{
  archive << reason;
}

MessageType RejectMessageData::getType() const
{
  return messageType_reject;
}

KickMessageData::KickMessageData(const String& r) :
  MessageData(),
  reason(r)
{
}

KickMessageData::KickMessageData(IArchive& in) :
  MessageData()
{
  in >> reason;
}

void KickMessageData::fillArchive(OArchive& archive) const
{
  archive << reason;
}

MessageType KickMessageData::getType() const
{
  return messageType_kick;
}

LeaveMessageData::LeaveMessageData() :
  MessageData()
{
}

LeaveMessageData::LeaveMessageData(IArchive&) :
  MessageData()
{
}

void LeaveMessageData::fillArchive(OArchive&) const
{
}

MessageType LeaveMessageData::getType() const
{
  return messageType_leave;
}

GetSettingMessageData::GetSettingMessageData(
    const String& s
  ) :
  MessageData(),
  setting(s)
{
}

GetSettingMessageData::GetSettingMessageData(IArchive& in) :
  MessageData()
{
  in >> setting;
}

void GetSettingMessageData::fillArchive(OArchive& archive) const
{
  archive << setting;
}

MessageType GetSettingMessageData::getType() const
{
  return messageType_getSetting;
}

ChangeSettingMessageData::ChangeSettingMessageData(
    const String& s,
    const String& v
  ) :
  MessageData(),
  setting(s),
  value(v)
{
}

ChangeSettingMessageData::ChangeSettingMessageData(IArchive& in) :
  MessageData()
{
  in >> setting;
  in >> value;
}

void ChangeSettingMessageData::fillArchive(OArchive& archive) const
{
  archive << setting;
  archive << value;
}

MessageType ChangeSettingMessageData::getType() const
{
  return messageType_changeSetting;
}

NotifySettingMessageData::NotifySettingMessageData(
    const String& s,
    bool l,
    const set<String>& v
  ) :
  MessageData(),
  setting(s),
  leaf(l),
  value(v)
{
}

NotifySettingMessageData::NotifySettingMessageData(IArchive& in) :
  MessageData()
{
  in >> setting >> leaf >> value;
}

void NotifySettingMessageData::fillArchive(OArchive& archive) const
{
  archive << setting << leaf << value;
}

MessageType NotifySettingMessageData::getType() const
{
  return messageType_notifySetting;
}

GameStartMessageData::GameStartMessageData(
    PlayerId pI,
    Topology t,
    const Point<sint32>& tR,
    const Point<sint32>& bL,
    uint32 d,
    uint16 g,
    const Heightfield& hf
  ) :
  MessageData(),
  playerId(pI),
  topology(t),
  topRight(tR),
  bottomLeft(bL),
  dexPerPixel(d),
  gravity(g),
  heightfield(hf)
{
}

GameStartMessageData::GameStartMessageData(
    IArchive& in,
    const DeserializationContext& context
  ) :
  MessageData(),
  /* Dummy call because there's no default constructor for Heightfield */
  heightfield(1, 1, 2, 2) 
{
  (in >> playerId).extractEnum(topology) >> topRight >> bottomLeft >>
    dexPerPixel >> gravity;
  heightfield = Heightfield::load(in, context);
}

void GameStartMessageData::fillArchive(OArchive& archive) const
{
  (archive << playerId).insertEnum(topology) << topRight << bottomLeft <<
    dexPerPixel << gravity;
  heightfield.store(archive);
}

MessageType GameStartMessageData::getType() const
{
  return messageType_gameStart;
}

OrderMessageData::OrderMessageData(const OrderMessage& oM) :
  MessageData(),
  orderMessage(oM)
{
}

OrderMessageData::OrderMessageData(
    IArchive& in,
    const DeserializationContext& context
  ) :
  MessageData(),
  orderMessage(OrderMessage::load(in, context))
{
}

void OrderMessageData::fillArchive(OArchive& archive) const
{
  orderMessage.store(archive);
}

MessageType OrderMessageData::getType() const
{
  return messageType_order;
}

/** \brief Constructs from given time and list of updates
 *
 * The list of updates passed to this method is cleared */
UpdateMessageData::UpdateMessageData(
    Time t,
    uint64 e,
    uint64 m,
    list<Update>& u
  ) :
  MessageData(),
  time(t),
  energy(e),
  metal(m),
  updates()
{
  updates.swap(u);
}

UpdateMessageData::UpdateMessageData(
    IArchive& in,
    const DeserializationContext& context
  ) :
  MessageData(),
  updates()
{
  in >> time >> energy >> metal;
  in.extract(updates, context);
}

void UpdateMessageData::fillArchive(OArchive& archive) const
{
  archive << time << energy << metal << updates;
}

MessageType UpdateMessageData::getType() const
{
  return messageType_update;
}

ExtensionMessageData::ExtensionMessageData(
    const String& e,
    uint16 v,
    const Buffer& d
  ) :
  extension(e),
  version(v),
  data(d)
{
}

ExtensionMessageData::ExtensionMessageData(IArchive& in)
{
  in >> extension >> version >> data;
}

void ExtensionMessageData::fillArchive(OArchive& archive) const
{
  archive << extension << version << data;
}

MessageType ExtensionMessageData::getType() const
{
  return messageType_extension;
}

}}
