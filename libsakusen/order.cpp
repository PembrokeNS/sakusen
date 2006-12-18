#include "order.h"

using namespace sakusen;

Order::Order() :
  type(orderType_none),
  data()
{
}

Order::Order(const OrderData* d) :
  type(d->getType()),
  data(d)
{
}

void Order::store(OArchive& out) const
{
  out.insertEnum(type);
  if (data) {
    data->store(out);
  }
}

Order Order::load(IArchive& in, const PlayerID* player)
{
  OrderType type;
  in.extractEnum(type);
  
  switch (type) {
    case orderType_none:
      return Order();
    case orderType_move:
      return Order(new MoveOrderData(in));
    case orderType_setVelocity:
      return Order(new SetVelocityOrderData(in));
    case orderType_targetPoint:
      return Order(new TargetPointOrderData(in));
    case orderType_targetSensorReturns:
      return Order(new TargetSensorReturnsOrderData(in, player));
    default:
      throw EnumDeserializationExn("type", type);
  }
}

