#ifndef ISENSORRETURNS_H
#define ISENSORRETURNS_H

#include "libsakusen-global.h"
#include "gnu_extensions.h"
#include "visibility.h"
#include "playerid.h"
#include "region.h"
#include "rectangle.h"

namespace sakusen {

typedef uint32 SensorReturnsID;

typedef __gnu_cxx::hash_map<uint32, EachSensorReturn> SensorReturnMap;

class ISensorReturns {
  public:
    virtual ~ISensorReturns() {}

    virtual SensorReturnsID getId() const = 0;
    virtual Perception getPerception() const = 0;
    virtual PlayerID getOwner() const = 0;
    virtual const Region<sint32>* getRegion() const = 0;
    virtual const ICompleteUnit* getUnit() const = 0;
    virtual const SensorReturnMap& getSensorReturns() const = 0;

    virtual Point<sint32> getBestPosition() const;
    virtual Rectangle<sint32> getBoundingRectangle() const;
};

}

#endif // ISENSORRETURNS_H

