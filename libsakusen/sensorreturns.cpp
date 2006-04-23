#include "sensorreturns.h"

#include "oarchive-methods.h"
#include "world.h"
#include "region-methods.h"

using namespace sakusen;

void SensorReturns::store(OArchive& out) const
{
  (out << id).insertEnum(perception);
  if (0 != (perception & perception_owner)) {
    out << owner;
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
  PlayerID owner;
  Region<sint32>* region;
  CompleteUnit* unit;
  SensorReturnMap returns;

  (in >> id).extractEnum(perception);

  if (0 != (perception & ~perception_full)) {
    throw new EnumDeserializationExn("perception", perception);
  }
  
  if (0 != (perception & perception_owner)) {
    in >> owner;
  } else {
    owner = 0;
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

  return SensorReturns(id, perception, owner, region, unit, returns);
}

