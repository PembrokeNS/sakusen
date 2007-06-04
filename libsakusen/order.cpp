#include "order.h"

using namespace sakusen;

/** \brief Constructs an empty Order */
Order::Order() :
  type(orderType_none),
  data()
{
}

/** \brief Constructs an order around given data.
 *
 * Ownership of pointer to data is transferred to this.
 */
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
    case orderType_targetPosition:
      return Order(new TargetPositionOrderData(in));
    case orderType_targetPositionOrientation:
      return Order(new TargetPositionOrientationOrderData(in));
    case orderType_targetUnit:
      return Order(new TargetUnitOrderData(in, player));
    case orderType_targetSensorReturns:
      return Order(new TargetSensorReturnsOrderData(in, player));
    default:
      throw EnumDeserializationExn("OrderType type", type);
  }
}

