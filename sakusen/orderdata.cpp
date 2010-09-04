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

void SetVelocityOrderData::store(OArchive& out) const
{
  out << target;
}

void MoveOrderData::store(OArchive& out) const
{
  out << target;
}

void SetAngularVelocityOrderData::store(OArchive& out) const
{
  out << target;
}

void OrientOrderData::store(OArchive& out) const
{
  target.store(out);
}

void TargetNoneOrderData::store(OArchive& out) const
{
  out << weaponIndex;
}

void TargetNumberOrderData::store(OArchive& out) const
{
  out << weaponIndex << target;
}

void TargetPositionOrderData::store(OArchive& out) const
{
  out << weaponIndex << target;
}

void TargetFrameOrderData::store(OArchive& out) const
{
  target.store(out);
  out << weaponIndex;
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

