#include "unitorders.h"

#include "oarchive-methods.h"

using namespace std;
using namespace sakusen;

UnitOrders::UnitOrders(
    /* current goals which have been set by orders */
    const LinearTargetType& lT,
    const Point<sint32>& tP,
    const Point<sint16>& tV,
    const RotationalTargetType& rT,
    const Orientation& tO,
    const AngularVelocity& tAV,
    const std::vector<WeaponOrders>& wO
  ) :
  linearTarget(lT),
  targetPosition(tP),
  targetVelocity(tV),
  rotationalTarget(rT),
  targetOrientation(tO),
  targetAngularVelocity(tAV),
  weaponOrders(wO)
{
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
void UnitOrders::acceptOrder(const Order& order)
{
  /* To remind that update here is needed by causing a compile error when a
   * new enum value is added. */
  SAKUSEN_STATIC_ASSERT(orderType_max == 11);

  /* Alter the Unit's state appropriately for the order */
  switch (order.getType()) {
    case orderType_setVelocity:
      linearTarget = linearTargetType_velocity;
      targetVelocity = order.getSetVelocityData().getTarget();
      break;
    case orderType_move:
      linearTarget = linearTargetType_position;
      targetPosition = order.getMoveData().getTarget();
      break;
    case orderType_setAngularVelocity:
      rotationalTarget = rotationalTargetType_angularVelocity;
      targetAngularVelocity = order.getSetAngularVelocityData().getTarget();
      break;
    case orderType_orient:
      rotationalTarget = rotationalTargetType_orientation;
      targetOrientation = order.getOrientData().getTarget();
      break;
    case orderType_targetNone:
    case orderType_targetNumber:
    case orderType_targetPosition:
    case orderType_targetFrame:
    case orderType_targetUnit:
    case orderType_targetSensorReturns:
      {
        uint16 weaponIndex;
        weaponIndex = order.getTargetWeaponData().getWeaponIndex();
        if (weaponIndex >= weaponOrders.size()) {
          SAKUSEN_DEBUG("Weapon index out of range");
          break;
        }
        weaponOrders[weaponIndex].update(order);
      }
      break;
    default:
      SAKUSEN_FATAL("Unknown OrderType " << order.getType());
  }
}

void UnitOrders::store(OArchive& out) const
{
  (out.insertEnum(linearTarget) << targetPosition <<
    targetVelocity).insertEnum(rotationalTarget);
  targetOrientation.store(out);
  out << targetAngularVelocity << weaponOrders;
}

UnitOrders UnitOrders::load(IArchive& in, const DeserializationContext& context)
{
  /* current goals which have been set by orders */
  LinearTargetType linearTarget;
  Point<sint32> targetPosition;
  Point<sint16> targetVelocity;
  RotationalTargetType rotationalTarget;
  Orientation targetOrientation;
  AngularVelocity targetAngularVelocity;
  vector<WeaponOrders> weaponOrders;

  in.extractEnum(linearTarget);
  in >> targetPosition >> targetVelocity;
  in.extractEnum(rotationalTarget);
  targetOrientation = Orientation::load(in);
  in >> targetAngularVelocity;
  in.extract(weaponOrders, context);

  return UnitOrders(
      linearTarget, targetPosition, targetVelocity,
      rotationalTarget, targetOrientation, targetAngularVelocity, weaponOrders
    );
}

