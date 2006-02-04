#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

#include <iosfwd>

namespace sakusen {
namespace comms {

/* Note that current serialization implementation limits us to 256 types */
enum MessageType {
  messageType_none,
  messageType_solicit,
  messageType_advertise,
  messageType_join,
  messageType_accept,
  messageType_reject,
  messageType_kick,
  messageType_leave,
  messageType_getSetting,
  messageType_changeSetting,
  messageType_notifySetting,
  messageType_gameStart,
  messageType_update
};

std::ostream& operator<<(std::ostream& output, MessageType type);

}}

#endif // MESSAGETYPE_H

