#include "updatedata.h"

using namespace sakusen;

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
    IArchive& in,
    const DeserializationContext& context
  ) :
  unit(CompleteUnit::load(in, context))
{
  in.extractEnum(reason);
}

void UnitAddedUpdateData::store(OArchive& out) const
{
  unit.store(out);
  out.insertEnum(reason);
}

UnitAlteredUpdateData::UnitAlteredUpdateData(
    IArchive& in,
    const DeserializationContext& context
  ) :
  unit(CompleteUnit::load(in, context))
{
}

void UnitAlteredUpdateData::store(OArchive& out) const
{
  unit.store(out);
}

OrderAcceptedUpdateData::OrderAcceptedUpdateData(
    IArchive& in,
    const DeserializationContext& context
  )
{
  in >> unitId;
  order = Order::load(in, context);
}

void OrderAcceptedUpdateData::store(OArchive& out) const
{
  out << unitId;
  order.store(out);
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
    IArchive& in,
    const DeserializationContext& context
  ) :
  UpdateData(),
  returns(SensorReturns::load(in, context))
{
}

void SensorReturnsAddedUpdateData::store(OArchive& out) const
{
  returns.store(out);
}

SensorReturnsAlteredUpdateData::SensorReturnsAlteredUpdateData(
    IArchive& in,
    const DeserializationContext& context
  ) :
  UpdateData(),
  returns(SensorReturns::load(in, context))
{
}

void SensorReturnsAlteredUpdateData::store(OArchive& out) const
{
  returns.store(out);
}

BallisticRemovedUpdateData::BallisticRemovedUpdateData(IArchive& in) :
  UpdateData()
{
  in >> id;
}

void BallisticRemovedUpdateData::store(OArchive& out) const
{
  out << id;
}

BallisticAddedUpdateData::BallisticAddedUpdateData(IArchive& in) :
  UpdateData()
{
  in >> id;
  path  = Quadratic::load(in);
}

void BallisticAddedUpdateData::store(OArchive& out) const
{
  out << id;
  path.store(out);
}

