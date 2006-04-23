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

void OrderMessage::store(OArchive& out) const
{
  (out << orderee).insertEnum(condition);
  order.store(out);
}

OrderMessage OrderMessage::load(IArchive& in)
{
  uint32 orderee;
  OrderCondition orderCondition;
  
  in >> orderee;
  in.extractEnum(orderCondition);
  Order order(Order::load(in));

  return OrderMessage(orderee, orderCondition, order);
}

