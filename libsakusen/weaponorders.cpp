#include "weaponorders.h"

using namespace sakusen;

void WeaponOrders::update(const Order& order)
{
  switch (order.getType()) {
    case orderType_move:
    case orderType_setVelocity:
      Fatal("Order type not appropriate for a Weapon");
    case orderType_targetSensorReturns:
      /** \todo This bit */
      break;
    default:
      Fatal("unknown OrderType " << order.getType());
  }
}

void WeaponOrders::store(OArchive& /*archive*/) const
{
}

WeaponOrders WeaponOrders::load(IArchive& /*archive*/)
{
  return WeaponOrders();
}

