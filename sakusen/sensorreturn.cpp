#include <sakusen/sensorreturn.h>

using namespace sakusen;

void SensorReturn::store(OArchive& out) const
{
  out.insertEnum(perception);
}

SensorReturn SensorReturn::load(IArchive& in)
{
  Perception perception;

  in.extractEnum(perception);

  return SensorReturn(perception);
}

