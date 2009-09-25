#include "ordertype.h"

#include "libsakusen-global.h"

#include <ostream>

namespace sakusen{

LIBSAKUSEN_API std::ostream& operator<<(
    std::ostream& output,
    OrderType orderType
  )
{
  /* To remind that update here is needed by causing a compile error when a
   * new enum value is added. */
  SAKUSEN_STATIC_ASSERT(orderType_max == 11);
  
  switch (orderType) {
    #define CASE(type)              \
    case orderType_##type:          \
      output << "orderType_" #type; \
      break;
    CASE(none)
    CASE(setVelocity)
    CASE(move)
    CASE(setAngularVelocity)
    CASE(orient)
    CASE(targetNone)
    CASE(targetNumber)
    CASE(targetPosition)
    CASE(targetFrame)
    CASE(targetUnit)
    CASE(targetSensorReturns)
    #undef CASE
    default:
      output << "orderType_" << uint32(orderType);
      break;
  }
  return output;
}
}

