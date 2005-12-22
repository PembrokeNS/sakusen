#include "messagetype.h"
#include "libsakusen-global.h"

#include <ostream>

std::ostream& operator<<(std::ostream& out, MessageType type)
{
  switch (type) {
#define CASE(name) case name: out << #name; break;
    CASE(messageType_none)
    CASE(messageType_solicit)
    CASE(messageType_advertise)
    CASE(messageType_join)
    CASE(messageType_accept)
    CASE(messageType_reject)
    CASE(messageType_kick)
    CASE(messageType_leave)
    CASE(messageType_getSetting)
    CASE(messageType_changeSetting)
    CASE(messageType_notifySetting)
    CASE(messageType_update)
#undef CASE
    default:
      out << "messageType_" << static_cast<uint32>(type);
      break;
  }
  return out;
}

