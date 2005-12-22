#include "updatedata.h"

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
  out << updateType_unitRemoved;
  out << id;
  out << reason;
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
  out << updateType_orderAccepted;
  out << unitId;
  out << condition;
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
  out << updateType_orderCompleted;
  out << unitId;
  out << condition;
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

