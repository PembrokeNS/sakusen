#include "messagetype.h"
#include "libsakusen-global.h"

#include <ostream>

using namespace sakusen::comms;

std::ostream& sakusen::comms::operator<<(std::ostream& out, MessageType type)
{
  switch (type) {
#define CASE(name) case messageType_##name: out << "messageType_"#name; break;
    CASE(none)
    CASE(solicit)
    CASE(advertise)
    CASE(join)
    CASE(accept)
    CASE(reject)
    CASE(kick)
    CASE(leave)
    CASE(getSetting)
    CASE(changeSetting)
    CASE(notifySetting)
    CASE(order)
    CASE(update)
    CASE(extension)
#undef CASE
    default:
      out << "messageType_" << static_cast<uint32>(type);
      break;
  }
  return out;
}

