#include "updatetype.h"
#include "libsakusen-global.h"

#include <ostream>

using namespace sakusen;

std::ostream& sakusen::operator<<(std::ostream& output, UpdateType update)
{
  switch(update) {
    #define CASE(type) case type: output << #type; break;
    CASE(updateType_unitRemoved)
    CASE(updateType_unitAdded)
    CASE(updateType_orderQueued)
    CASE(updateType_orderCompleted)
    CASE(updateType_orderAccepted)
    #undef CASE
    default:
      output << "updateType_" << static_cast<uint32>(update);
      break;
  }
  return output;
}

