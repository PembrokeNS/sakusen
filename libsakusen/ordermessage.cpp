#include "ordermessage.h"

using namespace sakusen;

OrderMessage::OrderMessage(
    uint32 oee,
    OrderCondition c,
    const Order& o) :
  orderee(oee),
  condition(c),
  order(o)
{
}

