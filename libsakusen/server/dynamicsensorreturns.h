#ifndef DYNAMICSENSORRETURNS_H
#define DYNAMICSENSORRETURNS_H

#include "libsakusen-global.h"
#include <list>
#include "visibility.h"
#include "isensorreturns.h"

namespace sakusen {
namespace server {

class Player;
class LayeredUnit;

class DynamicSensorReturns : public ISensorReturns {
  private:
    DynamicSensorReturns();
  public:
    DynamicSensorReturns(
        SensorReturnsID id,
        const LayeredUnit* sensee,
        Player* senserOwner
      );
    ~DynamicSensorReturns() {}
  private:
    SensorReturnsID id;
    Perception perception;
    Region<sint32>* region;
    Player* senserOwner;
    SensorReturnMap sensers;
    const LayeredUnit* sensee;
    bool dirty;
  public:
    inline SensorReturnsID getId() const { return id; }
    inline Perception getPerception() const { return perception; }
    PlayerID getOwner() const;
    const Region<sint32>* getRegion() const;
    const ICompleteUnit* getUnit() const;
    inline const SensorReturnMap& getSensorReturns() const { return sensers; }
    inline bool empty() const { return sensers.empty(); }
    inline bool isDirty() const { return dirty; }
    inline void clearDirty() { dirty = false; }
    void update();
};

typedef __gnu_cxx::hash_map<SensorReturnsID, DynamicSensorReturns>::iterator
  DynamicSensorReturnsRef;

}}

#endif // DYNAMICSENSORRETURNS_H

