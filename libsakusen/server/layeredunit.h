#ifndef LIBSAKUSEN_SERVER__LAYEREDUNIT_H
#define LIBSAKUSEN_SERVER__LAYEREDUNIT_H

#include "libsakusen-global.h"

#include <list>

#include "unit.h"
#include "iunit.h"
#include "unitlayer.h"
#include "unittemplate.h"

namespace sakusen {
namespace server {

/** \brief A Unit together with all layers currently attached to it.
 *
 * This is a servers representation of a Unit, with all the additional data
 * that only the server gets to see, such as the layers which have been added
 * to it, SensorReturns from it to various Players, etc.
 *
 * At present the direct method of implementing layers is used - LayeredUnit
 * contains a pointer to the top layer, which has a pointer to the next one
 * down and so forth until the UnitCore is reached at the centre.
 *
 * LayeredUnit also carries a direct pointer to the Unit which is at its heart.
 * This allows direct access to certain aspects of the Unit (the physical
 * properties - position, velocity, etc. and the order queue) so that these can
 * be handled directly.  It would be bad (and slow) for layers to mess with the
 * physics too much.
 */
class LIBSAKUSEN_API LayeredUnit : public IUnit {
  public:
    static void spawn(
      const PlayerID owner,
      const UnitTypeID type,
      const Point<sint32>& startNear,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity
    ); /* factory class */
    static void spawn(const PlayerID owner, const UnitTemplate& t);
      /* factory class */
  private:
    LayeredUnit(); /**< Default constructor should not be used */
    LayeredUnit(
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity,
      HitPoints startHitPoints,
      bool startRadarActive,
      bool startSonarActive
    ); /* constructor intended to be used when spawning a Unit on the map -
          this is called only by Unit::spawn, and is thus private.
          owner is expected to be set after construction since it should not
          be set until *after* the Unit has been added to the World */
    LayeredUnit(
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity
    ); /* This is a simpler version of the above constructor for when you don't
          need to specify the extra data */
  public:
    LayeredUnit(const LayeredUnit&); /**< Copy constructor needs to be
                                       different from the default */
/* Not written yet. */
    LayeredUnit& operator=(const LayeredUnit&);
    ~LayeredUnit();
  private:
    PlayerID owner;
    UnitLayer* topLayer;
    Unit* unit; /**< \brief Shortcut pointer to the Unit at the heart
                 *
                 * Not owned by this
                 */
    
    void acceptOrder(OrderCondition condition);
      /**< Accept a new order from the queue */
  public:
    /* accessors */
    inline UnitTypeID getType(void) const { return unit->getType(); }
    inline uint32 getId(void) const { return unit->getId(); }
    inline void setId(uint32 id) { unit->setId(id); }
    inline const Point<sint32>& getPosition(void) const {
      return unit->getPosition();
    }
    inline const Orientation& getOrientation(void) const {
      return unit->getOrientation();
    }
    inline const Point<sint16>& getVelocity(void) const {
      return unit->getVelocity();
    }
    inline HitPoints getHitPoints(void) const { return unit->getHitPoints(); }
    inline bool isRadarActive(void) const { return unit->isRadarActive(); }
    inline bool isSonarActive(void) const { return unit->isSonarActive(); }
    
    void setPosition(const Point<sint32>& pos);
    bool setRadar(bool active);
    bool setSonar(bool active);

    inline const Order& getOrder(OrderCondition c) const {
      return unit->getOrder(c);
    }
    inline const Order& getCurrentOrder(void) const {
      return unit->getCurrentOrder();
    }
    inline LinearTargetType getLinearTarget(void) const {
      return unit->getLinearTarget();
    }
    inline const Point<sint32>& getTargetPosition(void) const {
      return unit->getTargetPosition();
    }
    inline const Point<sint16>& getTargetVelocity(void) const {
      return unit->getTargetVelocity();
    }
    inline RotationalTargetType getRotationalTarget(void) const {
      return unit->getRotationalTarget();
    }
    inline const Orientation& getTargetOrientation(void) const {
      return unit->getTargetOrientation();
    }
    inline const AngularVelocity& getTargetAngularVelocity(void) const {
      return unit->getTargetAngularVelocity();
    }

    inline PlayerID getOwner(void) const {return owner;}
    inline HitPoints getMaxHitPoints(void) const {
      return topLayer->getMaxHitPoints();
    }
    inline uint8 getMass(void) const {return topLayer->getMass();}
    inline const Point<uint32>& getSize(void) const {
      return topLayer->getSize();
    }
    inline const Region<sint16>& getPossibleAccelerations(void) const {
      return topLayer->getPossibleAccelerations();
    }
    inline const Region<sint16>& getPossibleVelocities(void) const {
      return topLayer->getPossibleVelocities();
    }
    inline const Region<sint16>& getPossibleAngularVelocities(void) const {
      return topLayer->getPossibleAngularVelocities();
    }
    inline const Visibility& getVisibility(void) const {
      return topLayer->getVisibility();
    }
    inline const Sensors& getVision(void) const {return topLayer->getVision();}
    
    /* game mechanics methods */
    void setPhysics(
        const Point<sint32>& newPosition,
        const Orientation& newOrientation,
        bool orientationIsRelative,
        bool zeroVelocity
      );
    void incrementState(const Time& timeNow);
      /* advances the state of the unit one unit of game time (changing its
			   velocity, position, applying all actions for the given time) */
    void enqueueOrder(const OrderCondition& condition, const Order& order);
    
    inline void kill(HitPoints excessDamage) { topLayer->kill(excessDamage); }
      /**< kills the unit unconditionally */
    inline void damage(HitPoints amount) { topLayer->damage(amount); }
      /**< damages the unit and kills if HP <= 0 */
    inline void repair(HitPoints amount, bool superhealth) {
      topLayer->repair(amount, superhealth);
    } /**< repairs the unit up to its max HP, if superhealth is false; o/w,
       * repairs the unit up to no maximum */
    inline void changeType(
        const UnitTypeID& to,
        hitPointAlteration hpAlteration
      ) { topLayer->changeType(to, hpAlteration); }
      /**< the semantics of this are going to need some thought, but does
       * transforming of units and deployment */
    inline void changeOwner(const PlayerID to, enum changeOwnerReason why) {
      topLayer->changeOwner(to, why);
      owner = topLayer->getOwner();
    }
};

}}

#endif // LIBSAKUSEN_SERVER__LAYEREDUNIT_H

