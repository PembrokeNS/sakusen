#include "updatedata.h"

using namespace sakusen;

UnitRemovedUpdateData::UnitRemovedUpdateData(
    uint32 i,
    changeOwnerReason r) :
  UpdateData(),
  id(i),
  reason(r)
{
}

UnitRemovedUpdateData::UnitRemovedUpdateData(IArchive& in) :
  UpdateData()
{
  in >> id;
  in.extractEnum(reason);
}

void UnitRemovedUpdateData::store(OArchive& out) const
{
  (out << id).insertEnum(reason);
}

UnitAddedUpdateData::UnitAddedUpdateData(
    changeOwnerReason r,
    const ICompleteUnit* u
  ) :
  UpdateData(),
  reason(r),
  unit(u)
{
}

UnitAddedUpdateData::UnitAddedUpdateData(
    IArchive& in,
    const Universe* universe
  ) :
  unit(CompleteUnit::load(in, universe))
{
  in.extractEnum(reason);
}

void UnitAddedUpdateData::store(OArchive& out) const
{
  unit.store(out);
  out.insertEnum(reason);
}

UnitAlteredUpdateData::UnitAlteredUpdateData(const ICompleteUnit* u) :
  UpdateData(),
  unit(u)
{
}

UnitAlteredUpdateData::UnitAlteredUpdateData(
    IArchive& in,
    const Universe* universe
  ) :
  unit(CompleteUnit::load(in, universe))
{
}

void UnitAlteredUpdateData::store(OArchive& out) const
{
  unit.store(out);
}

OrderAcceptedUpdateData::OrderAcceptedUpdateData(
    uint32 u,
    OrderCondition c) :
  UpdateData(),
  unitId(u),
  condition(c)
{
}

OrderAcceptedUpdateData::OrderAcceptedUpdateData(IArchive& in)
{
  in >> unitId;
  in.extractEnum(condition);
}

void OrderAcceptedUpdateData::store(OArchive& out) const
{
  (out << unitId).insertEnum(condition);
}

OrderCompletedUpdateData::OrderCompletedUpdateData(
    uint32 u,
    OrderCondition c) :
  UpdateData(),
  unitId(u),
  condition(c)
{
}

OrderCompletedUpdateData::OrderCompletedUpdateData(IArchive& in)
{
  in >> unitId;
  in.extractEnum(condition);
}

void OrderCompletedUpdateData::store(OArchive& out) const
{
  (out << unitId).insertEnum(condition);
}

OrderQueuedUpdateData::OrderQueuedUpdateData(
    uint32 u,
    const Order& o,
    OrderCondition c) :
  UpdateData(),
  unitId(u),
  order(o),
  condition(c)
{
}

OrderQueuedUpdateData::OrderQueuedUpdateData(IArchive& in)
{
  in >> unitId;
  order = Order::load(in);
  in.extractEnum(condition);
}

void OrderQueuedUpdateData::store(OArchive& out) const
{
  out << unitId;
  order.store(out);
  out.insertEnum(condition);
}

SensorReturnsRemovedUpdateData::SensorReturnsRemovedUpdateData(
    SensorReturnsID i
  ) :
  UpdateData(),
  id(i)
{
}

SensorReturnsRemovedUpdateData::SensorReturnsRemovedUpdateData(IArchive& in) :
  UpdateData()
{
  in >> id;
}

void SensorReturnsRemovedUpdateData::store(OArchive& out) const
{
  out << id;
}

SensorReturnsAddedUpdateData::SensorReturnsAddedUpdateData(
    const ISensorReturns* r
  ) :
  UpdateData(),
  returns(r)
{
}

SensorReturnsAddedUpdateData::SensorReturnsAddedUpdateData(
    IArchive& in,
    const Universe* universe
  ) :
  UpdateData(),
  returns(SensorReturns::load(in, universe))
{
}

void SensorReturnsAddedUpdateData::store(OArchive& out) const
{
  returns.store(out);
}

SensorReturnsAlteredUpdateData::SensorReturnsAlteredUpdateData(
    const ISensorReturns* r
  ) :
  UpdateData(),
  returns(r)
{
}

SensorReturnsAlteredUpdateData::SensorReturnsAlteredUpdateData(
    IArchive& in,
    const Universe* universe
  ) :
  UpdateData(),
  returns(SensorReturns::load(in, universe))
{
}

void SensorReturnsAlteredUpdateData::store(OArchive& out) const
{
  returns.store(out);
}

