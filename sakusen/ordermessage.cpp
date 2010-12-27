#include <sakusen/ordermessage.h>

using namespace sakusen;

OrderMessage::OrderMessage(
    UnitId oee,
    const Order& o) :
  orderee(oee),
  order(o)
{
}

void OrderMessage::store(OArchive& out) const
{
  out << orderee;
  order.store(out);
}

OrderMessage OrderMessage::load(
    IArchive& in,
    const DeserializationContext& context
  )
{
  UnitId orderee;
  
  in >> orderee;
  Order order(Order::load(in, context));

  return OrderMessage(orderee, order);
}

