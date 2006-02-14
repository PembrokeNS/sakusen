#ifndef SENSORRETURN_H
#define SENSORRETURN_H

#include <vector>
#include "visibility.h"
#include "libsakusen-global.h"
#include "unitid.h"

namespace sakusen {
namespace server {

typedef uint32 SensorReturnID;

class UnitSensorPair {
  public:
    UnitSensorPair();
    ~UnitSensorPair();
  private:
    UnitID spotter;
    enum sensorType method;
};
  
class SensorReturn {
  public:
    SensorReturn();
    ~SensorReturn();
  private:
    SensorReturnID srId;
    std::vector<class UnitSensorPair> returns;
  public:
    inline SensorReturnID getId(void) {return srId;}
};

}}

#endif

