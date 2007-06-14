#include "messagedata.h"

#include "libsakusen-global.h"
#include "oarchive-methods.h"
#include "world.h"

using namespace std;

namespace sakusen{
namespace comms{

MessageData::MessageData() :
  archive()
{
}

MessageData::MessageData(const MessageData& copy) :
  archive(copy.archive)
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

void JoinMessageData::fillArchive() const
{
  archive << address;
}

MessageType JoinMessageData::getType() const
{
  return messageType_join;
}

AcceptMessageData::AcceptMessageData(const String& a, const ClientID& i) :
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

void AcceptMessageData::fillArchive() const
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

void RejectMessageData::fillArchive() const
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

void KickMessageData::fillArchive() const
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

void LeaveMessageData::fillArchive() const
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

void GetSettingMessageData::fillArchive() const
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

void ChangeSettingMessageData::fillArchive() const
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
    const String& v
  ) :
  MessageData(),
  setting(s),
  value(v)
{
}

NotifySettingMessageData::NotifySettingMessageData(IArchive& in) :
  MessageData()
{
  in >> setting;
  in >> value;
}

void NotifySettingMessageData::fillArchive() const
{
  archive << setting;
  archive << value;
}

MessageType NotifySettingMessageData::getType() const
{
  return messageType_notifySetting;
}

GameStartMessageData::GameStartMessageData(
    PlayerID pI,
    Topology t,
    const Point<sint32>& tR,
    const Point<sint32>& bL,
    uint16 g,
    uint32 hHR,
    uint32 vHR
  ) :
  MessageData(),
  playerId(pI),
  topology(t),
  topRight(tR),
  bottomLeft(bL),
  gravity(g),
  horizontalHeightfieldRes(hHR),
  verticalHeightfieldRes(vHR)
{
}

GameStartMessageData::GameStartMessageData(IArchive& in) :
  MessageData()
{
  (in >> playerId).extractEnum(topology) >> topRight >> bottomLeft >>
    gravity >> horizontalHeightfieldRes >> verticalHeightfieldRes;
}

void GameStartMessageData::fillArchive() const
{
  (archive << playerId).insertEnum(topology) << topRight << bottomLeft <<
    gravity << horizontalHeightfieldRes << verticalHeightfieldRes;
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

void OrderMessageData::fillArchive() const
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
UpdateMessageData::UpdateMessageData(Time t, list<Update>& u) :
  MessageData(),
  time(t),
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
  in >> time;
  in.extract(updates, context);
}

void UpdateMessageData::fillArchive() const
{
  archive << time << updates;
}

MessageType UpdateMessageData::getType() const
{
  return messageType_update;
}

ExtensionMessageData::ExtensionMessageData(
    const String& e,
    uint16 v,
    const OArchive& d
  ) :
  extension(e),
  version(v),
  oData(d),
  iData(d)
{
}

ExtensionMessageData::ExtensionMessageData(IArchive& in)
{
  in >> extension >> version >> iData;
  oData = iData;
}

void ExtensionMessageData::fillArchive() const
{
  archive << extension << version << oData;
}

MessageType ExtensionMessageData::getType() const
{
  return messageType_extension;
}

}}
