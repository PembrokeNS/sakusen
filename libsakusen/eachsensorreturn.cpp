#include "eachsensorreturn.h"

using namespace sakusen;

void EachSensorReturn::store(OArchive& out) const
{
  optical.store(out);
  infraRed.store(out);
  radarPassive.store(out);
  radarActive.store(out);
  sonarPassive.store(out);
  sonarActive.store(out);
  seismar.store(out);
}

EachSensorReturn EachSensorReturn::load(IArchive& in)
{
  SensorReturn optical(SensorReturn::load(in));
  SensorReturn infraRed(SensorReturn::load(in));
  SensorReturn radarPassive(SensorReturn::load(in));
  SensorReturn radarActive(SensorReturn::load(in));
  SensorReturn sonarPassive(SensorReturn::load(in));
  SensorReturn sonarActive(SensorReturn::load(in));
  SensorReturn seismar(SensorReturn::load(in));

  return EachSensorReturn(
      optical, infraRed, radarPassive, radarActive, sonarPassive, sonarActive,
      seismar
    );
}

