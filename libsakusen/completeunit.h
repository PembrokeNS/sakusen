#ifndef COMPLETEUNIT_H
#define COMPLETEUNIT_H

#include "libsakusen-global.h"

#include <list>
#include "point.h"
#include "unittype.h"
#include "unitstatus.h"
#include "icompleteunit.h"

namespace sakusen {

class LIBSAKUSEN_API CompleteUnit : public ICompleteUnit {
  private:
    CompleteUnit(); /**< Default constructor should not be used */
    CompleteUnit(
        uint32 uI,
        const UnitStatus& st,
        HitPoints mHP,
        uint8 m,
        const Point<uint32>& s,
        const Region<sint16>& pA,
        const Region<sint16>& pV,
        const Region<sint16>& pAV,
        const Visibility& visib,
        const Sensors& visio
      );
  public:
    CompleteUnit(const ICompleteUnit* copy);
  private:
    uint32 unitId;
    UnitStatus status;
    
    /* modifiable stuff from UnitType */
    HitPoints maxHitPoints;
    uint8 mass;
    Point<uint32> size;
    Region<sint16> possibleAccelerations;
    Region<sint16> possibleVelocities;
    Region<sint16> possibleAngularVelocities;
    Visibility visibility;
    Sensors vision;

  public:
    /* accessors */
    inline uint32 getId(void) const {return unitId;}
    inline IUnitStatus* getStatus(void) { return &status; }
    inline const IUnitStatus* getStatus(void) const { return &status; }
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

    typedef Universe loadArgument;
    void store(OArchive&) const;
    static CompleteUnit load(IArchive&, const loadArgument*);
};

}

#endif // COMPLETEUNIT_H

