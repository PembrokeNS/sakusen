#include "weaponorders.h"

#include "region-methods.h"

using namespace sakusen;

WeaponOrders::WeaponOrders() :
  targetType(weaponTargetType_none),
  targetPoint(),
  targetSensorReturns()
{
}

WeaponOrders::WeaponOrders(
    WeaponTargetType tT,
    const Point<sint32>& tP,
    const Ref<ISensorReturns>& tSR
  ) :
  targetType(tT),
  targetPoint(tP),
  targetSensorReturns(tSR)
{
}

Point<sint32> WeaponOrders::getTargetPosition() const
{
  switch (targetType) {
    case weaponTargetType_none:
      Fatal("no target to get position of");
    case weaponTargetType_point:
      return targetPoint;
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

Point<sint16> WeaponOrders::getTargetVelocity() const
{
  switch (targetType) {
    case weaponTargetType_none:
      Fatal("no target to get velocity of");
    case weaponTargetType_point:
      return Point<sint32>();
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

void WeaponOrders::update(const Order& order)
{
  switch (order.getType()) {
    case orderType_move:
    case orderType_setVelocity:
      Fatal("Order type not appropriate for a Weapon");
    case orderType_targetPoint:
      targetType = weaponTargetType_point;
      targetPoint = order.getTargetPointData().getTarget();
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
  archive.insertEnum(targetType) << targetPoint;
  targetSensorReturns.store(archive);
}

WeaponOrders WeaponOrders::load(IArchive& archive, const PlayerID* player)
{
  WeaponTargetType targetType;
  Point<sint32> targetPoint;
  Ref<ISensorReturns> targetSensorReturns;

  archive.extractEnum(targetType) >> targetPoint;
  targetSensorReturns = Ref<ISensorReturns>::load(archive, player);
  
  return WeaponOrders(targetType, targetPoint, targetSensorReturns);
}

