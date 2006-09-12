#include "orderdata.h"
#include "libsakusen-global.h"

using namespace sakusen;

OrderData::OrderData()
{
}

OrderData::OrderData(const OrderData&)
{
}

OrderData* MoveOrderData::newCopy(void) const
{
  return new MoveOrderData(*this);
}

void MoveOrderData::store(OArchive& out) const
{
  out << target;
}

OrderData* SetVelocityOrderData::newCopy(void) const
{
  return new SetVelocityOrderData(*this);
}

void SetVelocityOrderData::store(OArchive& out) const
{
  out << target;
}

OrderData* TargetSensorReturnsOrderData::newCopy(void) const
{
  return new TargetSensorReturnsOrderData(*this);
}

void TargetSensorReturnsOrderData::store(OArchive& out) const
{
  out << weaponIndex;
  target.store(out);
}

OrderData* TargetPointOrderData::newCopy(void) const
{
  return new TargetPointOrderData(*this);
}

void TargetPointOrderData::store(OArchive& out) const
{
  out << weaponIndex << target;
}

