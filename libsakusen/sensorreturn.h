#ifndef SENSORRETURN_H
#define SENSORRETURN_H

#include "libsakusen-global.h"
#include "perception.h"
#include "oarchive.h"
#include "iarchive.h"

namespace sakusen {

struct SensorReturn {
  public:
    SensorReturn() :
      perception(perception_none)
    {}
    SensorReturn(Perception p) :
      perception(p)
    {}
    
    Perception perception;

    inline void clear() {
      perception = perception_none;
    }
    
    void store(OArchive&) const;
    static SensorReturn load(IArchive&);
};

}

#endif

