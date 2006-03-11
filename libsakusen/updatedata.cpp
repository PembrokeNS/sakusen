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
  out << id << uint8(reason);
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
  out << uint8(reason);
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
  out << unitId << uint8(condition);
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
  out << unitId << uint8(condition);
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
  out << uint8(condition);
}

