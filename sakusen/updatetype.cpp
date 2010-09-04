#include <sakusen/updatetype.h>
#include <sakusen/libsakusen-global.h>

#include <ostream>

using namespace sakusen;

std::ostream& sakusen::operator<<(std::ostream& output, UpdateType update)
{
  switch(update) {
    #define CASE(type) case type: output << #type; break;
    CASE(updateType_none)
    CASE(updateType_unitRemoved)
    CASE(updateType_unitAdded)
    CASE(updateType_unitAltered)
    CASE(updateType_orderAccepted)
    CASE(updateType_sensorReturnsRemoved)
    CASE(updateType_sensorReturnsAdded)
    CASE(updateType_sensorReturnsAltered)
    CASE(updateType_ballisticRemoved)
    CASE(updateType_ballisticAdded)
    #undef CASE
    default:
      output << "updateType_" << static_cast<uint32>(update);
      break;
  }
  return output;
}

