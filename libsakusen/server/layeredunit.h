#ifndef LIBSAKUSEN_SERVER__LAYEREDUNIT_H
#define LIBSAKUSEN_SERVER__LAYEREDUNIT_H

#include "libsakusen-global.h"

#include <list>

#include "icompleteunit.h"
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
class LIBSAKUSEN_API LayeredUnit : public ICompleteUnit {
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
    LayeredUnit(const UnitTemplate&);
      /**< Constructor used by LayeredUnit::spawn during initial construction
       * of the Map */
    LayeredUnit(
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity
    ); /* constructor intended to be used when spawning a Unit on the map -
          this is called only by Unit::spawn, and is thus private.
          owner is expected to be set after construction since it should not
          be set until *after* the Unit has been added to the World */
  public:
    LayeredUnit(const LayeredUnit&); /**< Copy constructor needs to be
                                       different from the default */
    LayeredUnit& operator=(const LayeredUnit&);
    ~LayeredUnit();
  private:
    PlayerID owner;
    uint32 unitId;
    UnitLayer* topLayer;
    UnitStatus* unit; /**< \brief Shortcut pointer to the Unit at the heart
                       *
                       * Not owned by this
                       */
    
    void acceptOrder(OrderCondition condition);
      /**< Accept a new order from the queue */
  public:
    /* accessors */
    inline PlayerID getOwner(void) const {return owner;}
    inline uint32 getId(void) const { return unitId; }
    inline void setId(uint32 id) { unitId = id; }
    inline const IUnitStatus* getIStatus(void) const { return unit; }
    /*inline IUnitStatus* getStatus(void) { return unit; }*/
    inline const IUnitTypeData* getITypeData(void) const { return topLayer; }
    
    void setPosition(const Point<sint32>& pos);
    bool setRadar(bool active);
    bool setSonar(bool active);
    
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
    inline void onDestruct(void) { topLayer->onDestruct(); }
};

}}

#endif // LIBSAKUSEN_SERVER__LAYEREDUNIT_H

