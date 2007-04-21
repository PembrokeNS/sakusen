#include "unitorders.h"

#include "oarchive-methods.h"

using namespace std;
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
    const AngularVelocity& tAV,
    const vector<WeaponOrders>& wO
  ) :
  currentOrder(cO),
  linearTarget(lT),
  targetPosition(tP),
  targetVelocity(tV),
  rotationalTarget(rT),
  targetOrientation(tO),
  targetAngularVelocity(tAV),
  weaponOrders(wO)
{
  for (int i=0; i<orderCondition_max; ++i) {
    orders[i] = ord[i];
  }
}

UnitOrders::UnitOrders(uint16 numWeapons) :
  linearTarget(linearTargetType_none),
  rotationalTarget(rotationalTargetType_none),
  weaponOrders(numWeapons)
{
  while (weaponOrders.size() < numWeapons) {
    weaponOrders.push_back(WeaponOrders());
  }
}

/** Makes the queued order with condition \p condition the current order, and
 * updates the state to follow the order.
 */
void UnitOrders::acceptOrder(OrderCondition condition)
{
  assert(condition < orderCondition_max);
  /* accept the order */
  Order thisOrder;
  if (condition == orderCondition_incidental) {
    thisOrder = orders[condition];
    orders[condition] = Order();
  } else {
    thisOrder = currentOrder = orders[condition];
    clearQueue();
  }

  /* Alter the Unit's state appropriately for the order */
  switch (thisOrder.getType()) {
    case orderType_setVelocity:
      linearTarget = linearTargetType_velocity;
      targetVelocity = thisOrder.getSetVelocityData().getTarget();
      break;
    case orderType_move:
      linearTarget = linearTargetType_position;
      targetPosition = thisOrder.getMoveData().getTarget();
      break;
    case orderType_targetPosition:
    case orderType_targetPositionOrientation:
    case orderType_targetSensorReturns:
      {
        uint16 weaponIndex;
        switch(thisOrder.getType()) {
          case orderType_targetPosition:
            weaponIndex = thisOrder.getTargetPositionData().getWeaponIndex();
            break;
          case orderType_targetPositionOrientation:
            weaponIndex = thisOrder.getTargetPositionOrientationData().getWeaponIndex();
            break;
          case orderType_targetSensorReturns:
            weaponIndex = thisOrder.getTargetSensorReturnsData().getWeaponIndex();
            break;
          default:
          Fatal("Unknown OrderType " << thisOrder.getType());
        }
        if (weaponIndex >= weaponOrders.size()) {
          Debug("Weapon index out of range");
          break;
        }
        weaponOrders[weaponIndex].update(thisOrder);
      }
      break;
    default:
      Fatal("Unknown OrderType " << thisOrder.getType());
  }
}

/** Clears all queued orders. */
void UnitOrders::clearQueue()
{
  /* Clear all orders from the queue */
  for (int i=0; i<orderCondition_max; i++) {
    orders[i] = Order();
  }
}

void UnitOrders::store(OArchive& out) const
{
  out << orders;
  currentOrder.store(out);
  (out.insertEnum(linearTarget) << targetPosition <<
    targetVelocity).insertEnum(rotationalTarget);
  targetOrientation.store(out);
  out << targetAngularVelocity << weaponOrders;
}

UnitOrders UnitOrders::load(IArchive& in, const PlayerID* player)
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
  vector<WeaponOrders> weaponOrders;

  in.extract<Order, orderCondition_max>(orders, player);
  currentOrder = Order::load(in, player);
  in.extractEnum(linearTarget);
  in >> targetPosition >> targetVelocity;
  in.extractEnum(rotationalTarget);
  targetOrientation = Orientation::load(in);
  in >> targetAngularVelocity;
  in.extract(weaponOrders, player);

  return UnitOrders(
      orders, currentOrder, linearTarget,
      targetPosition, targetVelocity,
      rotationalTarget, targetOrientation, targetAngularVelocity, weaponOrders
    );
}

