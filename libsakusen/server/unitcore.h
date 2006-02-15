#ifndef UNITCORE_H
#define UNITCORE_H

#include "unitstatus.h"
#include "unitlayer.h"

namespace sakusen {
namespace server {

class LayeredUnit;

class UnitCore : public UnitLayer, private UnitStatus {
  private:
    UnitCore();
    UnitCore(const UnitCore&);
  public:
    UnitCore(const UnitCore&, LayeredUnit* outerUnit);
    UnitCore(
        LayeredUnit* outerUnit,
        const UnitTypeID& startType,
        const Point<sint32>& startPosition,
        const Orientation& startOrientation,
        const Point<sint16>& startVelocity
      );
    UnitCore(LayeredUnit* outerUnit, const UnitStatus& status);
  private:
    LayeredUnit* outerUnit;
    PlayerID owner;
  public:
    inline UnitLayer* newCopy(LayeredUnit* outer) const {
      return new UnitCore(*this, outer);
    }
    inline UnitStatus* getCore() { return this; }
    
    /* accessors */
    inline PlayerID getOwner() const { return owner; }
    inline HitPoints getMaxHitPoints() const {
      return getTypePtr()->getMaxHitPoints();
    }
    inline uint8 getMass(void) const { return getTypePtr()->getMass(); }
    inline const Point<uint32>& getSize(void) const {
      return getTypePtr()->getSize();
    }
    inline const Region<sint16>& getPossibleAccelerations(void) const {
      return getTypePtr()->getPossibleAccelerations();
    }
    inline const Region<sint16>& getPossibleVelocities(void) const {
      return getTypePtr()->getPossibleVelocities();
    }
    inline const Region<sint16>& getPossibleAngularVelocities(void) const {
      return getTypePtr()->getPossibleAngularVelocities();
    }
    inline const Visibility& getVisibility(void) const {
      return getTypePtr()->getVisibility();
    }
    inline const Sensors& getVision(void) const {
      return getTypePtr()->getVision();
    }

    /* game mechanics */
    void kill(HitPoints excessDamage);
    void damage(HitPoints amount);
    void repair(HitPoints amount, bool superhealth);
    void changeType(const UnitTypeID& to, hitPointAlteration hpAlteration);
    void changeOwner(const PlayerID to, enum changeOwnerReason why);
    
    /* callbacks */
    virtual void onCreate(void) {}
    virtual void onDestruct(void) { /* TODO: what should the default be? */ }
};

}}

#endif // UNITCORE_H
