#ifndef LIBSAKUSEN_COMMS__MESSAGETYPE_H
#define LIBSAKUSEN_COMMS__MESSAGETYPE_H

#include "libsakusen-comms-global.h"

#include <iosfwd>

namespace sakusen {
namespace comms {

/** This contains the magic numbers that indicate what type of message the bearer is.
 * \note The current serialization method for dealing with enums limits us to 256 message types.
 */
enum MessageType {
  messageType_none,
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
  messageType_update,
  messageType_extension
};

LIBSAKUSEN_COMMS_API std::ostream& operator<<(std::ostream& output, MessageType type);

}}

#endif // LIBSAKUSEN_COMMS__MESSAGETYPE_H

