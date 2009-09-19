#ifndef UNITMASK_H
#define UNITMASK_H

#include "unitlayer.h"
#include "unitcore.h"
#include "unitstatus-methods.h"

namespace sakusen {
namespace server {

/** \brief A UnitLayer which forms a mask, altering the unit in some way
 *
 * This class is intended to be the base of any UnitLayer other than UnitCore.
 * It implements all the methods of UnitLayer with versions that don't change
 * the unit at all; you must override those which you wish to do something
 * non-trivial.
 */
class UnitMask : public UnitLayer {
  friend class LayeredUnit;
  public:
    typedef boost::shared_ptr<UnitMask> Ptr;
    typedef boost::shared_ptr<const UnitMask> ConstPtr;
  protected:
    UnitMask() : UnitLayer() { /* nextLayer set later by LayeredUnit */ }

    UnitLayer::Ptr nextLayer;
  public:
    /** \name Accessors. */
    //@{
    HitPoints getMaxHitPoints() const { return nextLayer->getMaxHitPoints(); }
    uint8 getMass() const { return nextLayer->getMass(); }
    const Point<uint32>& getSize() const { return nextLayer->getSize(); }
    Region<sint16>::ConstPtr getPossibleAccelerations() const {
      return nextLayer->getPossibleAccelerations();
    }
    Region<sint16>::ConstPtr getPossibleVelocities() const {
      return nextLayer->getPossibleVelocities();
    }
    Region<sint32>::ConstPtr getPossibleAngularVelocities() const {
      return nextLayer->getPossibleAngularVelocities();
    }
    const Visibility& getVisibility() const {
      return nextLayer->getVisibility();
    }
    const Sensors& getVision() const { return nextLayer->getVision(); }
    //@}

    UnitCore& getCore() { return nextLayer->getCore(); }
    UnitLayer::Ptr getLayer(const std::type_info& typeInfo) {
      if (typeInfo == typeid(*nextLayer)) {
        return nextLayer;
      } else {
        return nextLayer->getLayer(typeInfo);
      }
    }
    void removeLayer(const UnitMask* layer) {
      if (nextLayer.get() == layer) {
        nextLayer = layer->nextLayer;
      } else {
        nextLayer->removeLayer(layer);
      }
    }
    Ref<LayeredUnit> getOuterUnit() {
      return nextLayer->getOuterUnit();
    }
    Ref<const LayeredUnit> getOuterUnit() const {
      return nextLayer->getOuterUnit();
    }
    PlayerId getOwner() const { return nextLayer->getOwner(); }
    void incrementState() { nextLayer->incrementState(); }

    /** \name Game mechanics.
     *
     * See similarly named methods in LayeredUnit for more details. */
    //@{
    void incrementWeaponsState() { nextLayer->incrementWeaponsState(); }
    bool kill(HitPoints excessDamage) { return nextLayer->kill(excessDamage); }
    void damage(HitPoints amount) { nextLayer->damage(amount); }
    void repair(HitPoints amount, bool superHealth) {
      return nextLayer->repair(amount, superHealth);
    }
    void changeType(
        const UnitTypeId& to,
        hitPointAlteration hpAlteration
      ) {
      nextLayer->changeType(to, hpAlteration);
    }
    void changeOwner(const PlayerId to, changeOwnerReason why) {
      nextLayer->changeOwner(to, why);
    }
    //@}

    void onCreate() { nextLayer->onCreate(); }
    void onDestruct() { nextLayer->onDestruct(); }
    void onRemoval() { nextLayer->onRemoval(); }
};

}}

#endif // UNITMASK_H

