#include "ordermessage.h"

using namespace sakusen;

OrderMessage::OrderMessage(
    uint32 oee,
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

OrderMessage OrderMessage::load(IArchive& in, const PlayerID* player)
{
  uint32 orderee;
  
  in >> orderee;
  Order order(Order::load(in, player));

  return OrderMessage(orderee, order);
}

