#ifndef LIBSAKUSEN__SENSORRETURNMAP_H
#define LIBSAKUSEN__SENSORRETURNMAP_H

#include "libsakusen-global.h"
#include "unitid.h"
#include "eachsensorreturn.h"

namespace sakusen {

typedef u_map<UnitId, EachSensorReturn>::type SensorReturnMap;

}

#endif // LIBSAKUSEN__SENSORRETURNMAP_H

