#ifndef DYNAMICSENSORRETURNS_H
#define DYNAMICSENSORRETURNS_H

#include "libsakusen-global.h"
#include <list>
#include "visibility.h"
#include "isensorreturns.h"
#include "serializationhandler.h"

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
        const Ref<const LayeredUnit>& sensee,
        Player* senserOwner
      );
    ~DynamicSensorReturns() {}
  private:
    SensorReturnsID id;
    Perception perception;
    Region<sint32>::ConstPtr region;
    Player* senserOwner;
    SensorReturnMap sensers;
    Ref<const LayeredUnit> sensee;
    /** \brief When set, indicates that there have been changes to this that
     * have yet to be sent to the sensorOwner's clients. */
    bool dirty;
  public:
    /** \name Accessors. */
    //@{
    inline SensorReturnsID getId() const { return id; }
    inline Perception getPerception() const { return perception; }
    PlayerID getSenserOwner() const;
    PlayerID getSenseeOwner() const;
    Region<sint32>::ConstPtr getRegion() const;
    Ref<const ICompleteUnit> getUnit() const;
    inline const SensorReturnMap& getSensorReturns() const { return sensers; }
    inline bool empty() const { return sensers.empty(); }
    //@}
    inline bool isDirty() const { return dirty; }
    inline void setDirty() { dirty = true; }
    inline void clearDirty() { dirty = false; }
    void senseeDestroyed();
    void update();
};

typedef __gnu_cxx::hash_map<
    SensorReturnsID,
    Ref<DynamicSensorReturns>
  >::iterator DynamicSensorReturnsRef;

}}

#endif // DYNAMICSENSORRETURNS_H

