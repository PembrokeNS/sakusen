#ifndef COMPLETEUNIT_H
#define COMPLETEUNIT_H

#include "libsakusen-global.h"

#include <list>
#include "point.h"
#include "unittype.h"
#include "visibility.h"
#include "orientation.h"
#include "order.h"
#include "ordercondition.h"
#include "iunit.h"
#include "world.h"

namespace sakusen {

class LIBSAKUSEN_API CompleteUnit : public IUnit {
  private:
    CompleteUnit(); /**< Default constructor should not be used */
  public:
    CompleteUnit(const IUnit* copy);
    CompleteUnit(
        UnitTypeID t,
        uint32 uI,
        const Point<sint32>& p,
        const Orientation& o,
        const Point<sint16>& v,
        HitPoints mHP,
        uint8 m,
        const Point<uint32>& s,
        const Region<sint16>& pA,
        const Region<sint16>& pV,
        const Region<sint16>& pAV,
        const Visibility& visib,
        const Sensors& visio,
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
  private:
    UnitTypeID type;
    uint32 unitId;
    Point<sint32> position; /* this maybe needs a 'magic value' for when the
                               unit is a subunit of another unit */
    Orientation orientation;
    Point<sint16> velocity; /* velocity is in distance-units per frame */
    
    /* modifiable stuff from UnitType */
    HitPoints maxHitPoints;
    uint8 mass;
    Point<uint32> size;
    Region<sint16> possibleAccelerations;
    Region<sint16> possibleVelocities;
    Region<sint16> possibleAngularVelocities;
    Visibility visibility;
    Sensors vision;
    /* status stuff */
    HitPoints hitPoints;
    bool radarIsActive;
    bool sonarIsActive;
    std::list<uint32> subunits;
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

  public:
    /* accessors */
    inline UnitTypeID getType(void) const {return type;}
    inline const UnitType* getTypePtr(void) const {
      return world->getUniverse()->getUnitTypePtr(type);
    }
    inline uint32 getId(void) const {return unitId;}
    inline const Point<sint32>& getPosition(void) const {return position;}
    inline const Orientation& getOrientation(void) const {return orientation;}
    inline const Point<sint16>& getVelocity(void) const {return velocity;}
    inline HitPoints getHitPoints(void) const {return hitPoints;}
    
    inline HitPoints getMaxHitPoints(void) const { return maxHitPoints; }
    inline uint8 getMass(void) const {return mass;}
    inline const Point<uint32>& getSize(void) const {return size;}
    inline const Region<sint16>& getPossibleAccelerations() const {
      return possibleAccelerations;
    }
    inline const Region<sint16>& getPossibleVelocities() const {
      return possibleVelocities;
    }
    inline const Region<sint16>& getPossibleAngularVelocities() const {
      return possibleAngularVelocities;
    }
    inline const Visibility& getVisibility(void) const {return visibility;}
    inline const Sensors& getVision(void) const {return vision;}
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
    void store(OArchive&) const;
    static CompleteUnit load(IArchive&, const loadArgument*);
};

}

#endif // COMPLETEUNIT_H

