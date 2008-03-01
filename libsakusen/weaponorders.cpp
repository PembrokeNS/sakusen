#include "weaponorders.h"

#include "region-methods.h"

using namespace sakusen;

WeaponOrders::WeaponOrders() :
  targetType(weaponTargetType_none),
  targetPosition(),
  targetFrame(Position(), Orientation()),
  targetSensorReturns()
{
}

WeaponOrders::WeaponOrders(
    WeaponTargetType tT,
    const Point<sint32>& tP,
    const Frame& tF,
    const Ref<ICompleteUnit>& tU,
    const Ref<ISensorReturns>& tSR
  ) :
  targetType(tT),
  targetPosition(tP),
  targetFrame(tF),
  targetUnit(tU),
  targetSensorReturns(tSR)
{
}

bool WeaponOrders::isTargetValid() const
{
  /* To cause a compile error when a new enum value is added.  If you get this,
   * update all of the (several) switch statements in this file and then
   * adjust the value appropriately. */
  SAKUSEN_STATIC_ASSERT(weaponTargetType_max == 6);

  switch (targetType) {
    case weaponTargetType_none:
      return false;
    case weaponTargetType_position:
    case weaponTargetType_frame:
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
      return targetPosition;
    case weaponTargetType_frame:
      return targetFrame.getPosition();
    case weaponTargetType_unit:
      return targetUnit->getIStatus().getPosition();
    case weaponTargetType_sensorReturns:
      if (0 != (targetSensorReturns->getPerception() | perception_unit)) {
        return targetSensorReturns->getUnit()->getIStatus().getPosition();
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
    case weaponTargetType_frame:
      return Point<sint16>();
    case weaponTargetType_unit:
      return targetUnit->getIStatus().getVelocity();
    case weaponTargetType_sensorReturns:
      if (0 != (targetSensorReturns->getPerception() | perception_unit)) {
        return targetSensorReturns->getUnit()->getIStatus().getVelocity();
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
  /* To remind that update here is needed by causing a compile error when a
   * new enum value is added. */
  SAKUSEN_STATIC_ASSERT(orderType_max == 9);

  switch (order.getType()) {
    case orderType_setVelocity:
    case orderType_move:
    case orderType_setAngularVelocity:
    case orderType_orient:
      Fatal("Order type not appropriate for a Weapon");
    case orderType_targetPosition:
      targetType = weaponTargetType_position;
      targetPosition = order.getTargetPositionData().getTarget();
      break;
    case orderType_targetFrame:
      targetType = weaponTargetType_frame;
      targetFrame = order.getTargetFrameData().getTarget();
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
  targetFrame.store(archive);
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
  Ref<ICompleteUnit> targetUnit;
  Ref<ISensorReturns> targetSensorReturns;

  archive.extractEnum(targetType) >> targetPosition;
  Frame targetFrame = Frame::load(archive);
  targetUnit = Ref<ICompleteUnit>::load(archive, context);
  targetSensorReturns = Ref<ISensorReturns>::load(archive, context);
  
  return WeaponOrders(
      targetType, targetPosition, targetFrame, targetUnit,
      targetSensorReturns
    );
}

