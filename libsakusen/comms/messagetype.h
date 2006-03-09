#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

#include <iosfwd>

namespace sakusen {
namespace comms {

/** This contains the magic numbers that indicate what type of message the bearer is.
 * \note The current serialization method for dealing with enums limits us to 256 message types.
 */
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
  messageType_order,
  messageType_update
};

std::ostream& operator<<(std::ostream& output, MessageType type);

}}

#endif // MESSAGETYPE_H

