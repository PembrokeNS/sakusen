#include "weaponorders.h"

using namespace sakusen;

WeaponOrders::WeaponOrders() :
  targetType(weaponTargetType_none),
  targetSensorReturns()
{
}

WeaponOrders::WeaponOrders(
    WeaponTargetType tT,
    const Ref<ISensorReturns>& tSR
  ) :
  targetType(tT),
  targetSensorReturns(tSR)
{
}

void WeaponOrders::update(const Order& order)
{
  switch (order.getType()) {
    case orderType_move:
    case orderType_setVelocity:
      Fatal("Order type not appropriate for a Weapon");
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
  archive.insertEnum(targetType);
  targetSensorReturns.store(archive);
}

WeaponOrders WeaponOrders::load(IArchive& archive, const PlayerID* player)
{
  WeaponTargetType targetType;
  Ref<ISensorReturns> targetSensorReturns;

  archive.extractEnum(targetType);
  targetSensorReturns = Ref<ISensorReturns>::load(archive, *player);
  
  return WeaponOrders(targetType, targetSensorReturns);
}

