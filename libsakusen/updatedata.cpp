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

void UnitRemovedUpdateData::store(OArchive& out) const
{
  out << uint8(updateType_unitRemoved) << id << uint8(reason);
}

UnitAddedUpdateData::UnitAddedUpdateData(
    const Unit* u,
    changeOwnerReason r) :
  UpdateData(),
  unit(u),
  reason(r)
{
}

void UnitAddedUpdateData::store(OArchive& /*out*/) const
{
  /* TODO: store */
}

OrderAcceptedUpdateData::OrderAcceptedUpdateData(
    uint32 u,
    OrderCondition c) :
  UpdateData(),
  unitId(u),
  condition(c)
{
}

void OrderAcceptedUpdateData::store(OArchive& out) const
{
  out << uint8(updateType_orderAccepted) << unitId << uint8(condition);
}

OrderCompletedUpdateData::OrderCompletedUpdateData(
    uint32 u,
    OrderCondition c) :
  UpdateData(),
  unitId(u),
  condition(c)
{
}

void OrderCompletedUpdateData::store(OArchive& out) const
{
  out << uint8(updateType_orderCompleted) << unitId << uint8(condition);
}

OrderQueuedUpdateData::OrderQueuedUpdateData(
    uint32 u,
    const Order* o,
    OrderCondition c) :
  UpdateData(),
  unitId(u),
  order(o),
  condition(c)
{
}

void OrderQueuedUpdateData::store(OArchive& /*out*/) const
{
  /* TODO: store */
}

