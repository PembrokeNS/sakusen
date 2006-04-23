#ifndef EACHSENSORRETURN_H
#define EACHSENSORRETURN_H

#include "sensorreturn.h"

namespace sakusen {

struct EachSensorReturn {
  EachSensorReturn() {}
  EachSensorReturn(
      const SensorReturn& o,
      const SensorReturn& iR,
      const SensorReturn& rP,
      const SensorReturn& rA,
      const SensorReturn& sP,
      const SensorReturn& sA,
      const SensorReturn& s
    ) :
    optical(o),
    infraRed(iR),
    radarPassive(rP),
    radarActive(rA),
    sonarPassive(sP),
    sonarActive(sA),
    seismar(s)
  {}
  
  SensorReturn optical;
  SensorReturn infraRed;
  SensorReturn radarPassive;
  SensorReturn radarActive;
  SensorReturn sonarPassive;
  SensorReturn sonarActive;
  SensorReturn seismar;

  inline void clear() {
    optical.clear();
    infraRed.clear();
    radarPassive.clear();
    radarActive.clear();
    sonarPassive.clear();
    sonarActive.clear();
    seismar.clear();
  }

  void store(OArchive&) const;
  static EachSensorReturn load(IArchive&);
};

}

#endif // EACHSENSORRETURN_H

