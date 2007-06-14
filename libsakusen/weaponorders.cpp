#include "weaponorders.h"

#include "region-methods.h"

using namespace sakusen;

WeaponOrders::WeaponOrders() :
  targetType(weaponTargetType_none),
  targetPosition(),
  targetSensorReturns()
{
}

WeaponOrders::WeaponOrders(
    WeaponTargetType tT,
    const Point<sint32>& tP,
    const Orientation& tO,
    const Ref<ICompleteUnit>& tU,
    const Ref<ISensorReturns>& tSR
  ) :
  targetType(tT),
  targetPosition(tP),
  targetOrientation(tO),
  targetUnit(tU),
  targetSensorReturns(tSR)
{
}

bool WeaponOrders::isTargetValid() const
{
  switch (targetType) {
    case weaponTargetType_none:
      return false;
    case weaponTargetType_position:
    case weaponTargetType_positionOrientation:
      return true;
    case weaponTargetType_unit:
      return targetUnit.isValid();
    case weaponTargetType_sensorReturns:
      return targetSensorReturns.isValid();
    default:
      Fatal("unexpected WeaponTargetType: " << targetType);
  }
}

/** \brief Turn the target of the Weapon (whatever it may be) into an estimated
 * position
 *
 * \warning This method assumes the target is valid.  You should call
 * isTargetValid first to check that.
 */
Point<sint32> WeaponOrders::getTargetPosition() const
{
  switch (targetType) {
    case weaponTargetType_none:
      Fatal("no target to get position of");
    case weaponTargetType_position:
    case weaponTargetType_positionOrientation:
      return targetPosition;
    case weaponTargetType_unit:
      return targetUnit->getIStatus()->getPosition();
    case weaponTargetType_sensorReturns:
      if (0 != (targetSensorReturns->getPerception() | perception_unit)) {
        return targetSensorReturns->getUnit()->getIStatus()->getPosition();
      } else {
        return targetSensorReturns->getRegion()->getBestPosition();
      }
    default:
      Fatal("unexpected WeaponTargetType: " << targetType);
  }
}

/** \brief Turn the target of the Weapon (whatever it may be) into an estimated
 * velocity
 *
 * \warning This method assumes the target is valid.  You should call
 * isTargetValid first to check that.
 */
Point<sint16> WeaponOrders::getTargetVelocity() const
{
  switch (targetType) {
    case weaponTargetType_none:
      Fatal("no target to get velocity of");
    case weaponTargetType_position:
    case weaponTargetType_positionOrientation:
      return Point<sint16>();
    case weaponTargetType_unit:
      return targetUnit->getIStatus()->getVelocity();
    case weaponTargetType_sensorReturns:
      if (0 != (targetSensorReturns->getPerception() | perception_unit)) {
        return targetSensorReturns->getUnit()->getIStatus()->getVelocity();
      } else {
        return Point<sint16>();
      }
    default:
      Fatal("unexpected WeaponTargetType: " << targetType);
  }
}

/** \brief Clear the orders
 *
 * Sets targetType to none.
 */
void WeaponOrders::clear()
{
  targetType = weaponTargetType_none;
}

void WeaponOrders::update(const Order& order)
{
  switch (order.getType()) {
    case orderType_move:
    case orderType_setVelocity:
      Fatal("Order type not appropriate for a Weapon");
    case orderType_targetPosition:
      targetType = weaponTargetType_position;
      targetPosition = order.getTargetPositionData().getTarget();
      break;
    case orderType_targetPositionOrientation:
      targetType = weaponTargetType_positionOrientation;
      boost::tie(targetPosition, targetOrientation) =
        order.getTargetPositionOrientationData().getTarget();
      break;
    case orderType_targetUnit:
      targetType = weaponTargetType_unit;
      targetUnit = order.getTargetUnitData().getTarget();
      break;
    case orderType_targetSensorReturns:
      targetType = weaponTargetType_sensorReturns;
      targetSensorReturns = order.getTargetSensorReturnsData().getTarget();
      break;
    default:
      Fatal("unknown OrderType " << order.getType());
  }
}

void WeaponOrders::store(OArchive& archive) const
{
  archive.insertEnum(targetType) << targetPosition;
  targetOrientation.store(archive);
  targetUnit.store(archive);
  targetSensorReturns.store(archive);
}

WeaponOrders WeaponOrders::load(
    IArchive& archive,
    const DeserializationContext& context
  )
{
  WeaponTargetType targetType;
  Point<sint32> targetPosition;
  Orientation targetOrientation;
  Ref<ICompleteUnit> targetUnit;
  Ref<ISensorReturns> targetSensorReturns;

  archive.extractEnum(targetType) >> targetPosition;
  targetOrientation = Orientation::load(archive);
  targetUnit = Ref<ICompleteUnit>::load(archive, context);
  targetSensorReturns = Ref<ISensorReturns>::load(archive, context);
  
  return WeaponOrders(
      targetType, targetPosition, targetOrientation, targetUnit,
      targetSensorReturns
    );
}

