#include <sakusen/sensorreturns.h>

#include <sakusen/oarchive-methods.h>
#include <sakusen/world.h>
#include <sakusen/region-methods.h>

using namespace sakusen;

void SensorReturns::store(OArchive& out) const
{
  (out << id).insertEnum(perception) << senserOwner;
  if (0 != (perception & perception_owner)) {
    out << senseeOwner;
  }
  if (0 != (perception & perception_region)) {
    region->store(out);
  }
  if (0 != (perception & perception_unit)) {
    unit->store(out, true /* sanitize to remove unitId */);
  }
  out << returns;
}

SensorReturns SensorReturns::load(
    IArchive& in,
    const DeserializationContext& context
  )
{
  SensorReturnsId id;
  Perception perception;
  PlayerId senserOwner;
  PlayerId senseeOwner;
  Region<sint32>::Ptr region;
  CompleteUnit* unit;
  SensorReturnMap returns(10);

  (in >> id).extractEnum(perception) >> senserOwner;

  if (0 != (perception & ~perception_full)) {
    throw EnumDeserializationExn("perception", perception);
  }
  
  if (0 != (perception & perception_owner)) {
    in >> senseeOwner;
  } else {
    senseeOwner = PlayerId();
  }
  if (0 != (perception & perception_region)) {
    region = Region<sint32>::loadNew(in);
  } else {
    region.reset();
  }
  if (0 != (perception & perception_unit)) {
    unit = new CompleteUnit(CompleteUnit::load(in, context));
  } else {
    unit = NULL;
  }
  in >> returns;

  return SensorReturns(
      id, perception, senserOwner, senseeOwner, region, unit, returns
    );
}

