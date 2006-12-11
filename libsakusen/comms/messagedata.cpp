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

SolicitMessageData::SolicitMessageData(const String& a) :
  MessageData(),
  address(a)
{
}

SolicitMessageData::SolicitMessageData(IArchive& in) :
  MessageData()
{
  in >> address;
}

void SolicitMessageData::fillArchive() const
{
  archive << address;
}

MessageType SolicitMessageData::getType() const
{
  return messageType_solicit;
}

MessageData* SolicitMessageData::newCopy() const
{
  return new SolicitMessageData(*this);
}

AdvertiseMessageData::AdvertiseMessageData() :
  MessageData(),
  serverName(""),
  gameName("")
{
}

AdvertiseMessageData::AdvertiseMessageData(const String& sN, const String& gN) :
  MessageData(),
  serverName(sN),
  gameName(gN)
{
}

AdvertiseMessageData::AdvertiseMessageData(IArchive& in) :
  MessageData()
{
  in >> serverName;
  in >> gameName;
}

void AdvertiseMessageData::fillArchive() const
{
  archive << serverName;
  archive << gameName;
}

MessageType AdvertiseMessageData::getType() const
{
  return messageType_advertise;
}

MessageData* AdvertiseMessageData::newCopy() const
{
  return new AdvertiseMessageData(*this);
}

String AdvertiseMessageData::getServerName() const
{
  return serverName;
}

String AdvertiseMessageData::getGameName() const
{
  return gameName;
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

MessageData* JoinMessageData::newCopy() const
{
  return new JoinMessageData(*this);
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

MessageData* AcceptMessageData::newCopy() const
{
  return new AcceptMessageData(*this);
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

MessageData* RejectMessageData::newCopy() const
{
  return new RejectMessageData(*this);
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

MessageData* KickMessageData::newCopy() const
{
  return new KickMessageData(*this);
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

MessageData* LeaveMessageData::newCopy() const
{
  return new LeaveMessageData(*this);
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

MessageData* GetSettingMessageData::newCopy() const
{
  return new GetSettingMessageData(*this);
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

MessageData* ChangeSettingMessageData::newCopy() const
{
  return new ChangeSettingMessageData(*this);
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

MessageData* NotifySettingMessageData::newCopy() const
{
  return new NotifySettingMessageData(*this);
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

MessageData* GameStartMessageData::newCopy() const
{
  return new GameStartMessageData(*this);
}

OrderMessageData::OrderMessageData(const OrderMessage& oM) :
  MessageData(),
  orderMessage(oM)
{
}

OrderMessageData::OrderMessageData(IArchive& in, const PlayerID* player) :
  MessageData(),
  orderMessage(OrderMessage::load(in, player))
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

MessageData* OrderMessageData::newCopy() const
{
  return new OrderMessageData(*this);
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

UpdateMessageData::UpdateMessageData(IArchive& in, const PlayerID* player) :
  MessageData(),
  updates()
{
  in >> time;
  in.extract(updates, world->getUniverse(), player);
}

void UpdateMessageData::fillArchive() const
{
  archive << time << updates;
}

MessageType UpdateMessageData::getType() const
{
  return messageType_update;
}

MessageData* UpdateMessageData::newCopy() const
{
  return new UpdateMessageData(*this);
}

}}
