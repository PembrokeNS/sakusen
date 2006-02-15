#ifndef LIBSAKUSEN__UNIT_H
#define LIBSAKUSEN__UNIT_H

#include "libsakusen-global.h"

#include "iunitstatus.h"

namespace sakusen {

class LIBSAKUSEN_API UnitStatus : public IUnitStatus {
  private:
    UnitStatus(); /**< Default constructor should not be used */
    UnitStatus(
        UnitTypeID t,
        const Point<sint32>& p,
        const Orientation& o,
        const Point<sint16>& v,
        /* status stuff */
        HitPoints hP,
        bool rIA,
        bool sIA,
        const std::list<uint32>& su,
        const std::list<Weapon>& w,
        /* conditioned orders */
        Order ord[orderCondition_max],
        const Order& cO,
        /* current goals which have been set by orders */
        const LinearTargetType& lT,
        const Point<sint32>& tP,
        const Point<sint16>& tV,
        const RotationalTargetType& rT,
        const Orientation& tO,
        const AngularVelocity& tAV
      );
  public:
    UnitStatus(const IUnitStatus* copy);
    UnitStatus(
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity,
      HitPoints startHitPoints,
      bool startRadarActive,
      bool startSonarActive
    );
    UnitStatus(
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity
    ); /* This is a simpler version of the above constructor for when you don't
          need to specify the extra data */
    UnitStatus(
      const Universe* universe,
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity
    ); /* This is a variant of the above constructor for when the world does
          not exist yet, so the universe must be provided directly */
  public:
    UnitTypeID type;
    Point<sint32> position; /* this maybe needs a 'magic value' for when the
                               unit is a subunit of another unit */
    Orientation orientation;
    Point<sint16> velocity; /* velocity is in distance-units per frame */
    
    /* status stuff */
    HitPoints hitPoints;
    bool radarIsActive;
    bool sonarIsActive;
    std::list<uint32> subunits; /* ids of subunits which must belong to the
                                   same player as this unit */
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
    inline const Point<sint32>& getPosition(void) const {return position;}
    inline const Orientation& getOrientation(void) const {return orientation;}
    inline const Point<sint16>& getVelocity(void) const {return velocity;}
    inline HitPoints getHitPoints(void) const {return hitPoints;}
    inline bool isRadarActive(void) const {return radarIsActive;}
    inline bool isSonarActive(void) const {return sonarIsActive;}

    inline const Order& getOrder(OrderCondition c) const {
      assert(c>=0);
      assert(c<orderCondition_max);
      return orders[c];
    }
    inline const Order& getCurrentOrder(void) const { return currentOrder; }
    inline LinearTargetType getLinearTarget(void) const { return linearTarget; }
    inline const Point<sint32>& getTargetPosition(void) const {
      return targetPosition;
    }
    inline const Point<sint16>& getTargetVelocity(void) const {
      return targetVelocity;
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

    typedef Universe loadArgument;
    void store(OArchive&, const Universe*) const;
    static UnitStatus load(IArchive&, const loadArgument*);
};

}

#endif // LIBSAKUSEN__UNIT_H

