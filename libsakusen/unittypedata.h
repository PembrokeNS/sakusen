#ifndef UNITTYPEDATA_H
#define UNITTYPEDATA_H

#include "iarchive.h"
#include "oarchive.h"
#include "iunittypedata.h"

namespace sakusen {

class LIBSAKUSEN_API UnitTypeData : public IUnitTypeData {
  private:
    UnitTypeData();
  public:
    UnitTypeData(
        HitPoints mHP,
        uint8 m,
        const Point<uint32>& s,
        const Region<sint16>::Ptr& pA,
        const Region<sint16>::Ptr& pV,
        const Region<sint16>::Ptr& pAV,
        const Visibility& visib,
        const Sensors& visio
      );
    UnitTypeData(const IUnitTypeData* copy);

    HitPoints maxHitPoints;
    uint8 mass;
    Point<uint32> size;
    /* TODO: armour */
    Region<sint16>::ConstPtr possibleAccelerations;
    Region<sint16>::ConstPtr possibleVelocities;
    Region<sint16>::ConstPtr possibleAngularVelocities;
    Visibility visibility;
    Sensors vision;
    
    inline HitPoints getMaxHitPoints(void) const { return maxHitPoints; }
    inline uint8 getMass(void) const {return mass;}
    inline const Point<uint32>& getSize(void) const {return size;}
    inline Region<sint16>::ConstPtr getPossibleAccelerations() const {
      return possibleAccelerations;
    }
    inline Region<sint16>::ConstPtr getPossibleVelocities() const {
      return possibleVelocities;
    }
    inline Region<sint16>::ConstPtr getPossibleAngularVelocities() const {
      return possibleAngularVelocities;
    }
    inline const Visibility& getVisibility(void) const {return visibility;}
    inline const Sensors& getVision(void) const {return vision;}

    void store(OArchive&) const;
    static UnitTypeData load(IArchive&);
};

}

#endif // UNITTYPEDATA_H

