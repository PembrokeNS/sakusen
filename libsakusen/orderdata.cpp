#include "orderdata.h"
#include "libsakusen-global.h"

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

void TargetSensorReturnsOrderData::store(OArchive& out) const
{
  out << weaponIndex;
  target.store(out);
}

void TargetPointOrderData::store(OArchive& out) const
{
  out << weaponIndex << target;
}

