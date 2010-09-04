#ifndef LIBSAKUSEN__SENSORRETURNMAP_H
#define LIBSAKUSEN__SENSORRETURNMAP_H

#include <sakusen/global.h>
#include <sakusen/unitid.h>
#include <sakusen/eachsensorreturn.h>

namespace sakusen {

typedef u_map<UnitId, EachSensorReturn>::type SensorReturnMap;

}

#endif // LIBSAKUSEN__SENSORRETURNMAP_H

