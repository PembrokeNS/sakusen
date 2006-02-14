#ifndef LIBSAKUSEN__UNIT_H
#define LIBSAKUSEN__UNIT_H

#include "libsakusen-global.h"

#include <list>
#include "point.h"
#include "unittype.h"
#include "visibility.h"
#include "orientation.h"
#include "order.h"
#include "ordercondition.h"
#include "universe.h"
#include "targettype.h"

namespace sakusen {

class LIBSAKUSEN_API Unit {
  private:
    Unit(); /**< Default constructor should not be used */
  protected:
    Unit(
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity,
      HitPoints startHitPoints,
      bool startRadarActive,
      bool startSonarActive
    );
    Unit(
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity
    ); /* This is a simpler version of the above constructor for when you don't
          need to specify the extra data */
  public:
    virtual ~Unit() {}
  protected:
    UnitTypeID type;
    uint32 unitId;
    Point<sint32> position; /* this maybe needs a 'magic value' for when the
                               unit is a subunit of another unit */
    Orientation orientation;
    Point<sint16> velocity; /* velocity is in distance-units per frame */
    
    /* status stuff */
    HitPoints hitPoints;
    bool radarIsActive;
    bool sonarIsActive;
    std::list<Unit*> subunits; /* TODO: think hard about this.  What happens
				  when the subunits belong to another player?
				  What happens when this Unit is serialized? */
    std::list<Weapon> weapons;
    /* conditioned orders */
    Order orders[orderCondition_max];
    Order currentOrder; /* The last order given - the one which awaits its
			   success or failure */
    /* current goals which have been set by orders */
    LinearTargetType linearTarget; /* Whether we're currently trying to
                                      achieve a position or a velocity */
    Point<sint32> targetPosition; /* The position we're trying to reach */
    Point<sint16> targetVelocity; /* The velocity we're trying to achieve */
    RotationalTargetType rotationalTarget;
    Orientation targetOrientation; /* The orientation we're trying to reach */
    AngularVelocity targetAngularVelocity; /* The angular velocity we're trying
					      to achieve */

    void initializeWeapons(const UnitType* typePtr, const Universe* universe);
  public:
    /* accessors */
    inline UnitTypeID getType(void) const {return type;}
    inline const UnitType* getTypePtr(void) const;
    inline uint32 getId(void) const {return unitId;}
    inline void setId(uint32 id) {unitId = id;}
    inline const Point<sint32>& getPosition(void) const {return position;}
    inline void setPosition(const Point<sint32>& p) { position = p; }
    inline const Orientation& getOrientation(void) const {return orientation;}
    inline void setOrientation(const Orientation& o) { orientation = o; }
    inline const Point<sint16>& getVelocity(void) const {return velocity;}
    inline void setVelocity(const Point<sint16>& v) { velocity = v; }
    inline void zeroVelocity(void) { velocity.zero(); }
    inline HitPoints getHitPoints(void) const {return hitPoints;}
    inline bool isRadarActive(void) const {return radarIsActive;}
    inline void setRadarActive(bool active) { radarIsActive = active; }
    inline bool isSonarActive(void) const {return sonarIsActive;}
    inline void setSonarActive(bool active) { sonarIsActive = active; }

    inline const Order& getOrder(OrderCondition c) const {
      assert(c>=0);
      assert(c<orderCondition_max);
      return orders[c];
    }
    inline void setOrder(OrderCondition c, const Order& order) {
      assert(c>=0);
      assert(c<orderCondition_max);
      orders[c] = order;
    }
    inline const Order& getCurrentOrder(void) { return currentOrder; }
    inline void setCurrentOrder(const Order& order) { currentOrder = order; }
    inline LinearTargetType getLinearTarget(void) { return linearTarget; }
    inline void setLinearTarget(LinearTargetType l) { linearTarget = l; }
    inline const Point<sint32>& getTargetPosition(void) {
      return targetPosition;
    }
    inline void setTargetPosition(const Point<sint32>& t) {
      targetPosition = t;
    }
    inline const Point<sint16>& getTargetVelocity(void) {
      return targetVelocity;
    }
    inline void setTargetVelocity(const Point<sint16>& t) {
      targetVelocity = t;
    }
    inline RotationalTargetType getRotationalTarget(void) const {
      return rotationalTarget;
    }
    inline const Orientation& getTargetOrientation(void) const {
      return targetOrientation;
    }
    inline const AngularVelocity& getTargetAngularVelocity(void) const {
      return targetAngularVelocity;
    }
};

}

#endif // LIBSAKUSEN__UNIT_H

