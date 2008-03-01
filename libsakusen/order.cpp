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

Order Order::load(IArchive& in, const DeserializationContext& context)
{
  OrderType type;
  in.extractEnum(type);
  
  /* To remind that update here is needed by causing a compile error when a
   * new enum value is added. */
  SAKUSEN_STATIC_ASSERT(orderType_max == 9);

  switch (type) {
    case orderType_none:
      return Order();
    case orderType_setVelocity:
      return Order(new SetVelocityOrderData(in));
    case orderType_move:
      return Order(new MoveOrderData(in));
    case orderType_setAngularVelocity:
      return Order(new SetAngularVelocityOrderData(in));
    case orderType_orient:
      return Order(new OrientOrderData(in));
    case orderType_targetPosition:
      return Order(new TargetPositionOrderData(in));
    case orderType_targetFrame:
      return Order(new TargetFrameOrderData(in));
    case orderType_targetUnit:
      return Order(new TargetUnitOrderData(in, context));
    case orderType_targetSensorReturns:
      return Order(new TargetSensorReturnsOrderData(in, context));
    default:
      throw EnumDeserializationExn("OrderType type", type);
  }
}

