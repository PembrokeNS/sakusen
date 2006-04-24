#include "unitorders.h"

#include "oarchive-methods.h"

using namespace sakusen;

UnitOrders::UnitOrders(
    /* conditioned orders */
    Order ord[orderCondition_max],
    const Order& cO,
    /* current goals which have been set by orders */
    const LinearTargetType& lT,
    const Point<sint32>& tP,
    const Point<sint16>& tV,
    const RotationalTargetType& rT,
    const Orientation& tO,
    const AngularVelocity& tAV
  ) :
  currentOrder(cO),
  linearTarget(lT),
  targetPosition(tP),
  targetVelocity(tV),
  rotationalTarget(rT),
  targetOrientation(tO),
  targetAngularVelocity(tAV)
{
  for (int i=0; i<orderCondition_max; ++i) {
    orders[i] = ord[i];
  }
}

UnitOrders::UnitOrders() :
  linearTarget(linearTargetType_none),
  rotationalTarget(rotationalTargetType_none)
{
}

void UnitOrders::acceptOrder(OrderCondition condition)
{
  assert(condition < orderCondition_max);
  const Order& newOrder = orders[condition];
  /* accept the order */
  currentOrder = newOrder;
  
  clear();

  /* Alter the Unit's state appropriately for the order */
  switch (currentOrder.getOrderType()) {
    case orderType_setVelocity:
      linearTarget = linearTargetType_velocity;
      targetVelocity = currentOrder.getSetVelocityData().getTarget();
      break;
    case orderType_move:
      linearTarget = linearTargetType_position;
      targetPosition = currentOrder.getMoveData().getTarget();
      break;
    default:
      Fatal("Unknown OrderType " << currentOrder.getOrderType());
  }
}

void UnitOrders::clear()
{
  /* Clear all orders from the queue */
  for (int i=0; i<orderCondition_max; i++) {
    orders[i] = Order();
  }
}

void UnitOrders::store(OArchive& out) const
{
  out.insert<Order, orderCondition_max>(orders);
  currentOrder.store(out);
  (out.insertEnum(linearTarget) << targetPosition <<
    targetVelocity).insertEnum(rotationalTarget);
  targetOrientation.store(out);
  out << targetAngularVelocity;
}

UnitOrders UnitOrders::load(IArchive& in)
{
  /* conditioned orders */
  Order orders[orderCondition_max];
  Order currentOrder;
  /* current goals which have been set by orders */
  LinearTargetType linearTarget;
  Point<sint32> targetPosition;
  Point<sint16> targetVelocity;
  RotationalTargetType rotationalTarget;
  Orientation targetOrientation;
  AngularVelocity targetAngularVelocity;

  in.extract<Order, orderCondition_max>(orders);
  currentOrder = Order::load(in);
  in.extractEnum(linearTarget);
  in >> targetPosition >> targetVelocity;
  in.extractEnum(rotationalTarget);
  targetOrientation = Orientation::load(in);
  in >> targetAngularVelocity;

  return UnitOrders(
      orders, currentOrder, linearTarget,
      targetPosition, targetVelocity,
      rotationalTarget, targetOrientation, targetAngularVelocity
    );
}

