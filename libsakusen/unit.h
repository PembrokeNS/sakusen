#ifndef UNIT_H
#define UNIT_H

#include "libsakusen-global.h"

#include <list>
#include "changeownerreason.h"
#include "point.h"
#include "unittype.h"
#include "visibility.h"
#include "sensorreturn.h"
#include "orientation.h"
#include "order.h"
#include "ordercondition.h"
#include "universe.h"
#include "targettype.h"

namespace sakusen {

enum hitPointAlteration {
  fullHitPoints,
  scaleHitPoints,
  fixHitPoints
};

#ifdef NEED_TEMPLATE_INSTANTIATION
#pragma warning (disable: 4231)
EXPORT_LIST(Unit*)
#endif

class LIBSAKUSEN_API Unit {
  public:
    static void spawn(
      const PlayerID owner,
      const UnitTypeID type,
      const Point<sint32>& startNear,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity
    ); /* factory class */
  private:
    Unit(
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity
    ); /* constructor intended to be used when spawning a Unit on the map -
          this is called only by Unit::spawn, and is thus private */
  public:
    Unit(
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity,
      const Universe* universe
    ); /* constructor intended to be used when making Map object - at this
          stage no World exists */
    Unit();
    virtual ~Unit() {}
  private:
    PlayerID owner;
    UnitTypeID type;
    uint32 unitId;
    Point<sint32> position; /* this maybe needs a 'magic value' for when the unit is a subunit of another unit */
    Orientation orientation;
    Point<sint16> velocity; /* velocity is in distance-units per frame */
    
    /* modifiable stuff from UnitType */
    HitPoints maxHitPoints;
    uint8 mass;
    Point<uint32> size;
    Point<uint16> maxAcceleration;
    Point<uint16> maxMinusAcceleration;
    Point<sint16> maxSpeed; /* I'm confused as to why maxSpeed is a vector.  I'm
			       also confused as to why maxSpeed is signed and
			       maxAcceleration is not - JJB */
    Point<sint16> maxMinusSpeed;
    AngularSpeed maxYawSpeed;
    AngularSpeed maxPitchSpeed;
    Visibility visibility;
    Sensors vision;
    /* status stuff */
    HitPoints hitPoints;
    bool radarIsActive;
    bool sonarIsActive;
    std::list<Unit*> subunits; /* TODO: think hard about this.  What happens
				  when the subunits belong to another player?
				  What happens when this Unit is serialized? */
    std::list<Weapon> weapons;
    /* action queue */
    /* conditioned orders */
    Order orders[orderCondition_max];
    Order currentOrder; /* The last order given - the one which awaits its
			   success or failure */
    /* current goals which have been set by orders */
    LinearTargetType linearTarget; /* Whether we're currently trying to achieve a
				    position or a velocity */
    Point<sint32> targetPosition; /* The position we're trying to reach */
    Point<sint16> targetVelocity; /* The velocity we're trying to achieve */
    RotationalTargetType rotationalTarget;
    Orientation targetOrientation; /* The orientation we're trying to reach */
    AngularVelocity targetAngularVelocity; /* The angular velocity we're trying
					      to achieve */
    void initializeFromUnitType(
      const UnitType* typePtr,
      const Universe* universe
    );
      /* Initialize a new unit by taking all appropriate values from the given
       * UnitType */
    void acceptOrder(OrderCondition condition);
      /* Accept a new order from the queue */

  public:
    PLACEHOLDER_OPERATORS(Unit)

    /* accessors */
    inline PlayerID getOwner(void) const {return owner;}
    inline UnitTypeID getType(void) const {return type;}
    inline uint32 getId(void) const {return unitId;}
    inline void setId(uint32 id) {unitId = id;}
    inline const Point<sint32>& getPosition(void) const {return position;}
    void setPosition(const Point<sint32> pos);
    inline const Orientation& getOrientation(void) const {return orientation;}
    inline void setOrientation(const Orientation& o) { orientation = o; }
    inline const Point<sint16>& getVelocity(void) const {return velocity;}
    inline void setVelocity(const Point<sint16>& v) { velocity = v; }
    inline HitPoints getHitPoints(void) const {return hitPoints;}
    inline uint8 getMass(void) const {return mass;}
    inline const Point<uint32>& getSize(void) const {return size;}
    inline const Point<uint16>& getMaxAcceleration(void) const {return maxAcceleration;}
    inline const Point<uint16>& getMaxMinusAcceleration(void) const {return maxMinusAcceleration;}
    inline const Point<sint16>& getMaxSpeed(void) const {return maxSpeed;}
    inline const Point<sint16>& getMaxMinusSpeed(void) const {return maxMinusSpeed;}
    inline const Angle& getMaxYawSpeed(void) const {return maxYawSpeed;}
    inline const Angle& getMaxPitchSpeed(void) const {return maxPitchSpeed;}
    inline const Visibility& getVisibility(void) const {return visibility;}
    inline const Sensors& getVision(void) const {return vision;}
    inline bool isRadarActive(void) const {return radarIsActive;}
    inline bool isSonarActive(void) const {return sonarIsActive;}

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
    void processActions(const Time& timeNow); /* apply all actions up to the
						 given time */
    void enqueueOrder(const OrderCondition& condition, const Order& order);
    void kill(HitPoints excessDamage); /* kills the unit unconditionally */
    void damage(HitPoints amount); /* damages the unit and kills if HP <= 0 */
    void repair(HitPoints amount, bool superhealth);
      /* repairs the unit up to its max HP, if superhealth is false; o/w,
       * repairs the unit up to no maximum */
    void changeType(const UnitTypeID& to, hitPointAlteration hpAlteration);
      /* the semantics of this are going to need some thought, but does
       * transforming of units and deployment */
    void changeOwner(const PlayerID to, enum changeOwnerReason why);
      /* captured is true if the unit was captured, false if it was
       * given/reverted */

#if 0
    /* I don't think this is what we really want - we may want functions to
     * *target* a weapon here, but not fire it...
     */
    void fire(const Weapon* w, const Unit* target);
    void fire(const Weapon* w, const SensorReturn* target);
    void fire(const Weapon* w, const Point<sint32> direction, uint16 speed);
    void fire(const Weapon* w, const Point<sint32> target);
#endif
    bool setRadar(bool active);
    bool setSonar(bool active);

    /* callbacks */
    virtual void onCreate(void) {}
    virtual void onDestruct(void) { /* what should the default be? */ }
    
    typedef Universe loadArgument;
    void store(OArchive&) const;
    static Unit load(IArchive&, const loadArgument*);
};

#ifdef NEED_TEMPLATE_INSTANTIATION
#include <vector>
#pragma warning (disable: 4231)
EXPORT_VECTOR(Unit)
/* The following commented out because it generates an undisablable 4876 */
/* Note: The following is exporting a vector of vectors of units */
/*EXPORT_VECTOR(std::vector<Unit>)*/
#endif

}

#endif

