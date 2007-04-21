#include "orderdata.h"
#include "libsakusen-global.h"
#include "oarchive-methods.h"

using namespace sakusen;

OrderData::OrderData()
{
}

OrderData::OrderData(const OrderData&)
{
}

void MoveOrderData::store(OArchive& out) const
{
  out << target;
}

void SetVelocityOrderData::store(OArchive& out) const
{
  out << target;
}

void TargetPositionOrderData::store(OArchive& out) const
{
  out << weaponIndex << target;
}

void TargetPositionOrientationOrderData::store(OArchive& out) const
{
  out << weaponIndex << target;
}

void TargetUnitOrderData::store(OArchive& out) const
{
  out << weaponIndex;
  target.store(out);
}

void TargetSensorReturnsOrderData::store(OArchive& out) const
{
  out << weaponIndex;
  target.store(out);
}

