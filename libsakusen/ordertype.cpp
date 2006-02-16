#include "ordertype.h"

#include "libsakusen-global.h"

#include <ostream>

namespace sakusen{

LIBSAKUSEN_API std::ostream& operator<<(
    std::ostream& output,
    OrderType orderType
  )
{
  switch (orderType) {
    #define CASE(type) case type: output << #type; break;
    CASE(orderType_none)
    CASE(orderType_setVelocity)
    CASE(orderType_move)
    #undef CASE
    default:
      output << "orderType_" << uint32(orderType);
      break;
  }
  return output;
}
}

