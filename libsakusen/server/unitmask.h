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
    UnitMask(const UnitMask& copy, LayeredUnit* outer) :
      UnitLayer(copy),
      nextLayer(copy.nextLayer->newCopy(outer))
    {}
  private:
    UnitLayer::Ptr nextLayer;
  public:
    HitPoints getMaxHitPoints() const { return nextLayer->getMaxHitPoints(); }
    uint8 getMass() const { return nextLayer->getMass(); }
    const Point<uint32>& getSize() const { return nextLayer->getSize(); }
    const Region<sint16>::Ptr& getPossibleAccelerations() const {
      return nextLayer->getPossibleAccelerations();
    }
    const Region<sint16>::Ptr& getPossibleVelocities() const {
      return nextLayer->getPossibleVelocities();
    }
    const Region<sint16>::Ptr& getPossibleAngularVelocities() const {
      return nextLayer->getPossibleAngularVelocities();
    }
    const Visibility& getVisibility() const {
      return nextLayer->getVisibility();
    }
    const Sensors& getVision() const { return nextLayer->getVision(); }

    UnitCore* getCore() { return nextLayer->getCore(); }
    Ref<const LayeredUnit> getOuterUnit() const {
      return nextLayer->getOuterUnit();
    }
    PlayerID getOwner() const { return nextLayer->getOwner(); }

    void incrementWeaponsState() { nextLayer->incrementWeaponsState(); }
    bool kill(HitPoints excessDamage) { return nextLayer->kill(excessDamage); }
    void damage(HitPoints amount) { nextLayer->damage(amount); }
    void repair(HitPoints amount, bool superHealth) {
      return nextLayer->repair(amount, superHealth);
    }
    void changeType(
        const UnitTypeID& to,
        hitPointAlteration hpAlteration
      ) {
      nextLayer->changeType(to, hpAlteration);
    }
    void changeOwner(const PlayerID to, changeOwnerReason why) {
      nextLayer->changeOwner(to, why);
    }

    void onCreate() { nextLayer->onCreate(); }
    void onDestruct() { nextLayer->onDestruct(); }
};

}}

#endif // UNITMASK_H

