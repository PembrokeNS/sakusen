#include "sensorreturns.h"

#include "oarchive-methods.h"
#include "world.h"
#include "region-methods.h"

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
    unit->store(out);
  }
  out << returns;
}

SensorReturns SensorReturns::load(IArchive& in, const Universe* universe)
{
  SensorReturnsID id;
  Perception perception;
  PlayerID senserOwner;
  PlayerID senseeOwner;
  Region<sint32>* region;
  CompleteUnit* unit;
  SensorReturnMap returns(10);

  (in >> id).extractEnum(perception) >> senserOwner;

  if (0 != (perception & ~perception_full)) {
    throw EnumDeserializationExn("perception", perception);
  }
  
  if (0 != (perception & perception_owner)) {
    in >> senseeOwner;
  } else {
    senseeOwner = 0;
  }
  /** \todo Less copying of the Region and UnitStatus would be good here, but
   * entails some messiness of one kind or another */
  if (0 != (perception & perception_region)) {
    region = new Region<sint32>(Region<sint32>::load(in));
  } else {
    region = NULL;
  }
  if (0 != (perception & perception_unit)) {
    unit = new CompleteUnit(CompleteUnit::load(in, universe));
  } else {
    unit = NULL;
  }
  in >> returns;

  return SensorReturns(
      id, perception, senserOwner, senseeOwner, region, unit, returns
    );
}

