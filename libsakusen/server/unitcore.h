#ifndef UNITCORE_H
#define UNITCORE_H

#include "unitstatus.h"
#include "unitlayer.h"
#include "weapon.h"

namespace sakusen {
namespace server {

class LayeredUnit;

/** \brief The lowest UnitLayer in a LayeredUnit.
 *
 * See \ref units for more details on how the various Unit classes fit together
 * */
class UnitCore : public UnitLayer, public UnitStatus {
  public:
    typedef boost::shared_ptr<UnitCore> Ptr;
    typedef boost::shared_ptr<const UnitCore> ConstPtr;
  private:
    UnitCore(const UnitCore&);
  public:
    UnitCore(const UnitCore&, LayeredUnit* outerUnit);
    UnitCore(
        LayeredUnit* outerUnit,
        const UnitTypeId& startType,
        const Point<sint32>& startPosition,
        const Orientation& startOrientation,
        const Point<sint16>& startVelocity,
        const HitPoints startHP
      );
    UnitCore(LayeredUnit* outerUnit, const UnitStatus& status);
    ~UnitCore();
  private:
    LayeredUnit* outerUnit;
    PlayerId owner;
    std::vector<Weapon*> weapons;

    void initializeWeapons();
  public:
    inline UnitLayer::Ptr newCopy(LayeredUnit* outer) const {
      return UnitLayer::Ptr(new UnitCore(*this, outer));
    }
    inline UnitCore* getCore() { return this; }
    inline UnitLayer::Ptr getLayer(const std::type_info&) {
      return UnitLayer::Ptr();
    }
    inline void removeLayer(const UnitMask*) {
      Fatal("tried to remove non-existant layer");
    }
    inline Ref<LayeredUnit> getOuterUnit() {
      return outerUnit->getRefToThis();
    }
    inline Ref<const LayeredUnit> getOuterUnit() const {
      return outerUnit->getRefToThis();
    }
    
    /** \name Accessors. */
    //@{
    inline PlayerId getOwner() const { return owner; }
    inline HitPoints getMaxHitPoints() const {
      return getTypePtr()->getDynamicData().getMaxHitPoints();
    }
    inline uint8 getMass(void) const {
      return getTypePtr()->getDynamicData().getMass();
    }
    inline const Point<uint32>& getSize(void) const {
      return getTypePtr()->getDynamicData().getSize();
    }
    inline Region<sint16>::ConstPtr getPossibleAccelerations(void) const {
      return getTypePtr()->getDynamicData().getPossibleAccelerations();
    }
    inline Region<sint16>::ConstPtr getPossibleVelocities(void) const {
      return getTypePtr()->getDynamicData().getPossibleVelocities();
    }
    inline Region<sint16>::ConstPtr getPossibleAngularVelocities(void) const {
      return getTypePtr()->getDynamicData().getPossibleAngularVelocities();
    }
    inline const Visibility& getVisibility(void) const {
      return getTypePtr()->getDynamicData().getVisibility();
    }
    inline const Sensors& getVision(void) const {
      return getTypePtr()->getDynamicData().getVision();
    }
    //@}

    /** \name Game mechanics.
     *
     * See similarly named methods in LayeredUnit for more details. */
    //@{
    void incrementWeaponsState();
    bool kill(HitPoints excessDamage);
    void damage(HitPoints amount);
    void repair(HitPoints amount, bool superhealth);
    void changeType(const UnitTypeId& to, hitPointAlteration hpAlteration);
    void changeOwner(const PlayerId to, enum changeOwnerReason why);
    //@}
    
    /* callbacks */
    void onCreate() {}
    void onDestruct() { /** \todo what should the default be? */ }
    void onRemoval() {}
};

}}

#endif // UNITCORE_H

