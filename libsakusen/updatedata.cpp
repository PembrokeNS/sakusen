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

OrderAcceptedUpdateData::OrderAcceptedUpdateData(IArchive& in)
{
  in >> unitId;
  in.extractEnum(condition);
}

void OrderAcceptedUpdateData::store(OArchive& out) const
{
  (out << unitId).insertEnum(condition);
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
    IArchive& in,
    const PlayerID* player
  ) :
  UpdateData()
{
  in >> unitId;
  order = Order::load(in, player);
  in.extractEnum(condition);
}

void OrderQueuedUpdateData::store(OArchive& out) const
{
  out << unitId;
  order.store(out);
  out.insertEnum(condition);
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

